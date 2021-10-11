#ifndef LINKEDNODE_H
#define LINKEDNODE_H

class LinkedNode {
public:
	LinkedNode() : data(-1), next(nullptr) {};
	LinkedNode(int data) : data(data), next(nullptr) {};
	LinkedNode(int data, LinkedNode* next) : data(data), next(next) {};

	friend class LinkedList;
private:
	int data;
	LinkedNode* next;
};

#endif //LINKEDNODE_H