/*
 * WaitList.h
 *
 *  Created on: Sep 29, 2019
 *      Author: Mihailo Mladenovic
 */

#ifndef WAITLIST_H_
#define WAITLIST_H_

#include "List.h"
#include "PCB.h"
#include "Thread.h"

class WaitList {

	struct WaitNode {
		void* data;
		Time waitingTime;
		WaitNode(void* data, Time waitingTime) {
			this->data = data;
			this->waitingTime = waitingTime;
		}
	};

	List* waitList;
	int waitingTimeSum;

public:
	static int totalWait;
	WaitList();
	~WaitList();
	void add(void* data, Time waitingTime);
	void decTime();
	void* takeDoneWaiting();
	int isEmpty() {
		return waitList->isEmpty();
	}
	void* takeTail();

	// Za testiranje
	void ispis();
};

#endif /* WAITLIST_H_ */
