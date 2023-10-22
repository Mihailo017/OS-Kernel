/*
 * List.cpp
 *
 *  Created on: Sep 27, 2019
 *      Author: Mihailo Mladenovic
 */

#include "List.h"

/*int List::totalList = 0;
void* List::operator new(size_t s) {
	totalList++;
	void* p = ::new List();
	return p;
}

void List::operator delete(void* p) {
	totalList--;
	free(p);
}*/

List::List() {
	head = tail = cur = 0;

}

List::~List() {
	emptyList();

}

void List::addHead(void* data) { // Sigurno radi
	Node* temp = new Node(data);
	temp->next = head;
	head = temp;
	if (!tail) tail = head;
}

void List::addTail(void* data) { // Sigurno radi
	tail = (!head ? head : tail->next) = new Node(data);
}

void List::addBeforeCurrent(void* data) { // Sigurno radi
	Node* temp;
	if (cur) {
		temp = new Node(data);
		temp->next = cur->next;
		temp->data = cur->data;
		cur->next = temp;
		cur->data = data;
		if (tail == cur) tail = temp;
		cur = cur->next;
	} else
		if (!head) head = tail = cur = new Node(data);
}

void List::currentToHead() {
	cur = head;
}

void List::next() {
	if (cur) cur = cur->next;
}

int List::hasCurrent() {
	return cur != 0;
}

void* List::getCurrent() {
	return cur ? cur->data : 0;
}

void* List::takeHead() { // Sigurno radi
	void* ret = 0;
	if (head) {
		Node* temp = head;
		head = temp->next;
		ret = temp->data;
		if (cur == temp) cur = head;
		if (!head) tail = 0;
		delete temp;
	}
	return ret;
}

void* List::takeCurrent() { // Sigurno radi
	void* ret = 0;
	if (cur) {
		ret = cur->data;
		deleteCurrent();
	}
	return ret;
}

void* List::takeTail() { // Sigurno radi
	Node *temp1 = cur, *temp2 = tail;
	cur = tail;
	void* ret = takeCurrent();
	if (temp1 != temp2) cur = temp1;
	return ret;
}

int List::isEmpty() {return head == 0;}

void List::deleteCurrent() { // Sigurno radi
	if (!cur) return;
	Node* temp;
	if (cur->next) {
		temp = cur->next;
		cur->data = temp->data;
		cur->next = temp->next;
		if (tail == temp) tail = cur;
		delete temp;
	} else {
		temp = head;
		while (temp->next != tail) {
			temp = temp->next;
			if (!temp) break;
		}
		if (temp) {
			tail = temp;
			temp->next = 0;
		} else head = tail = 0;
		delete cur;
		cur = 0;
	}
}

void List::emptyList() { // Sigurno radi
	while (head) {
		tail = head;
		head = head->next;
		delete tail;
	}
	head = tail = cur = 0;
}

void List::ispis() { // Samo za integer
	cur = head;
	if (!cur) cout << "Lista je prazna" << endl;
	while (cur) {
		cout << *(int*)(cur->data) << endl;
		cur = cur->next;
	}
}
