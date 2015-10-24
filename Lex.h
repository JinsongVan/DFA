
#ifndef LEX_H_
#define LEX_H_

#include <string>

#include "Token.h"

class Lex {
public:
	Lex();
	~Lex();

	Token* GetToken();
	bool SetPattern(const std::string& pattern);
	char GetNextData();
	bool IsSpecialData(const char& data);

private:
	std::string pattern_;
	int current_id_;
};

#endif // LEX_H_
