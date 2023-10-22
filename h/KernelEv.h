/*
 * KernelEv.h
 *
 *  Created on: Aug 24, 2019
 *      Author: Mihailo Mladenovic
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "Event.h"
#include "PCB.h"
#include "IVTEntry.h"


class KernelEv {

private:
	PCB* owner;
	PCB* blocked;
	unsigned char val;
	IVTNo ivtNo;

//protected:
public:
	//friend class IVTEntry;
	void signal();

//public:
	KernelEv(IVTNo ivtNo);
	~KernelEv();

	pInterrupt oldRout;

	void wait();
};

#endif /* KERNELEV_H_ */
