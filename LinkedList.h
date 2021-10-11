#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "LinkedNode.h"

class LinkedList {
public:
	LinkedList() : first(nullptr) {};
	LinkedList(LinkedNode* first) : first(first) {};
	void add(int data);									//在尾端加入data為par位址的LinkedNode
	int pop();
	void setFirst(int data);
	int getFirst();
	bool found();
private:
	LinkedNode* first;
	bool IsEmpty();
};


#endif //LINKEDLIST_H