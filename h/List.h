/*
 * List.h
 *
 *  Created on: Sep 27, 2019
 *      Author: Mihailo Mladenovic
 */

#ifndef LIST_H_
#define LIST_H_

#include <iostream.h>
#include <stdlib.h>


class List {

	struct Node {
		void* data;
		Node* next;
		Node(void* data) {
			this->data = data;
			next = 0;
		}
	};
public: // Samo za potrebe testiranja
	Node *head, *tail, *cur;

public:

	/*static int totalList;
	void* operator new(size_t);
	void operator delete(void*);*/

	List();

	~List();

	void addHead(void* data);

	void addTail(void* data);

	void addBeforeCurrent(void* data);

	void currentToHead();

	void next();

	int hasCurrent();

	void* getCurrent();

	void deleteCurrent(); // Sigurno radi

	void* takeHead();

	void* takeCurrent();

	void* takeTail();

	int isEmpty();

	void emptyList(); // Sigurno radi

	void ispis();

};

#endif /* LIST_H_ */
