/*
 * event.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: Mihailo Mladenovic
 */

#include "Event.h"
#include "PCB.h"
#include "SCHEDULE.H"
#include "timer.h"
#include "IVTEntry.h"
#include "KernelEv.h"
#include <iostream.h>

Event::Event(IVTNo ivtNo) {
	asm pushf
	asm cli
	myImpl = new KernelEv(ivtNo);
	asm popf
}

Event::~Event() {
	asm pushf
	asm cli
	delete myImpl;
	asm popf
}

void Event::wait() {
	asm pushf
	asm cli
	myImpl->wait();
	asm popf
}

void Event::signal() {
	asm pushf
	asm popf
	myImpl->signal();
	asm cli
}


