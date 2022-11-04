#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

class Scanner
{
public:
	Scanner();
	void getTokenTable();
	void getSymbolTable();
	void getPIFTable();
	void getErrors();
	void createProgramInternalForm(string);
	void createSymbolTable(string);
	void scanFile(string);

private:
	map<string, int> tokenTable;
	map<string, int> symbolTable;
	vector<string> errorsList;

	struct PIFElement {
		string currentData;
		int tokenTablePos;
		int symbolTablePos;
	};
	vector<PIFElement> pifTable;
};

