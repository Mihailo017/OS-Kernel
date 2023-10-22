/*
 * Semaphor.cpp
 *
 *  Created on: Jul 6, 2019
 *      Author: Mihailo Mladenovic
 */

#include "KerSem.h"
#include "semaphor.h"
#include "PCB.h"
#include "SCHEDULE.H"
#include "timer.h"

Semaphore::Semaphore (int init) {
	asm pushf
	asm cli;
	myImpl = new KernelSem(init);
	asm popf
}

Semaphore::~Semaphore () {
	asm pushf
	asm cli;
	delete myImpl;
	asm popf
}

int Semaphore::wait (Time maxTimeToWait) {
	int ret;
	asm pushf
	asm cli;
	ret = myImpl->wait(maxTimeToWait);
	asm popf;
	return ret;
}

int Semaphore::signal(int n) {
	int ret;
	asm pushf
	asm cli;
	ret = myImpl->signal(n);
	asm popf;
	return ret;
}

int Semaphore::val () const {
	int ret;
	asm pushf
	asm cli;
	ret = myImpl->value();
	asm popf;
	return ret;
}
