#include "document.h"
#include "read_input_functions.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;

string ReadLine() {
	string s;
	getline(cin, s);
	return s;
}

int ReadLineWithNumber() {
	int result;
	std::cin >> result;
	ReadLine();
	return result;
}