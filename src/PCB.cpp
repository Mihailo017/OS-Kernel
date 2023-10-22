/*
 * PCB.cpp
 *
 *  Created on: Jun 30, 2019
 *      Author: Mihailo Mladenovic
 */

#include "PCB.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <iostream.h>
#include <stdlib.h>
#include "Thread.h"

// Inicijalizacija konteksta glavne niti
Thread* PCB::mainThread = new Thread(defaultStackSize, 1);

// Postavljanje statickih promeniljivih
ID PCB::nextID = 0;
// Pocetna vrednost za running, zaprvo PCB u koji ce se cuvati kontekst main
volatile PCB* PCB::running = mainThread->myPCB;
// Glava liste svih PCB-ova
volatile PCBNode* PCB::readyListHead = 0;

// Postavljanje steka
void PCB::setup_stack(StackSize stackSize) {
	stack = new unsigned[stackSize /= sizeof(StackSize)];
	stack[stackSize - 1] = 0x200;
	#ifndef BCC_BLOCK_IGNORE
	stack[stackSize - 2] = FP_SEG(PCB::wrapper);
	stack[stackSize - 3] = FP_OFF(PCB::wrapper);
	ss = FP_SEG(stack + stackSize - 12);
	sp = FP_OFF(stack + stackSize - 12);
	bp = sp;
	#endif
}

// Konstruktori
PCB::PCB(StackSize stackSize, Time timeSlice, Thread* thread) {
	asm pushf
	asm cli

	id = nextID++;
	state = NEW;
	time = timeSlice;
	myThread = thread;
	waitToCompleteSem = new Semaphore(0);

	signals = new List();
	handlers = new List*[16];
	for (int i=0; i<16; i++) {
		handlers[i] = new List();
	}

	local_blocked_signals = 0;
	parent = myThread == mainThread ? 0 : (PCB*)running;
	registerHandler(0, PCB::finish);

	setup_stack(stackSize);

	addToList((PCBNode**)&PCB::readyListHead, this);
	// Azuriranje pokazivaca liste
	readyListPrev = 0;
	// Prethodnik drugog elementa liste je glava liste
	PCB::readyListHead->next->pcb->readyListPrev = PCB::readyListHead;

	asm popf
}

// Destruktor

PCB::~PCB() {
	asm pushf
	asm cli;

	if (readyListPrev) {
		PCBNode* thisNode = readyListPrev->next;
		readyListPrev->next = thisNode->next;
		delete thisNode;
	} else {
		PCBNode* thisNode = (PCBNode*)PCB::readyListHead;
		PCB::readyListHead = thisNode->next;
		delete thisNode;
	}

	delete signals;
	for (int i=0; i<16; i++)
		delete handlers[i];
	delete handlers;
	delete waitToCompleteSem;

	asm popf;
	delete (void*)stack;
}

// Privatni konstruktor za idle nit
PCB::PCB(void (*idle)()) {
	StackSize stackSize = defaultStackSize;
	id = -2; // Posebna vrednost za id idle niti
	time = 1; // Samo jedan kvant se izvrsava
	myThread = 0;
	state = READY;

	readyListPrev = 0;

	signals = new List();
	handlers = new List*[16];
	for (int i=0; i<16; i++) {
		handlers[i] = new List();
	}

	local_blocked_signals = 0;
	parent = 0;

	stack = new unsigned[stackSize /= sizeof(StackSize)];
	stack[stackSize - 1] = 0x200;
	#ifndef BCC_BLOCK_IGNORE
	stack[stackSize - 2] = FP_SEG(idle);
	stack[stackSize - 3] = FP_OFF(idle);
	ss = FP_SEG(stack + stackSize - 12);
	sp = FP_OFF(stack + stackSize - 12);
	bp = sp;
	#endif
}

// Idle nit

void PCB::idle() {
	while(1);
}

PCB* const PCB::idleThread = new PCB(PCB::idle);

// Geteri
ID PCB::getID() { return id; }

Thread* PCB::getThread() { return myThread; }

ID PCB::getRunningId() { return running ? running->id : -1; }

