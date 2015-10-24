// dfa_unittest.cc : The test for DFA project.
// Author:jinsong.van@gmail.com.

#include <iostream>
#include <string>

#include "DFA.h"
#include "Node.h"
#include "TreeConstructer.h"

int main(int argc, char* argv[]) {
    // Print Lex tree.
	TreeConstructer m_TreeConstructer;
	Node* m_pSyntaxNode = NULL;
	m_TreeConstructer.ReleaseNode(m_pSyntaxNode);
	m_TreeConstructer.SetPattern("a(a|b)*c");
	m_TreeConstructer.ConstructSyntaxTree(&m_pSyntaxNode);
	std::cout << m_pSyntaxNode->ShowAllNode();
	m_TreeConstructer.ReleaseNode(m_pSyntaxNode);

    // Test DFA.
    DFA dfa;
	std::string reg("a(a|b)*");
	std::string pat1("aadac");
	std::string pat2("abad");
	std::string pat3("aadadq");
	dfa.RegExpToDFA(reg);
	std::cout << "Regex " << reg << std::endl;
	std::cout << " Match " << (dfa.Match(pat1) ? "successed! " : "failed!    ") << pat1 << std::endl;
	std::cout << " Match " << (dfa.Match(pat2) ? "successed! " : "failed!    ") << pat2 << std::endl;
	std::cout << " Match " << (dfa.Match(pat3) ? "successed! " : "failed!    ") << pat3 << std::endl;

	std::cout << std::endl;
	std::cout << dfa.GetTreeString();

	return 0;
}

