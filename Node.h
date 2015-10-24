
#ifndef NODE_H_
#define NODE_H_

#include <string>
#include <vector>

#include "Token.h"

class Node {
public:
	Node();
	~Node();

	std::string ShowAllNode();

	void CalculateAllFunction();
	void CalculateFunction(Node* node);

public:
	Node* left_;
	Node* right_;
	Token* token_;
	std::string data_;

	// Nullable, firstpos, lastpos, followpos.
	bool               nullable_;
	std::vector<Node*> first_pos_;
	std::vector<Node*> last_pos_;
	std::vector<Node*> follow_pos_;

private:
	void ShowNode(int deep, int child);
	void ShowPrefix(int deep, int child);
	static void AppendVector(std::vector<Node*>& dest, std::vector<Node*>& scr);
	static void CalcFollowPos(Node* node);
};

#endif // NODE_H_
