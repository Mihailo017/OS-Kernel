/*
 * WaitList.cpp
 *
 *  Created on: Sep 29, 2019
 *      Author: Mihailo Mladenovic
 */

#include "WaitList.h"
#include "Schedule.h"
#include <iostream.h>

int WaitList::totalWait = 0;

WaitList::WaitList() : waitingTimeSum(0) {
	totalWait++;
	waitList = new List();
}

WaitList::~WaitList() {
	totalWait--;
	delete waitList;
}


void WaitList::add(void* data, Time time) { // Sigurno radi
	WaitNode* node = new WaitNode(data, time);
	if (time >= waitingTimeSum) {
		node->waitingTime -= waitingTimeSum;
		waitingTimeSum += node->waitingTime;
		waitList->addTail(node);
	} else {
		int sum = 0;
		WaitNode* cur;
		for (waitList->currentToHead(); waitList->hasCurrent(); waitList->next()) {
			cur = (WaitNode*)(waitList->getCurrent());
			if (sum + cur->waitingTime > time) break;
			sum += cur->waitingTime;
		}
		node->waitingTime = time - sum;
		cur->waitingTime -= node->waitingTime;
		waitList->addBeforeCurrent(node);
	}
}

void WaitList::decTime() { // Sigurno radi
	if (!waitList->isEmpty()) {
		waitList->currentToHead();
		WaitNode* cur = (WaitNode*)(waitList->getCurrent());
		cur->waitingTime--;
		waitingTimeSum--;
	}
}

void* WaitList::takeDoneWaiting() { // Sigurno radi
	waitList->currentToHead();
	WaitNode* temp = (WaitNode*)(waitList->getCurrent());
	if (temp)
		if (!temp->waitingTime) {
			waitList->deleteCurrent();
			return temp->data;
		}
	return 0;
}

void* WaitList::takeTail() { // Sigurno radi
	WaitNode* temp = (WaitNode*)(waitList->takeTail());
	if (temp) {
	waitingTimeSum -= temp->waitingTime;
	return temp->data;
	}
	return 0;
}


void WaitList::ispis() { // Sigurno radi
	for (waitList->currentToHead(); waitList->hasCurrent(); waitList->next()) {
		WaitNode* cur = (WaitNode*)(waitList->getCurrent());
		cout << cur->waitingTime << endl;
	}
	cout << "Kraj liste" << endl;
}
