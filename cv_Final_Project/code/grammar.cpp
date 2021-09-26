#include "Grammar.h"

Grammar::Grammar() {
	//w = "F[@/L][*@L][+%L]";
	w = "F[*+L][-/!L][/@L][/*!L]";
	level = 6;
	//F是树干
	//+ - 绕x轴转动dx -dx
	//* / 绕y轴转动dy -dy
	//! @ 绕z轴转动dz -dz
	generations.push_back("F[+@L][/*!L][!-L]");
	generations.push_back("F[*@L][*-L][@/-L]");
	generations.push_back("F[/-L][+@L][-!L]");
	generations.push_back("F[*+L][-*L][/!L]");
}

string Grammar::getResult() {
	Iteration();
	return result;
}

Grammar::~Grammar() {
	generations.clear();
}

void Grammar::Iteration() {
	srand(time(NULL));
	result = w;
	for (int i = 0; i < level; i++) {
		string tmp = "";
		for (int j = 0; j < result.size(); j++) {
			tmp += search(result[j]);
		}
		result = tmp;
	}
}

string Grammar::search(char c) {
	if (c == 'L') {
		//替换
		int index = rand() % generations.size();
		return generations[index];
	}
	else {
		//不替换
		string tmp(1, c);
		return tmp;
	}
}
