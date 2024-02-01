#include "string_processing.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

std::vector<std::string> SplitIntoWords(const std::string text) {
	vector<string> words;
	string word;
	for (const char c : text) {
		if (c == ' ') {
			if (!word.empty()) {
				words.push_back(word);
				word.clear();
			}
		}
		else {
			word += c;
		}
	}
	if (!word.empty()) {
		words.push_back(word);
	}

	return words;
}
