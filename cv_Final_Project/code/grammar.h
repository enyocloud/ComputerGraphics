// Grammar.h �ķ���������ͷ�ļ�
#ifndef  GRAMMAR_H
#define GRAMMAR_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;
class Grammar {
public:
	string w;
	Grammar();
	~Grammar();
	void Iteration();
	string getResult();
private:

	string result;
	vector<string> generations;
	string search(char c);
	int level;
};


#endif // ! GRAMMAR_H
