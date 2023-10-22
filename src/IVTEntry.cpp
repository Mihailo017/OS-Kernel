/*
 * IVTEntry.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: Mihailo Mladenovic
 */

#include "IVTEntry.h"
#include "dos.h"
#include <iostream.h>
#include "Event.h"
#include "KernelEv.h"


KernelEv* IVTEntry::events[256];
pInterrupt IVTEntry::routines[256];

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt intrRout) {
	this->ivtNo = ivtNo;
	IVTEntry::routines[ivtNo] = intrRout;
}

IVTEntry::~IVTEntry() {

}

void IVTEntry::callOld() {
	events[ivtNo]->oldRout();
}

void IVTEntry::signal() {
	if (IVTEntry::events[ivtNo]) IVTEntry::events[ivtNo]->KernelEv::signal();
}
