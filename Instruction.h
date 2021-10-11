#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <iostream>
#include <string>
using std::string;

class Ins {
public:
	Ins() : label(""), ins(""), par("") {};
	Ins(string);
	void setAll(string);
	string getLabel();
	string getIns();
	string getPar();
	friend std::ostream& operator <<(std::ostream&, const Ins&);
private:
	string label, ins, par;
};


#endif //INSTRUCTION_H
