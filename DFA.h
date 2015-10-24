
#ifndef DFA_H_
#define DFA_H_

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Node.h"
#include "TreeConstructer.h"


struct StateRelation {
	int  state_id_from_;
	char data_;
	int  state_id_to_;
	StateRelation(int id_from, char data, int id_to)
		:state_id_from_(id_from), data_(data), state_id_to_(id_to) {
	}
};

struct DFAState {
	char** data_;
	DFAState** next_;
	DFAState() :data_(NULL), next_(NULL){
	}
	~DFAState(){
		delete[] data_;
		delete[] next_;
	}
};

class DFA {
public:
	DFA();
	~DFA();

	void RegExpToDFA(std::string pattern);
	bool Match(const std::string& pattern);
	void ConstructDFA();
	void CreateSyntaxTree();
	void CreateDFA(Node* node);
	void MinimizeDFA(int count,
		std::list<StateRelation>& state_relations,
		std::set<int>& accepting,
		std::list<std::set<int> >& states);
	void CreateDFATransTable();
	// For testing.
	std::string GetTreeString() {
		return tree_;
	}

private:
	int NextState(int state, char ch);
	void GetNextState(const std::set<Node*>& nodes, std::set<Node*>& next_state);
	bool IsStateInSet(const std::set<Node*>& state, int& state_id_next);
	bool IsContainAcceptingState(std::set<Node*>& nodes);
	void PartitionGroups(std::list<std::set<int> >& states,
		const std::list<StateRelation>& state_relations);
	void PartitionGroup(std::list<std::set<int> >& states,
		std::set<int>& group,
		int cur_group_id,
		const std::list<StateRelation>& state_relations,
		std::map<int, std::set<int> >& map_partition_info);
	bool FindRelationNode(const std::list<StateRelation>& state_relations,
		int current_state_id,
		char data,
		int& arrive_state_id);
	int  FindIdInNewStates(int old_state_id, const std::list<std::set<int> >& new_states);

private:
	Node* syntax_node_;
	TreeConstructer tree_constructer_;
	std::string pattern_;
	std::string tree_;

	std::list<std::set<Node*> > dfa_states_;
	std::list<std::set<int> >   final_dfa_states_;
	std::list<StateRelation>    state_relations_;
	std::set<int>               accepting_;
	std::vector<std::map<char, int> > dfa_trans_;
};


#endif // DFA_H_
