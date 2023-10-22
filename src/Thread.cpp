/*
 * Thread.cpp
 *
 *  Created on: Jun 28, 2019
 *      Author: Mihailo Mladenovic
 */

#include "Thread.h"

#include "PCB.h"
#include <stdio.h>

Thread::Thread(StackSize stackSize, Time timeSlice) {
	myPCB = new PCB(stackSize, timeSlice, this);
}

Thread:: ~Thread() {
	delete myPCB;
}

ID Thread::getId() {
	return myPCB->getID();
}

ID Thread::getRunningId() {
	return PCB::getRunningId();
}

Thread* Thread::getThreadById(ID id) {
	return PCB::getThreadById(id);
}

void Thread::waitToComplete() {
	myPCB->waitToComplete();
}

void Thread::start() {
	myPCB->start();
}

void dispatch() {
	sys_call_dispatch();
}

void Thread::signal(SignalId sig) {
	asm pushf
	asm cli;
	if (sig != 1 && sig != 2) myPCB->signal(sig);
	else {
		if (!myPCB->parent) myPCB->signal(sig);
	}
	asm popf
}

void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	asm pushf
	asm cli;
	myPCB->registerHandler(signal, handler);
	asm popf
}

void Thread::unregisterAllHandlers(SignalId id) {
	asm pushf
	asm cli;
	myPCB->unregisterAllHandlers(id);
	asm popf
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	asm pushf
	asm cli;
	myPCB->swap(id, hand1, hand2);
	asm popf
}

void Thread::blockSignal(SignalId signal) {
	asm pushf
	asm cli;
	myPCB->blockSignal(signal);
	asm popf
}

void Thread::blockSignalGlobally(SignalId signal) {
	asm pushf
	asm cli;
	PCB::blockSignalGlobally(signal);
	asm popf
}

void Thread::unblockSignal(SignalId signal) {
	asm pushf
	asm cli;
	myPCB->unblockSignal(signal);
	asm popf
}

void Thread::unblockSignalGlobally(SignalId signal) {
	asm pushf
	asm cli;
	PCB::unblockSignalGlobally(signal);
	asm popf
}