Thread* PCB::getThreadById(ID id) {
	asm pushf
	asm cli;

	Thread* thread = 0;
	if (PCB::readyListHead) {
		for (PCBNode* curr = (PCBNode*)readyListHead; curr; curr = curr->next)
			if (curr->pcb->id == id) {
				cout << "getThreadByID - if unutar for" << endl;
				asm cli;
				thread = curr->pcb->myThread;
				break;
			}
	}

	asm popf
	return thread;
}

// Metode za rad sa nitima
void PCB::waitToComplete() {

	asm pushf
	asm cli;

	if (state != FINISHED && id >= 0) {
		waitToCompleteSem->wait(0);
	}

	asm popf
}

void PCB::start() {
	asm pushf
	asm cli;

	if (state == NEW) {
		state = READY;
		Scheduler::put(this);
	}

	asm popf
}

// Metode za liste
void PCB::addToList(PCBNode** head, PCB* pcb) {
	if (head) {
		PCBNode* node = new PCBNode(pcb);
		node->next = *head;
		*head = node;
	} else
		*head = new PCBNode(pcb);
}

// Metoda wrapper koja poziva polimorfnu metodu run klase Thread
void PCB::wrapper() {

	PCB::running->myThread->run();

	asm cli
	PCB::finish();
	asm sti // Redundantno jer se ovde nikad nece vrati al ga neka za sad
}

// Dodato za signale

unsigned PCB::global_blocked_signals = 0;

void PCB::signal(SignalId id) {
	signals->addTail(new SignalId(id));
}

void PCB::registerHandler(SignalId signal, SignalHandler handler) {
	handlers[signal]->addTail(new SignalHandler(handler));
}

void PCB::unregisterAllHandlers(SignalId id) {
	handlers[id]->emptyList();
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	SignalHandler* handNodes[2];
	SignalHandler hands[2];
	handNodes[0] = handNodes[1] = 0;
	hands[0] = hand1;
	hands[1] = hand2;
	for (int i=0; i<2; i++) {
		for (handlers[id]->currentToHead(); handlers[id]->hasCurrent(); handlers[id]->next()) {
			handNodes[i] = (SignalHandler*)handlers[id]->getCurrent();
			if (*handNodes[i] == hands[i]) continue;
			else handNodes[i] = 0;
		}
	}
	if (handNodes[0] == 0 || handNodes[1] == 0) return;
	*handNodes[0] = hands[1];
	*handNodes[1] = hands[0];
}

void PCB::blockSignal(SignalId signal) { // Ako je bit jedan onda je blokiran
	local_blocked_signals |= (1 << signal);
}

void PCB::blockSignalGlobally(SignalId signal) {
	global_blocked_signals |= (1 << signal);
}

void PCB::unblockSignal(SignalId signal) {
	local_blocked_signals &= ~(1 << signal);
}

void PCB::unblockSignalGlobally(SignalId signal) {
	global_blocked_signals &= ~(1 << signal);
}

void PCB::signals_processing() {

	for ((PCB::running->signals)->currentToHead(); (PCB::running->signals)->hasCurrent();) {
		SignalId id = *((SignalId*)(PCB::running->signals)->getCurrent());
		if (!((PCB::running->local_blocked_signals >> id) & 1) && !((PCB::global_blocked_signals >> id) & 1)) {
			for ((PCB::running->handlers[id])->currentToHead(); (PCB::running->handlers[id])->hasCurrent(); (PCB::running->handlers[id])->next()) {
				SignalHandler handler = *((SignalHandler*)(PCB::running->handlers[id])->getCurrent());
				handler();
			}
			(PCB::running->signals)->deleteCurrent();
		} else {
			(PCB::running->signals)->next();
		}
	}
}

void PCB::killThread() {
	PCB::finish();
}

void PCB::finish() {
	// Ponovno stavljanje u Scheduler niti koje cekaju na tekucu da se zavrsi
		PCB::running->state = FINISHED;

		(PCB::running->waitToCompleteSem)->signal(-(PCB::running->waitToCompleteSem)->val());

		if (((PCB*)running)->parent) ((PCB*)running)->parent->signal(1);
		((PCB*)running)->signal(2);

		sys_call_block();
}
