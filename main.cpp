#define _CRT_SECURE_NO_WARNINGS
#include "Instruction.h"
#include "LinkedList.h"
#include "opTable.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#define THREE_BYTE 3
#define NUMBER_OF_INS 58

using std::fstream;
using std::ios;
using std::vector;
using std::endl;
using std::cout;
using std::to_string;

int toDec(string);
bool T_is_full(int, string);
void T_new_line(string&, string&, int);

int main(int argc, char *argv[]) {
	fstream fin, fobj;
	string file = argv[1];
	string line;
	Ins ins;
	vector<string> sym;
	vector<LinkedList> val;
	bool index_addr = false, label_exist;
	int location = 0, location_p, start;
	string H_record, T_record, obj;
	char temp[8];

	fin.open(file + ".asm", ios::in);
	if (!fin) {
		std::cout << "The file isn't exist!!!";
	}
	fobj.open("test.obj", ios::out);

	

	if (argc == 2 || (argc == 3 && strcmp(argv[2], "-t") == 0)) {
		while (!fin.eof()) {
			getline(fin, line);
			ins.setAll(line);
			/*有label 頂*/
			if (ins.getLabel() != "") {
				/*ins為START 頂*/
				if (ins.getIns() == "START") {
					location = toDec(ins.getPar());
					start = location;
					location_p = location;
					H_record = "H" + ins.getLabel() + "\t";
					for (int i = ins.getPar().length(); i < 6; i++) {
						H_record += "0";
					}
					H_record += ins.getPar();
				}
				/*ins為START 底*/
				/*此行為註解*/
				if (ins.getLabel()[0] == '.') {
					continue;
				}
				/*註解以外的指令*/
				else {
					/*先處理label*/
					/*檢查label是否在symtab*/
					label_exist = false;
					for (int i = 0; i < sym.size(); i++) {
						/*label在symtab*/
						if (ins.getLabel() == sym[i]) {
							label_exist = true;
							if (val[i].found()) {
								cout << val[i].found();
								cout << "label重複寫入";
								system("pause");
								exit(-1);
							}
							/*label在symtab，且沒有位置，所以將symtab中的位置取出*/
							else {
								T_new_line(obj, T_record, location_p);
								location_p = location;
								int loc = val[i].pop();
								string for_temp;
								while (loc != -1) {
									sprintf(temp, "%06X", loc);
									for_temp = temp;
									sprintf(temp, "%02X", to_string(location).length() / 2);
									T_record += "T" + for_temp + temp;
									sprintf(temp, "%X\n", location);
									T_record += temp;
									loc = val[i].pop();
								}
								sym.erase(sym.begin() + i);
								val.erase(val.begin() + i);
								sym.push_back(ins.getLabel());
								LinkedList list = new LinkedNode(location);
								val.push_back(list);
							}
							break;
							/*label在symtab 底*/
						}
					}
					/*label沒有在symtab 頂*/
					if (!label_exist) {
						sym.push_back(ins.getLabel());
						LinkedList list = new LinkedNode(location);
						val.push_back(list);
					}
					/*label沒有在symtab 底*/
					/*再處理ins*/
					if (ins.getIns() == "START") {
						continue;
					}
					/*ins為WORD 頂*/
					if (ins.getIns() == "WORD") {
						if (ins.getPar().length() > 6) {
							cout << "WORD參數長度過長";
							system("pause");
							exit(-1);
						}
						else {
							if (T_is_full(ins.getPar().length(), obj)) {
								T_new_line(obj, T_record, location_p);
								location_p = location + THREE_BYTE;
							}
							sprintf(temp, "%06X", atoi(ins.getPar().c_str()));
							obj += temp;
							location += THREE_BYTE;
						}
					}
					/*ins為WORD 底*/
					/*ins為BYTE 頂*/
					else if (ins.getIns() == "BYTE") {
						string temp_s = strtok(&ins.getPar()[0], "'");
						/*par是C開頭 頂*/
						if (temp_s == "C") {
							temp_s = strtok(NULL, "'");
							if (T_is_full(temp_s.length() * 2, obj)) {
								T_new_line(obj, T_record, location_p);
								location_p = location + ins.getPar().length();
							}
							for (int i = 0; i < temp_s.length(); i++) {
								sprintf(temp, "%X", temp_s[i]);
								obj += temp;
							}
							location += temp_s.length();
						}
						/*par是C開頭 底*/
						/*par是X開頭 頂*/
						else if (temp_s == "X") {
							temp_s = strtok(NULL, "'");
							if (T_is_full(temp_s.length(), obj)) {
								T_new_line(obj, T_record, location_p);
								location_p = location + temp_s.length() / 2;
							}
							obj += temp_s;
							location += temp_s.length() / 2;
						}
						/*par是開頭 底*/
					}
					/*ins為BYTE 底*/
					/*ins為RESW 頂*/
					else if (ins.getIns() == "RESW") {
						if (obj != "") {
							T_new_line(obj, T_record, location_p);
						}
						location += atoi(ins.getPar().c_str()) * 3;
						location_p = location;
					}
					/*ins為RESW 底*/
					/*ins為RESB 頂*/
					else if (ins.getIns() == "RESB") {
						if (obj != "") {
							T_new_line(obj, T_record, location_p);
						}
						location += atoi(ins.getPar().c_str());
						location_p = location;
					}
					/*ins為RESB 底*/
					/*ins為其餘的opCode 頂*/
					else {
						int i = 0;
						for (; i < NUMBER_OF_INS; i++) {
							if (ins.getIns() == instruction[i]) {
								break;
							}
						}
						if (i == NUMBER_OF_INS) {
							cout << "INS有誤";
							system("pause");
							exit(-1);
						}
						if (T_is_full(THREE_BYTE * 2, obj)) {
							T_new_line(obj, T_record, location_p);
							location_p = location + THREE_BYTE;
						}
						index_addr = false;
						/*檢查是否indexed-addressing*/
						for (int j = 0; j < ins.getPar().length(); j++) {
							if (ins.getPar()[j] == ',') {
								index_addr = true;
								break;
							}
						}
						string par;
						if (index_addr) {
							par = strtok(&ins.getPar()[0], ",");
						}
						else {
							par = ins.getPar();
						}
						/*檢查par是否在symtab*/
						label_exist = false;
						int k = 0;
						for (; k < sym.size(); k++) {
							/*par在symtab*/
							if (par == sym[k]) {
								label_exist = true;
								/*par在symtab中有位址*/
								if (val[k].found()) {
									if (index_addr) {
										sprintf(temp, "%s%04X", &opCode[i][0], val[k].getFirst() + 32768);
									}
									else {
										sprintf(temp, "%s%04X", &opCode[i][0], val[k].getFirst());
									}
									obj += temp;
								}
								/*par在symtab中沒有位址*/
								else {
									if (index_addr) {
										sprintf(temp, "%s8000", &opCode[i][0]);
									}
									else {
										sprintf(temp, "%s0000", &opCode[i][0]);
									}
									obj += temp;
									val[k].add(location + 1);
								}
								break;
								/*par在symtab 底*/
							}
						}
						/*par沒有在symtab 頂*/
						if (!label_exist) {
							if (index_addr) {
								sprintf(temp, "%s8000", &opCode[i][0]);
							}
							else {
								sprintf(temp, "%s0000", &opCode[i][0]);
							}
							obj += temp;
							sym.push_back(par);
							LinkedList list = new LinkedNode();
							val.push_back(list);
							val[k].add(location + 1);
						}
						/*label沒有在symtab 底*/
						location += THREE_BYTE;
					}
					/*ins為其餘的opCode 底*/
				}
			}
			/*有label底*/
			/*沒有label 頂*/
			else {
				/*ins為END 頂*/
				if (ins.getIns() == "END") {
					T_new_line(obj, T_record, location_p);
					string E_record = "E";
					for (int i = 0; i < sym.size(); i++) {
						if (sym[i] == "FIRST") {
							sprintf(temp, "%06X", val[i].getFirst());
							break;
						}
					}
					E_record += temp;
					sprintf(temp, "%06X\n", location - start);
					H_record += temp;
					fobj << H_record << T_record << E_record;
					break;
				}
				/*ins為END 底*/
				/*ins為RSUB 頂*/
				else if (ins.getIns() == "RSUB") {
					if (T_is_full(THREE_BYTE * 2, obj)) {
						T_new_line(obj, T_record, location_p);
						location_p = location + THREE_BYTE;
					}
					obj += "4C0000";
					location += THREE_BYTE;
				}
				/*ins為RSUB 底*/
				/*ins為其餘的opCode 頂*/
				else {
					int i = 0;
					for (; i < NUMBER_OF_INS; i++) {
						if (ins.getIns() == instruction[i]) {
							break;
						}
					}
					if (i == NUMBER_OF_INS + 1) {
						cout << "INS有誤";
						system("pause");
						exit(-1);
					}
					if (T_is_full(THREE_BYTE * 2, obj)) {
						T_new_line(obj, T_record, location_p);
						location_p = location + THREE_BYTE;
					}
					index_addr = false;
					/*檢查是否indexed-addressing*/
					for (int j = 0; j < ins.getPar().length(); j++) {
						if (ins.getPar()[j] == ',') {
							index_addr = true;
							break;
						}
					}
					string par;
					if (index_addr) {
						par = strtok(&ins.getPar()[0], ",");
					}
					else {
						par = ins.getPar();
					}
					/*檢查par是否在symtab*/
					label_exist = false;
					int k = 0;
					for (; k < sym.size(); k++) {
						/*par在symtab*/
						if (par == sym[k]) {
							label_exist = true;
							/*par在symtab中有位址*/
							if (val[k].found()) {
								if (index_addr) {
									sprintf(temp, "%s%04X", &opCode[i][0], val[k].getFirst() + 32768);
								}
								else {
									sprintf(temp, "%s%04X", &opCode[i][0], val[k].getFirst());
								}
								obj += temp;
							}
							/*par在symtab中沒有位址*/
							else {
								if (index_addr) {
									sprintf(temp, "%s8000", &opCode[i][0]);
								}
								else {
									sprintf(temp, "%s0000", &opCode[i][0]);
								}
								obj += temp;
								val[k].add(location + 1);
							}
							/*par在symtab 底*/
						}
					}
					/*par沒有在symtab 頂*/
					if (!label_exist) {
						if (index_addr) {
							sprintf(temp, "%s8000", &opCode[i][0]);
						}
						else {
							sprintf(temp, "%s0000", &opCode[i][0]);
						}
						obj += temp;
						sym.push_back(par);
						LinkedList list = new LinkedNode();
						val.push_back(list);
						val[k].add(location + 1);
					}
					/*label沒有在symtab 底*/
					location += THREE_BYTE;
				}
				/*ins為其餘的opCode 底*/
			}
			/*沒有label 底*/
		}
	}
	else if (argc == 3) {
		if (strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "-a") == 0) {
			fstream flst;
			flst.open("test.lst", ios::out);
			flst << "Loc.\tSource statement\n=====\t==================================\n";
			while (!fin.eof()) {
				getline(fin, line);
				ins.setAll(line); 
				/*輸出.lst*/
				if (ins.getIns() == "START") {
					sprintf(temp, "%05X", toDec(ins.getPar()));
					flst << temp << "\t" << ins;
				}
				else if (ins.getLabel()[0] == '.') {
					flst << "\t" <<ins;
				}
				else if (ins.getIns() == "END") {
					flst << ins;
				}
				else {
					sprintf(temp, "%05X", location);
					flst << temp << "\t" << ins;
				}
				/*有label 頂*/
				if (ins.getLabel() != "") {
					/*ins為START 頂*/
					if (ins.getIns() == "START") {
						location = toDec(ins.getPar());
						start = location;
						location_p = location;
						H_record = "H" + ins.getLabel() + "\t";
						for (int i = ins.getPar().length(); i < 6; i++) {
							H_record += "0";
						}
						H_record += ins.getPar();
					}
					/*ins為START 底*/
					/*此行為註解*/
					if (ins.getLabel()[0] == '.') {
						continue;
					}
					/*註解以外的指令*/
					else {
						/*先處理label*/
						/*檢查label是否在symtab*/
						label_exist = false;
						for (int i = 0; i < sym.size(); i++) {
							/*label在symtab*/
							if (ins.getLabel() == sym[i]) {
								label_exist = true;
								if (val[i].found()) {
									cout << val[i].found();
									cout << "label重複寫入";
									system("pause");
									exit(-1);
								}
								/*label在symtab，且沒有位置，所以將symtab中的位置取出*/
								else {
									T_new_line(obj, T_record, location_p);
									location_p = location;
									int loc = val[i].pop();
									string for_temp;
									while (loc != -1) {
										sprintf(temp, "%06X", loc);
										for_temp = temp;
										sprintf(temp, "%02X", to_string(location).length() / 2);
										T_record += "T" + for_temp + temp;
										sprintf(temp, "%X\n", location);
										T_record += temp;
										loc = val[i].pop();
									}
									sym.erase(sym.begin() + i);
									val.erase(val.begin() + i);
									sym.push_back(ins.getLabel());
									LinkedList list = new LinkedNode(location);
									val.push_back(list);
								}
								break;
								/*label在symtab 底*/
							}
						}
						/*label沒有在symtab 頂*/
						if (!label_exist) {
							sym.push_back(ins.getLabel());
							LinkedList list = new LinkedNode(location);
							val.push_back(list);
						}
						/*label沒有在symtab 底*/
						/*再處理ins*/
						if (ins.getIns() == "START") {
							continue;
						}
						/*ins為WORD 頂*/
						if (ins.getIns() == "WORD") {
							if (ins.getPar().length() > 6) {
								cout << "WORD參數長度過長";
								system("pause");
								exit(-1);
							}
							else {
								if (T_is_full(ins.getPar().length(), obj)) {
									T_new_line(obj, T_record, location_p);
									location_p = location + THREE_BYTE;
								}
								sprintf(temp, "%06X", atoi(ins.getPar().c_str()));
								obj += temp;
								location += THREE_BYTE;
							}
						}
						/*ins為WORD 底*/
						/*ins為BYTE 頂*/
						else if (ins.getIns() == "BYTE") {
							string temp_s = strtok(&ins.getPar()[0], "'");
							/*par是C開頭 頂*/
							if (temp_s == "C") {
								temp_s = strtok(NULL, "'");
								if (T_is_full(temp_s.length() * 2, obj)) {
									T_new_line(obj, T_record, location_p);
									location_p = location + ins.getPar().length();
								}
								for (int i = 0; i < temp_s.length(); i++) {
									sprintf(temp, "%X", temp_s[i]);
									obj += temp;
								}
								location += temp_s.length();
							}
							/*par是C開頭 底*/
							/*par是X開頭 頂*/
							else if (temp_s == "X") {
								temp_s = strtok(NULL, "'");
								if (T_is_full(temp_s.length(), obj)) {
									T_new_line(obj, T_record, location_p);
									location_p = location + temp_s.length() / 2;
								}
								obj += temp_s;
								location += temp_s.length() / 2;
							}
							/*par是開頭 底*/
						}
						/*ins為BYTE 底*/
						/*ins為RESW 頂*/
						else if (ins.getIns() == "RESW") {
							if (obj != "") {
								T_new_line(obj, T_record, location_p);
							}
							location += atoi(ins.getPar().c_str()) * 3;
							location_p = location;
						}
						/*ins為RESW 底*/
						/*ins為RESB 頂*/
						else if (ins.getIns() == "RESB") {
							if (obj != "") {
								T_new_line(obj, T_record, location_p);
							}
							location += atoi(ins.getPar().c_str());
							location_p = location;
						}
						/*ins為RESB 底*/
						/*ins為其餘的opCode 頂*/
						else {
							int i = 0;
							for (; i < NUMBER_OF_INS; i++) {
								if (ins.getIns() == instruction[i]) {
									break;
								}
							}
							if (i == NUMBER_OF_INS) {
								cout << "INS有誤";
								system("pause");
								exit(-1);
							}
							if (T_is_full(THREE_BYTE * 2, obj)) {
								T_new_line(obj, T_record, location_p);
								location_p = location + THREE_BYTE;
							}
							index_addr = false;
							/*檢查是否indexed-addressing*/
							for (int j = 0; j < ins.getPar().length(); j++) {
								if (ins.getPar()[j] == ',') {
									index_addr = true;
									break;
								}
							}
							string par;
							if (index_addr) {
								par = strtok(&ins.getPar()[0], ",");
							}
							else {
								par = ins.getPar();
							}
							/*檢查par是否在symtab*/
							label_exist = false;
							int k = 0;
							for (; k < sym.size(); k++) {
								/*par在symtab*/
								if (par == sym[k]) {
									label_exist = true;
									/*par在symtab中有位址*/
									if (val[k].found()) {
										if (index_addr) {
											sprintf(temp, "%s%04X", &opCode[i][0], val[k].getFirst() + 32768);
										}
										else {
											sprintf(temp, "%s%04X", &opCode[i][0], val[k].getFirst());
										}
										obj += temp;
									}
									/*par在symtab中沒有位址*/
									else {
										if (index_addr) {
											sprintf(temp, "%s8000", &opCode[i][0]);
										}
										else {
											sprintf(temp, "%s0000", &opCode[i][0]);
										}
										obj += temp;
										val[k].add(location + 1);
									}
									break;
									/*par在symtab 底*/
								}
							}
							/*par沒有在symtab 頂*/
							if (!label_exist) {
								if (index_addr) {
									sprintf(temp, "%s8000", &opCode[i][0]);
								}
								else {
									sprintf(temp, "%s0000", &opCode[i][0]);
								}
								obj += temp;
								sym.push_back(par);
								LinkedList list = new LinkedNode();
								val.push_back(list);
								val[k].add(location + 1);
							}
							/*label沒有在symtab 底*/
							location += THREE_BYTE;
						}
						/*ins為其餘的opCode 底*/
					}
				}
				/*有label底*/
				/*沒有label 頂*/
				else {
					/*ins為END 頂*/
					if (ins.getIns() == "END") {
						T_new_line(obj, T_record, location_p);
						string E_record = "E";
						for (int i = 0; i < sym.size(); i++) {
							if (sym[i] == "FIRST") {
								sprintf(temp, "%06X", val[i].getFirst());
								break;
							}
						}
						E_record += temp;
						sprintf(temp, "%06X\n", location - start);
						H_record += temp;
						fobj << H_record << T_record << E_record;
						break;
					}
					/*ins為END 底*/
					/*ins為RSUB 頂*/
					else if (ins.getIns() == "RSUB") {
						if (T_is_full(THREE_BYTE * 2, obj)) {
							T_new_line(obj, T_record, location_p);
							location_p = location + THREE_BYTE;
						}
						obj += "4C0000";
						location += THREE_BYTE;
					}
					/*ins為RSUB 底*/
					/*ins為其餘的opCode 頂*/
					else {
						int i = 0;
						for (; i < NUMBER_OF_INS; i++) {
							if (ins.getIns() == instruction[i]) {
								break;
							}
						}
						if (i == NUMBER_OF_INS + 1) {
							cout << "INS有誤";
							system("pause");
							exit(-1);
						}
						if (T_is_full(THREE_BYTE * 2, obj)) {
							T_new_line(obj, T_record, location_p);
							location_p = location + THREE_BYTE;
						}
						index_addr = false;
						/*檢查是否indexed-addressing*/
						for (int j = 0; j < ins.getPar().length(); j++) {
							if (ins.getPar()[j] == ',') {
								index_addr = true;
								break;
							}
						}
						string par;
						if (index_addr) {
							par = strtok(&ins.getPar()[0], ",");
						}
						else {
							par = ins.getPar();
						}
						/*檢查par是否在symtab*/
						label_exist = false;
						int k = 0;
						for (; k < sym.size(); k++) {
							/*par在symtab*/
							if (par == sym[k]) {
								label_exist = true;
								/*par在symtab中有位址*/
								if (val[k].found()) {
									if (index_addr) {
										sprintf(temp, "%s%04X", &opCode[i][0], val[k].getFirst() + 32768);
									}
									else {
										sprintf(temp, "%s%04X", &opCode[i][0], val[k].getFirst());
									}
									obj += temp;
								}
								/*par在symtab中沒有位址*/
								else {
									if (index_addr) {
										sprintf(temp, "%s8000", &opCode[i][0]);
									}
									else {
										sprintf(temp, "%s0000", &opCode[i][0]);
									}
									obj += temp;
									val[k].add(location + 1);
								}
								/*par在symtab 底*/
							}
						}
						/*par沒有在symtab 頂*/
						if (!label_exist) {
							if (index_addr) {
								sprintf(temp, "%s8000", &opCode[i][0]);
							}
							else {
								sprintf(temp, "%s0000", &opCode[i][0]);
							}
							obj += temp;
							sym.push_back(par);
							LinkedList list = new LinkedNode();
							val.push_back(list);
							val[k].add(location + 1);
						}
						/*label沒有在symtab 底*/
						location += THREE_BYTE;
					}
					/*ins為其餘的opCode 底*/
				}
				/*沒有label 底*/
			}
			flst.close();
		}
		else {
			cout << "參數輸入錯誤";
			system("pause");
			exit(-1);
		}
	}
	else {
		cout << "參數輸入錯誤";
		system("pause");
		exit(-1);
	}
	if (argc == 3) {
		if (strcmp(argv[2], "-t") == 0 || strcmp(argv[2], "-a") == 0) {
			FILE* fstb;
			fstb = fopen("test.stb", "w+");
			fprintf(fstb, "Symbol\tValue\n=====\t=====\n");
			for (int i = 0; i < sym.size() - 1; i++) {
				fprintf(fstb, "%s\t%05X\n", &sym[i][0], val[i].getFirst());
			}
			fprintf(fstb, "%s\t%05X", &sym[sym.size() - 1][0], val[sym.size() - 1].getFirst());
			fclose(fstb);
		}
	}
	
	

	fin.close();
	fobj.close();

	system("PAUSE");
	return 0;
}

int toDec(string input) {
	int dec = 0;
	int length = input.length();
	for (int i = 0; i < length; i++) {
		dec += (input[i] - '0') * pow(16, length - i - 1);
	}

	return dec;
}


bool T_is_full(int add, string obj) {
	return (add + obj.length() > 60);
}
void T_new_line(string& obj, string& T_record, int location) {
	char temp[80];
	sprintf(temp, "T%06X%02X%s\n", location, obj.length() / 2, &obj[0]);
	string T = temp;
	T_record += T;
	obj = "";
}