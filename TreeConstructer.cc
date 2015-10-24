
#include "TreeConstructer.h"

static const TokenType g_need_add_concat[][2] = {
	{TT_NORMAL, TT_NORMAL},
	{TT_NORMAL, TT_LEFTPARENTHESIS},
	{TT_STAR, TT_NORMAL},
	{TT_STAR, TT_LEFTPARENTHESIS},
	{TT_RIGHTPARENTHESIS, TT_LEFTPARENTHESIS},
	{TT_RIGHTPARENTHESIS, TT_NORMAL},
};

TreeConstructer::TreeConstructer()
	: last_token_type_(TT_END),
	  node_count_(0),
	  token_count_(0) {
}


TreeConstructer::~TreeConstructer() {
	CleanStack();
}

bool TreeConstructer::SetPattern(const std::string& pattern) {
	return lex_.SetPattern(pattern);
}

bool TreeConstructer::ConstructSyntaxTree(Node** root) {
	bool        bRet = false;
	Token* token_concat = NULL;
	Token* token = NULL;
	Node* normal_node = NULL;
	bool        bLoopFlag = true;
	TokenType type;

	last_token_type_ = TT_END;
	id_ = 0;

	CleanStack();

	while (bLoopFlag) {
		token = lex_.GetToken();
		token_count_++;

		// TT_END is not a part of pattern, but we add one TT_END token
		// to indicate the accepting states (like # flag in BOOK)
		if (TT_END == token->GetType())
			bLoopFlag = false;
		else if (TT_ERROR == token->GetType())
			goto Exit0;

		if (TT_END == token->GetType()) {
			// analyze TwoDestOperations in stack
			while (0 != operation_.size())
				EvaluateTwoDestOperation();
		}

		if (IsNeedAddConcatToken(token)) {
			token_concat = new Token(TT_CONCAT);
			token_count_++;
			Evaluate(token_concat);
			token_concat = NULL;
		}

		if (token->IsOperation()) {
			type = token->GetType();
			Evaluate(token);
			last_token_type_ = type;
			token = NULL;
		} else {
			// normal char
			token->SetId(++id_);
			normal_node = new Node();
			node_count_++;
			normal_node->token_ = token;
			last_token_type_ = token->GetType();
			if (TT_WILDCARD == last_token_type_)
				last_token_type_ = TT_NORMAL;

			token = NULL;
			value_.push(normal_node);
			normal_node = NULL;
		}
	}

	// analyze TwoDestOperations in stack
	while (!operation_.empty())
		EvaluateTwoDestOperation();

	if (!operation_.empty() || value_.size() != 1)
		goto Exit0;

	*root = value_.top();
	value_.pop();
	bRet = true;
Exit0:
	if (token_concat) {
		delete token_concat;
		token_count_--;
		token_concat = NULL;
	}
	if (token) {
		delete token;
		token_count_--;
		token = NULL;
	}
	ReleaseNode(normal_node);
	return bRet;
}

void TreeConstructer::ReleaseNode(Node*& node) {
	if (!node)
		return;

	if (node->token_) {
		delete node->token_;
		token_count_--;
		node->token_ = NULL;
	}

	ReleaseNode(node->left_);
	ReleaseNode(node->right_);

	delete node;
	node_count_--;
	node = NULL;
}

void TreeConstructer::CleanStack() {
	while (!value_.empty()) {
		Node* node = value_.top();
		value_.pop();
		ReleaseNode(node);
	}
	while (!operation_.empty()) {
		Token* token = operation_.top();
		operation_.pop();
		delete token;
		token_count_--;
		token = NULL;
	}
}

bool TreeConstructer::IsNeedAddConcatToken(Token* token) {
	if (!token)
		return false;

	if (TT_END == last_token_type_)
		return false;
	if (TT_END == token->GetType())
		return true;

	TokenType type = token->GetType();
	if (TT_WILDCARD == type)
		type = TT_NORMAL;
	for (int i = 0; i < sizeof(g_need_add_concat) / sizeof(TokenType[2]); ++i) {
		if (g_need_add_concat[i][0] == last_token_type_
			&& g_need_add_concat[i][1] == type)
			return true;
	}
	return false;
}

void TreeConstructer::EvaluateOneDestOperation(Token* token) {
	Node* node = new Node();
	node->token_ = token;
	node->left_ = value_.top();
	value_.pop();

	node_count_++;  // For debug.
	// Save value in value_ stack.
	value_.push(node);
}

void TreeConstructer::EvaluateTwoDestOperation() {
	if (value_.size() < 2)
		return;

	Token* token = operation_.top();
	if (TT_UNION != token->GetType() && TT_CONCAT != token->GetType()) {
		token = NULL;
		return;
	}
	operation_.pop();

	Node* node = new Node();
	node->token_ = token;
	token = NULL;
	node_count_++;

	node->right_ = value_.top();
	value_.pop();
	node->left_ = value_.top();
	value_.pop();
	value_.push(node);
}

void TreeConstructer::Evaluate(Token* token) {
	Token* token_left_parenthesis = NULL;

	switch (token->GetType()) {
	case TT_LEFTPARENTHESIS:
		operation_.push(token);
		break;
	case TT_STAR:
		if (!value_.empty() &&
			TT_STAR != value_.top()->token_->GetType())
			EvaluateOneDestOperation(token);
		break;
	case TT_UNION:
	case TT_CONCAT:
		while (!operation_.empty()
				&& TT_LEFTPARENTHESIS != operation_.top()->GetType()
				&& (int)(token->GetType()) <= (int)(operation_.top()->GetType())) {
			// current operation priority less or equal last operation.
			EvaluateTwoDestOperation();
		}
		operation_.push(token);
		break;
	case TT_RIGHTPARENTHESIS:
		// if there are nothing between a pair of parentheses, it is wrong.
		if (TT_LEFTPARENTHESIS == last_token_type_)
			return;

		while (!operation_.empty() &&
				TT_LEFTPARENTHESIS != (operation_.top()->GetType()))
			EvaluateTwoDestOperation();
		// Wrong.
		if (operation_.empty() || TT_LEFTPARENTHESIS != operation_.top()->GetType())
			return;

		token_left_parenthesis = operation_.top();
		delete token_left_parenthesis;
		token_count_--;
		operation_.pop();
		delete token;
		token_count_--;
		break;
	case TT_NORMAL:
	default:
		break;
	}
}
