#include "Node.h"

enum CHILD {
	RIGHT_CHILD,
	LEFT_CHILD,
};

Node::Node()
	: left_(NULL),
	  right_(NULL),
	  token_(NULL) {
}


Node::~Node() {
}

std::string Node::ShowAllNode() {
	data_ = "";
	if (right_ && right_->token_ && TT_END == right_->token_->GetType()) {
		left_->ShowNode(1, RIGHT_CHILD);
		data_ = left_->data_;
	} else {
		ShowNode(1, RIGHT_CHILD);
	}
	return data_;
}

void Node::ShowNode(int deep, int child) {
	if (right_) {
		right_->ShowNode(deep + 1, RIGHT_CHILD);
		data_ += right_->data_;
	}
	ShowPrefix(deep, child);
	switch (token_->GetType()) {
	case TT_END:
	case TT_NORMAL:
	case TT_STAR:
	case TT_UNION:
	case TT_WILDCARD:
		data_ += "'";
		data_ += token_->GetData();
		data_ += "'\r\n";
		break;
	case TT_CONCAT:
		data_ += "'+'\r\n";
		break;
	default:
		break;
	}
	if (left_) {
		left_->ShowNode(deep + 1, LEFT_CHILD);
		data_ += left_->data_;
	}
}

void Node::ShowPrefix(int deep, int child) {
	for (int i = 0; i < deep - 1; ++i)
		data_ += "       ";
	if (RIGHT_CHILD == child)
		data_ += "---";
	else
		data_ += "___";
}

void Node::CalculateAllFunction() {
	CalculateFunction(this);
}

void Node::CalculateFunction(Node* node) {
	if (!node)
		return;

	CalculateFunction(node->left_);
	CalculateFunction(node->right_);

	switch (node->token_->GetType()) {
	case TT_END:
	case TT_NORMAL:
	case TT_WILDCARD:
		// A wilcard node n with position i:
		// nullable is false,
		// firstpos(n) = {i},
		// lastpos(n)  = {i}.
		node->nullable_ = false;
		node->first_pos_.push_back(node);
		node->last_pos_.push_back(node);
		break;
	case TT_STAR:
		// A star node n = c1*:
		// nullable is true,
		// firstpos(n) = firstpos(c1),
		// lastpos(n)  = lastpos(c1).
		node->nullable_ = true;
		AppendVector(node->first_pos_, node->left_->first_pos_);
		AppendVector(node->last_pos_, node->left_->last_pos_);
		CalcFollowPos(node);
		break;
	case TT_UNION:
		// An or-node n = c1 | c2:
		// nullable(n) = nullable(c1) or nullable(c2),
		// firstpos(n) = firstpos(c1) U firstpos(c2),
		// lastpos(c)  = lastpos(c1) U lastpos(c2).
		node->nullable_ = node->left_->nullable_ || node->right_->nullable_;
		AppendVector(node->first_pos_, node->left_->first_pos_);
		AppendVector(node->first_pos_, node->right_->first_pos_);
		AppendVector(node->last_pos_, node->left_->last_pos_);
		AppendVector(node->last_pos_, node->right_->last_pos_);
		break;
	case TT_CONCAT:
		// An cat-node n = c1c2:
		// nullable(n) = nullable(c1) and nullable(c2),
		// firstpos(n) = if (nullable(c1)) firstpos(c1) U firstpos(c2) else firstpos(c1),
		// lastpos(c)  = if (nullable(c2)) lastpos(c1) U lastpos(c2) else lastpos(c2).
		node->nullable_ = node->left_->nullable_ && node->right_->nullable_;
		// firstpos(n)
		AppendVector(node->first_pos_, node->left_->first_pos_);
		if (node->left_->nullable_)
			AppendVector(node->first_pos_, node->right_->first_pos_);

		// lastpos(n)
		if (node->right_->nullable_)
			AppendVector(node->last_pos_, node->left_->last_pos_);
		AppendVector(node->last_pos_, node->right_->last_pos_);

		CalcFollowPos(node);
		break;
	default:
		break;;
	}
}

void Node::AppendVector(std::vector<Node*>& dest, std::vector<Node*>& scr) {
	for (std::vector<Node*>::iterator it = scr.begin(); it != scr.end(); it++)
		dest.push_back(*it);
}

void Node::CalcFollowPos(Node* node) {
	switch (node->token_->GetType()) {
	case TT_STAR:
		// A star-node n, and i is a position in lastpos(n), then all position in firstpos(n) are in followpos(i).
		for (std::vector<Node*>::iterator it = node->last_pos_.begin();
				it != node->last_pos_.end();
				it++)
			AppendVector((*it)->follow_pos_, node->first_pos_);
		break;
	case TT_CONCAT:
		// A cat-node n with left child c1 and right child c2, then for every position i in lastpos(c1), all position
		// in firstpos(c2) are in followpos(i).
		for (std::vector<Node*>::iterator it = node->left_->last_pos_.begin();
				it != node->left_->last_pos_.end();
				it++)
			AppendVector((*it)->follow_pos_, node->right_->first_pos_);
		break;
	default:
		break;
	}
}
