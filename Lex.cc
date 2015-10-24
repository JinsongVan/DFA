
#include "Lex.h"

static const char g_special_data[] = {
	'.',
	'|',
	'*',
	'(',
	')',
	'\\',
};


Lex::Lex()
	: current_id_(0) {
}


Lex::~Lex() {
}

bool Lex::IsSpecialData(const char& data) {
	for (int i = 0; i < sizeof(g_special_data) / sizeof(char); i++)
		if (data == g_special_data[i])
			return true;
	return false;
}

char Lex::GetNextData() {
	return pattern_[current_id_++];
}

bool Lex::SetPattern(const std::string& pattern) {
	current_id_ = 0;
	pattern_ = pattern;
	return true;
}

Token* Lex::GetToken() {
	Token* token = new Token(TT_NORMAL);
	if (current_id_ >= pattern_.size()) {
		token->SetType(TT_END);
		token->SetData((char)(0x08));
		return token;
	}

	char data = GetNextData();
	if (IsSpecialData(data)) {
		switch (data) {
		case '.':
			token->SetType(TT_WILDCARD);
			break;
		case '*':
			token->SetType(TT_STAR);
			break;
		case '|':
			token->SetType(TT_UNION);
			break;
		case '(':
			token->SetType(TT_LEFTPARENTHESIS);
			break;
		case ')':
			token->SetType(TT_RIGHTPARENTHESIS);
			break;
		case '\\':
			if (current_id_ >= pattern_.size())
				token->SetType(TT_END);
			else if (!IsSpecialData(data = GetNextData()))
				token->SetType(TT_ERROR);
			break;
		default:
			break;
		}
	}
	token->SetData(data);
	return token;
}
