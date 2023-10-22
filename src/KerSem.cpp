/*
 * KernelSem.cpp
 *
 *  Created on: Aug 25, 2019
 *      Author: Mihailo Mladenovic
 */

#include "KerSem.h"
#include "semaphor.h"
#include "PCB.h"
#include "SCHEDULE.H"
#include "timer.h"
#include <iostream.h>



// Inicijalizacija statickih prom

List* KernelSem::semsWithWaiting = new List();


void KernelSem::timeBlockedProcessing() {
	asm pushf
	asm cli;

	for (semsWithWaiting->currentToHead(); semsWithWaiting->hasCurrent();) {
		KernelSem* cur = (KernelSem*)(semsWithWaiting->getCurrent());
		cur->waiting->decTime();
		PCB* temp;
		while ((temp = (PCB*)(cur->waiting->takeDoneWaiting()))) {
			temp->state = READY_ATB;
			Scheduler::put(temp);
		}
		if (cur->waiting->isEmpty()) semsWithWaiting->deleteCurrent();
		else semsWithWaiting->next();
	}

	asm popf
}

KernelSem::KernelSem(int val) {
	this->val = val;
	blocked = new List();
	waiting = new WaitList();
}

KernelSem::~KernelSem() {
	signal(val);
	delete blocked;
	delete waiting;
}

void KernelSem::block() {
	blocked->addTail((PCB*)PCB::running);
	PCB::running->state = BLOCKED;
	sys_call_block();
}


void KernelSem::block(Time time) {
	if (waiting->isEmpty()) semsWithWaiting->addHead(this);
	waiting->add((PCB*)PCB::running, time);
	PCB::running->state = BLOCKED;
	sys_call_block();
}

int KernelSem::deblock(int toDeblock) {
	int deBlocked = 0; // Broj odblokiranih niti
	while (toDeblock) { // Dok ima niti za odblokiranje
		PCB* temp;
		if ((temp = (PCB*)(blocked->takeHead()))) { // Ako ima niti koje su blokirane bez vremenskog ogranicenja


			temp->state = READY; // Stavi u Scheduler
			Scheduler::put(temp);


		} else {
				if ((temp = (PCB*)(waiting->takeTail()))) {
					temp->state = READY;
					Scheduler::put(temp);
				} else /* Nema dovoljno blokiranih niti, samo izadji iz while i vrati broj niti
				* koje su uspesno odblokirane po definicije funkcije signal */
					break;
		}
		toDeblock--;
		deBlocked++;
	}
	return deBlocked;
}

int KernelSem::wait(Time maxTimeToWait) {
	if (--val < 0)
		maxTimeToWait ? block(maxTimeToWait) : block() ;
	return PCB::running->state == READY_ATB ? PCB::running->state = READY, 0 : 1;
}

int KernelSem::signal(int n) {
	int ret;
	if (n < 0) ret = n;
	else
		if (!n) {
			if (val++ < 0) deblock(1);
			ret = 0;
		} else {
			val += n;
			ret = deblock(n);
		}
	return ret;
}

int KernelSem::value() const {
	return val;
}
