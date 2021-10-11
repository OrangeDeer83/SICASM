#define _CRT_SECURE_NO_WARNINGS
#include "Instruction.h"
#include "opTable.h"
#include <iostream>
#include <string>

Ins::Ins(string row) {
	if (row[0] == '.') {
		label = row;
		ins = "";
		par = "";
	}
	else if (row == "") {
		return;
	}
	else {
		string temp1 = strtok(&row[0], "\t");
		int type = 1;
		if (temp1 == "END") {
			label = "\t";
			ins = temp1 + "\t";
			par = strtok(NULL, "\0");
		}
		else {
			for (int i = 0; i < 59; i++) {
				if (!strcmp(&temp1[0], &instruction[i][0])) {
					type = 2;
					break;
				}
			}
			if (type == 1) {
				label = temp1 + "\t";
				ins = strtok(NULL, "\t");
				ins += "\t";
				par = strtok(NULL, "\0");
			}
			else if (type == 2) {
				label = "\t";
				ins = temp1 + "\t";
				if (temp1 == "RSUB") {
					par = "";
				}
				else
				{
					par = strtok(NULL, "\0");
				}
			}
			else {
				std::cout << "Wrong Instruction.";
				exit(-1);
			}
		}
	}
}

void Ins::setAll(string row) {
	if (row[0] == '.') {
		label = row;
		ins = "";
		par = "";
	}
	else if (row == ""){
		return;
	}
	else {
		string temp1 = strtok(&row[0], "\t");
		int type = 1;
		if (temp1 == "END") {
			label = "";
			ins = temp1 ;
			par = strtok(NULL, "\0");
		}
		else {
			for (int i = 0; i < 59; i++) {
				if (!strcmp(&temp1[0], &instruction[i][0])) {
					type = 2;
					break;
				}
			}
			if (type == 1) {
				label = temp1;
				ins = strtok(NULL, "\t");
				par = strtok(NULL, "\0");
			}
			else if (type == 2) {
				label = "";
				ins = temp1;
				if (temp1 == "RSUB") {
					par = "";
				}
				else
				{
					par = strtok(NULL, "\0");
				}
			}
			else {
				std::cout << "Wrong Instruction.";
				exit(-1);
			}
		}
	}
}

string Ins::getLabel() {
	return label;
}

string Ins::getIns() {
	return ins;
}

string Ins::getPar() {
	return par;
}

std::ostream& operator <<(std::ostream& outputstream, const Ins& i) {
	outputstream << i.label << "\t" << i.ins << "\t" << i.par << std::endl;
	return outputstream;
}