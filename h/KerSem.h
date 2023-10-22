/*
 * KernelSem.h
 *
 *  Created on: Aug 25, 2019
 *      Author: Mihailo Mladenovic
 */

#ifndef KERSEM_H_
#define KERSEM_H_

#include "semaphor.h"
#include "PCB.h"
#include "List.h"
#include "WaitList.h"

class KernelSem {

private:
	int val;
	List* blocked;
	WaitList* waiting;

	void block();
	void block(Time);
	int deblock(int);

public:
	KernelSem(int);
	~KernelSem();

	static void timeBlockedProcessing(); /* Javna staticka metoda koja se poziva iz timer i
	* koja smanjuje vreme cekanja za sve time blocked niti i deblokira ih ako je to
	* potrebno */

	static List* semsWithWaiting;

	int wait (Time maxTimeToWait);
	int signal(int n);

	int value() const;
};

#endif /* KERSEM_H_ */
