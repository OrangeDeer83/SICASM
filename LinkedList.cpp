#include "LinkedList.h"

bool LinkedList::IsEmpty() {
	return (first == nullptr);
}

void LinkedList::add(int data) {
	if (IsEmpty()) {
		first = new LinkedNode();
		first->next = new LinkedNode(data);
	}
	else {
		LinkedNode *cur = first;
		while (cur->next) {
			cur = first->next;
		}
		cur->next = new LinkedNode(data);
	}
}

int LinkedList::pop() {
	if (first->next == nullptr) {
		return -1;
	}
	else {
		LinkedNode* cur = first->next;
		first->next = first->next->next;
		int a = cur->data;
		delete cur;
		return a;
	}
}

void LinkedList::setFirst(int data) {
	LinkedNode node(data);
	if (IsEmpty()) {
		first = &node;
	}
	else {
		LinkedNode cur = *first;
		while (cur.next) {
			cur = *first->next;
		}
		cur.next = &node;
	}
}

int LinkedList::getFirst() {
	if (IsEmpty()) {
		return -1;
	}
	else {
		return first->data;
	}
}

bool LinkedList::found() {
	return (first->data != -1);
}