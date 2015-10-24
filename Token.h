
#ifndef TOKEN_H_
#define TOKEN_H_


enum TokenType {
	TT_NORMAL = 0,
	TT_WILDCARD,  // wildcard character.
	TT_UNION,  // | operation.
	TT_CONCAT,  //
	TT_STAR,  // * operation.
	TT_LEFTPARENTHESIS,  // ( operation.
	TT_RIGHTPARENTHESIS,  // ) operation.
	TT_END,
	TT_ERROR,
};

class Token {
public:
	Token();
	explicit Token(const enum TokenType& type)
		: data_('\0'),
		  type_(type),
		  id_(-1) {
		token_count_++;
	}

	~Token();

	enum TokenType GetType() const {
		return type_;
	}
	void SetType(const enum TokenType& type) {
		type_ = type;
	}

	char GetData() const {
		return data_;
	}
	void SetData(const char& data) {
		data_ = data;
	}

	bool IsOperation() const {
		if (TT_NORMAL == GetType())
			return false;
		return '|' == data_
			|| '*' == data_
			|| '(' == data_
			|| ')' == data_;
	}

	int GetId() const {
		return id_;
	}
	void SetId(int id) {
		id_ = id;
	}

private:
	static int token_count_;  // This is for test.

	char data_;  // The data of Token.
	enum TokenType type_;  // The type of Token.
	int id_;  // The id of Token.
};

#endif // TOKEN_H_
