
#ifndef TREE_CONSTRUCTER_H_
#define TREE_CONSTRUCTER_H_

#include <stack>

#include "Lex.h"
#include "Node.h"
#include "Token.h"

class TreeConstructer {
public:
	TreeConstructer();
	~TreeConstructer();

	bool ConstructSyntaxTree(Node** root);
	bool SetPattern(const std::string& pattern);
	void ReleaseNode(Node*& node);

private:
	void CleanStack();
	bool IsNeedAddConcatToken(Token* token);
	void Evaluate(Token* token);
	void EvaluateOneDestOperation(Token* token);
	void EvaluateTwoDestOperation();

	std::stack<Token*> operation_;
	std::stack<Node*> value_;
	enum TokenType last_token_type_;
	Lex lex_;
	int node_count_;
	int token_count_;
	int id_;
};

#endif // TREE_CONSTRUCTER_H_
