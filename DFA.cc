
#include <assert.h>
#include <iostream>

#include "DFA.h"


DFA::DFA()
    : syntax_node_(NULL) {
}


DFA::~DFA() {
	tree_constructer_.ReleaseNode(syntax_node_);
}

bool DFA::Match(const std::string& pattern) {
	int state = 0;
	int save_state = state;
	for (std::string::const_iterator iter = pattern.begin();
		iter != pattern.end();
		++iter) {
		state = NextState(state, *iter);
		if (state == -1)
			state = 0;
		if (state != -1) {
			save_state = state;
			if (save_state == dfa_trans_.size() - 1)
				return true;
		}
	}
	return false;
}

int DFA::NextState(int state, char ch) {
	if (state >= dfa_trans_.size() - 1 || state == -1)
		return -1;
	const std::map<char, int>& table = dfa_trans_[state];
	if (table.find(ch) == table.end())
		return -1;
	return table.find(ch)->second;
}

void DFA::RegExpToDFA(std::string pattern) {
	if (pattern.empty() || pattern_ == pattern)
		return;

	tree_constructer_.SetPattern(pattern);
	ConstructDFA();
}

void DFA::ConstructDFA() {
	CreateSyntaxTree();
	syntax_node_->CalculateAllFunction();
	CreateDFA(syntax_node_);
	MinimizeDFA(dfa_states_.size(), state_relations_, accepting_, final_dfa_states_);
	assert(final_dfa_states_.size() <= dfa_states_.size());
	CreateDFATransTable();
}

void DFA::CreateSyntaxTree() {
	tree_constructer_.ReleaseNode(syntax_node_);
	tree_constructer_.ConstructSyntaxTree(&syntax_node_);
	tree_ = syntax_node_->ShowAllNode();
}

void DFA::CreateDFA(Node* node) {
	dfa_states_.clear();
	state_relations_.clear();
	accepting_.clear();

	// Prepare start state.
	// #1 Start state is firstpos(n0), n0 is the root of syntax tree. 
	std::set<Node*> nodes;
	for (std::vector<Node*>::iterator it_node = node->first_pos_.begin();
			it_node != node->first_pos_.end();
			it_node++)
		nodes.insert(*it_node);

    // The id of current state.
	int state_id_cur = 0;
	dfa_states_.push_back(nodes);
	if (IsContainAcceptingState(nodes))
		accepting_.insert(state_id_cur);// The accepting_ is an accepting state.
	// End prepare start state.

	// #2 Construct DFA state sets and translation relation.
	int state_id_next = 0; // The id of next state.
	std::map<char, std::set<Node*> > char_map_nodes;
	for (std::list<std::set<Node*> >::iterator iter = dfa_states_.begin();
			iter != dfa_states_.end();
			iter++, state_id_cur++) {
		char_map_nodes.clear();
		nodes.clear();
		nodes = *iter;
		// Collect all translation char and nodes correspond to translation char into char_map_nodes.
		for (std::set<Node*>::iterator iter_set = nodes.begin();
				iter_set != nodes.end();
				iter_set++) {
			char ch = (*iter_set)->token_->GetData();
			char_map_nodes[ch].insert(*iter_set);
		}
		// End.

		// For each translation char, get all followpos of nodes that correspond to translation char.
		for (std::map<char, std::set<Node*> >::iterator iter_map = char_map_nodes.begin();
				iter_map != char_map_nodes.end();
				iter_map++) {
			char ch = iter_map->first;
			std::set<Node*>& temp_nodes = iter_map->second;
			std::set<Node*> next_state;
			// Get the union of followpos(node) for all node in the temp_nodes.
			GetNextState(temp_nodes, next_state);
			if (next_state.empty())
				continue;

			if (!IsStateInSet(next_state, state_id_next)) {
				// If the set is not in list, then add it into list.
				dfa_states_.push_back(next_state);
				if (IsContainAcceptingState(next_state))
					accepting_.insert(state_id_next);  // The state is an accepting state.
			}
			StateRelation relation(state_id_cur, ch, state_id_next);
			state_relations_.push_back(relation);
		}
		// End.
	}
}

void DFA::MinimizeDFA(int count,
		std::list<StateRelation>& state_relations,
		std::set<int>& accepting,
		std::list<std::set<int> >& states) {
	assert(count >= 1);
	assert(accepting.size() != 0);
	assert(state_relations.size() != 0);

	states.clear();
	// Add accepting state.
	states.push_back(accepting);

	// Get unAccepting states.
	std::set<int> unaccepting;
	for (int i = 0; i < count; i++) {
		if (accepting.find(i) == accepting.end())
			unaccepting.insert(i);
	}
	if (unaccepting.size() > 0)
		states.push_back(unaccepting);

	PartitionGroups(states, state_relations);
}

