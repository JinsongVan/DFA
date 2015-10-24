#include "Token.h"

int Token::token_count_ = 0;

Token::Token()
	: data_('\0'),
	  type_(TT_NORMAL),
	  id_(-1) {
	token_count_++;
}


Token::~Token() {
	token_count_--;
}
