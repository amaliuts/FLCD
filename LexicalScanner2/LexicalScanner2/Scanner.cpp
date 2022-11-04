#include "Scanner.h"
#include <vector>
#include <algorithm>

Scanner::Scanner()
{
	string tokenList[] = { "identifier", "constant", "int", "bool", "char", "string", "list", "if", "else", "while", "do", "for", "all", "read", "write", "true", "false",
				"begin_stmt", "end_stmt", "break", "print", "return", "between", "and", "void", "_", "+", "-", "*", "/", "=", "==", "<", "<=", ">=", ">", "&&", "^", "!", 
				"!=", "%", "[", "]", "{", "}", ":", ";", "(", ")", "||"};
	for (int i = 0; i < 50; i++) {
		tokenTable.insert(pair<string, int>(tokenList[i], i));
	}
}

bool isOperator(string str)
{
	string operatorsList[] = { "+", "-", "*", "/", "=", "<", "==", "<=", ">=", ">", "&&", "^", "!", "!=", "%", "||" };
	for (string val : operatorsList) {
		if (str == val)
			return true;
	}
	return false;
}

bool isSeparator(string str)
{
	string separatorsList[] = { "(", ")", "[", "]", "{", "}", ":", ";", "," };
	for (string val : separatorsList) {
		if (str == val)
			return true;
	}
	return false;
}

bool isKeyword(string str)
{
	string keywordsList[] = { "int", "char", "list", "bool", "const", "if", "else", "while", "do", "for", "all", "read", "write", "true", "false", "begin_stmt", "end_stmt",
			"break", "print", "return", "between", "and", "void" };
	for (string val : keywordsList) {
		if (str == val)
			return true;
	}
	return false;
}

bool isDigit(char ch)
{
	if (ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4'
		|| ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9')
		return true;
	return false;
}

bool isNumber(string str)
{
	int decimalsCount = 0;
	if (str.size() == 0)
		return false;
	for (int i = 0; i < str.size(); i++) {
		if (decimalsCount > 1 && str[i] == '.')
			return false;
		else if (decimalsCount <= 1)
			decimalsCount++;

		if (!isDigit(str[i]))
			return false;
	}
	return true;
}

bool isCharOrStringConstant(string str)
{
	if (!(str[0] > '!' && str[0] < '#'))
		return false;
	return true;
}

bool isValidIdentifier(string str)
{
	if (isKeyword(str))
		return false;
	if (!(str[0] >= 'a' && str[0] <= 'z'))
		return false;

	if (str.size() == 1)
		return true;
	for (int i = 1; i < str.size(); i++) {
		if (!(str[i] >= 'a' && str[i] <= 'z') &&
			!isDigit(str[i]))
			return false;
	}
	return true;
}

void Scanner::createSymbolTable(string filePath)
{
	string currentLine;
	ifstream file;
	file.open(filePath);
	if (!file.is_open()) {
		cout << "Opening the file was unsuccesfull";
		exit(0);
	}

	vector<string> symbolList;
	int currentLineNumber = 0;
	while (getline(file, currentLine)) {
		currentLineNumber++;
		istringstream iss(currentLine);
		string currentWord;
		while (iss >> currentWord) {
			bool alreadyOccurred = false;
			for (int i = 0; i < symbolList.size(); i++) {
				if (currentWord == symbolList[i])
					alreadyOccurred = true;
			}
			if (alreadyOccurred == false) {
				if (isKeyword(currentWord) || isOperator(currentWord) || isSeparator(currentWord))
					continue;
				else if (isValidIdentifier(currentWord) || isCharOrStringConstant(currentWord) || isNumber(currentWord))
					symbolList.push_back(currentWord);
				else {
					string errorOutput = "Error: undefined operator on line " + to_string(currentLineNumber);
					errorsList.push_back(errorOutput);
				}
			}
		}
	}

	for (int i = 0; i < symbolList.size(); i++) {
		symbolTable.insert(pair<string, int>(symbolList[i], i));
	}
}

void Scanner::createProgramInternalForm(string filePath)
{
	createSymbolTable(filePath);

	string currentLine;
	ifstream file;
	file.open(filePath);
	if (!file.is_open()) {
		cout << "Opening the file was unsuccesfull";
		exit(0);
	}

	vector<string> valuesList;
	while (getline(file, currentLine)) {
		istringstream iss(currentLine);
		string currentWord;
		while (iss >> currentWord) {
			int tokenPos = -1, symbolPos = -1;
			bool alreadyOccurred = false;
			for (int i = 0; i < valuesList.size(); i++) {
				if (currentWord == valuesList[i])
					alreadyOccurred = true;
			}
			if (alreadyOccurred == false) {
				if (isValidIdentifier(currentWord)) {
					tokenPos = 0;
					if (symbolTable.find(currentWord) != symbolTable.end()) {
						symbolPos = symbolTable.find(currentWord)->second;
					}
				}
				else if (isNumber(currentWord) || isCharOrStringConstant(currentWord)) {
					tokenPos = 1;
					if (symbolTable.find(currentWord) != symbolTable.end()) {
						symbolPos = symbolTable.find(currentWord)->second;
					}
				}
				else if (isKeyword(currentWord) || isOperator(currentWord) || isSeparator(currentWord)) {
					if (tokenTable.find(currentWord) != tokenTable.end()) {
						tokenPos = tokenTable.find(currentWord)->second;
					}
				}
				PIFElement newElement;
				newElement.currentData = currentWord;
				newElement.tokenTablePos = tokenPos;
				newElement.symbolTablePos = symbolPos;
				valuesList.push_back(currentWord);
				pifTable.push_back(newElement);
			}
		}
	}
}

void Scanner::scanFile(string filePath)
{
	createProgramInternalForm(filePath);
	getPIFTable();
	getSymbolTable();
	getTokenTable();
	getErrors();
}

void sorter(map<string, int>& m)
{
	vector<pair<string, int>> values;
	for (auto& it : m) {
		values.push_back(it);
	}

	sort(values.begin(), values.end(), [=](pair<string, int>& a, pair<string, int>& b)
		{
			return a.second < b.second;
		}
	);
	for (auto& it : values) {
		cout << it.first << " -> " << it.second << "\n";
	}
}

void Scanner::getTokenTable()
{
	cout << "\nToken table: \n";
	sorter(tokenTable);
}

void Scanner::getSymbolTable()
{
	cout << "\nSymbol table: \n";
	sorter(symbolTable);
}


void Scanner::getPIFTable()
{
	cout << "\nProgram Internal Form: \n";
	cout << "* value -> TokenTable index, SymbolTable index *\n";
	for (int i = 0; i < pifTable.size(); i++) {
		PIFElement currentElement = pifTable[i];
		cout << currentElement.currentData << " -> " << currentElement.tokenTablePos << " -> " << currentElement.symbolTablePos << "\n";
	}
}

void Scanner::getErrors()
{
	cout << "\n";
	for (int i = 0; i < errorsList.size(); i++) {
		cout << errorsList[i] << "\n";
	}
}