void DFA::CreateDFATransTable() {
	dfa_trans_.clear();
	std::map<char, int> tmp1;
	dfa_trans_.insert(dfa_trans_.begin(), final_dfa_states_.size(), tmp1);
	// -1 is dead state.
	// 0  is start state.
	//
	std::map<int, int> tmp;
	int id = 0;
	int id_temp = 1;
	for (std::list<std::set<int> >::const_iterator iter = final_dfa_states_.begin();
			iter != final_dfa_states_.end();
			iter++, id++) {
		bool accepting = false;
		for (std::set<int>::const_iterator it = iter->begin(); it != iter->end(); it++) {
			if (accepting_.find(*it) != accepting_.end()) {
				tmp[id] = final_dfa_states_.size() - 1;
				accepting = true;
				break;
			}
		}
		if (accepting)
			continue;
		if (iter->find(0) != iter->end()) {
			tmp[id] = 0;
			continue;
		}
		tmp[id] = id_temp++;
	}
	for (std::list<StateRelation>::const_iterator iter = state_relations_.begin();
			iter != state_relations_.end();
			iter++) {
		int id_from = tmp[FindIdInNewStates(iter->state_id_from_, final_dfa_states_)];
		int id_to = tmp[FindIdInNewStates(iter->state_id_to_, final_dfa_states_)];

		dfa_trans_[id_from][iter->data_] = id_to;
	}
}

void DFA::GetNextState(const std::set<Node*>& nodes, std::set<Node*>& next_state) {
	for (std::set<Node*>::iterator iter = nodes.begin();
			iter != nodes.end();
			iter++) {
		std::vector<Node*>& follow_pos = (*iter)->follow_pos_;
		for (std::vector<Node*>::iterator iter_vec = follow_pos.begin();
				iter_vec != follow_pos.end();
				iter_vec++)
			next_state.insert(*iter_vec);
	}
}

bool DFA::IsStateInSet(const std::set<Node*>& state, int& state_id) {
	state_id = 0;
	for (std::list<std::set<Node*> >::iterator iter = dfa_states_.begin();
			iter != dfa_states_.end();
			iter++, state_id++)
		if (*iter == state)
			return true;

	return false;
}

bool DFA::IsContainAcceptingState(std::set<Node*>& nodes) {
	for (std::set<Node*>::iterator iter = nodes.begin();
			iter != nodes.end();
			iter++) {
		if (TT_END == (*iter)->token_->GetType())
			return true;
	}

	return false;
}

void DFA::PartitionGroups(std::list<std::set<int> >& states, const std::list<StateRelation>& state_relations) {
	// Using map to record the node can translate to which group.
	// The key of map is group id.
	// The set<int> contain the node ID that can translate to the group.
	std::map<int, std::set<int> > map_partition_info;
	int cur_group_id = 0;
	for (std::list<std::set<int> >::iterator iter = states.begin(); iter != states.end();) {
		map_partition_info.clear();
		std::set<int>& group = *iter;
		PartitionGroup(states, group, cur_group_id, state_relations, map_partition_info);
		if (map_partition_info.size() > 1) {
			// Means that current group can partition.
			for (std::map<int, std::set<int> >::iterator iter_map = map_partition_info.begin();
					iter_map != map_partition_info.end();
					iter_map++)
				states.push_back(iter_map->second);
			// If a group has partition, the group need delete in the list.
			iter = states.erase(iter);
		} else {
			iter++;
			cur_group_id++;
		}
	}
}

void DFA::PartitionGroup(std::list<std::set<int> >& states,
		std::set<int>& group,
		int cur_group_id,
		const std::list<StateRelation>& state_relations,
		std::map<int, std::set<int> >& map_partition_info) {
	// #1 Collect each node's translation char in the group.
	std::set<char> datas;
	for (std::list<StateRelation>::const_iterator iter = state_relations.begin();
			iter != state_relations.end();
			iter++) {
		if (group.find(iter->state_id_from_) != group.end())
			datas.insert(iter->data_);
	}
	// End.

	for (std::set<char>::iterator iter_data = datas.begin();
			iter_data != datas.end();
			iter_data++) {
		map_partition_info.clear();
		int arrive_state_id = 0;
		for (std::set<int>::iterator iter_state_id = group.begin();
				iter_state_id != group.end();
				iter_state_id++) {
			if (FindRelationNode(state_relations, *iter_state_id, *iter_data, arrive_state_id)) {
				int new_state_id = FindIdInNewStates(arrive_state_id, states);
				if (new_state_id == -1) // Something wrong.
					break;
				map_partition_info[new_state_id].insert(*iter_state_id);
			} else {
				// Indicate map to a dead state, there no translation for this pair of node/char.
				map_partition_info[cur_group_id].insert(*iter_state_id);
			}		
		}
		if (map_partition_info.size() > 1)
			break;// The Group is distinguishable.
	}
}

bool DFA::FindRelationNode(const std::list<StateRelation>& state_relations, int current_state_id, char data, int& arrive_state_id) {
	for (std::list<StateRelation>::const_iterator it = state_relations.begin();
			it != state_relations.end();
			it++) {
		if (it->state_id_from_ == current_state_id && it->data_ == data) {
			arrive_state_id = it->state_id_to_;
			return true;
		}
	}
	return false;
}

int DFA::FindIdInNewStates(int old_state_id, const std::list<std::set<int> >& new_states) {
	int i = 0;
	for (std::list<std::set<int> >::const_iterator iter = new_states.begin();
			iter != new_states.end();
			iter++, i++) {
		for (std::set<int>::const_iterator iter_state_id = (*iter).begin();
				iter_state_id != (*iter).end();
				iter_state_id++) {
			if (old_state_id == *iter_state_id)
				return i;
		}
	}
	return -1;
}
