/*
 * KernelEv.cpp
 *
 *  Created on: Aug 24, 2019
 *      Author: Mihaiilo Mladenovic
 */

#include "KernelEv.h"
#include "SCHEDULE.H"
#include "PCB.h"
#include "timer.h"
#include "IVTEntry.h"
#include "dos.h"




KernelEv::KernelEv(IVTNo ivtNo) {
	this->ivtNo = ivtNo;
	IVTEntry::events[ivtNo] = this;
	owner = (PCB*)PCB::running;
	blocked = 0;
	val = 0;
	#ifndef BCC_BLOCK_IGNORE
	oldRout = getvect(ivtNo);
	setvect(ivtNo, IVTEntry::routines[ivtNo]);
	#endif
}

KernelEv::~KernelEv() {
	IVTEntry::events[ivtNo] = 0;
	#ifndef BCC_BLOCK_IGNORE
	setvect(ivtNo, oldRout);
	#endif
}

void KernelEv::wait() {
	if (PCB::running == owner) {
		if (!val) {
			owner->state = BLOCKED;
			blocked = owner;
			sys_call_block();
		}
		else val = 0;
	}
}

void KernelEv::signal() {
	if (!blocked) val = 1;
	else {
		blocked->state = READY;
		Scheduler::put(blocked);
		blocked = 0;
		dispatch();
	}
}
