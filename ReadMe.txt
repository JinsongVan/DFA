========================================================================
                       DFA Project Overview
========================================================================

This file contains a summary of what you will find in each of the files that
make up your DFA project.

Author:jinsong.van@gmail.com

Deterministic finite automaton (DFA)—also known as deterministic finite state
machine—is a finite state machine that accepts/rejects finite strings of
symbols and only produces a unique computation (or run) of the automaton for
each input string.

Supported Syntax:
a    Match a wildcard character
*    Match zero or more times
x|y  Match x or y (alternation operator)
()   Grouping and substring capturing

Compile:
$ gyp --depth=. gyp_dfa.gyp
$ make


Example: print Lex tree
	TreeConstructer m_TreeConstructer;
	Node* m_pSyntaxNode = NULL;
	m_TreeConstructer.ReleaseNode(m_pSyntaxNode);
	m_TreeConstructer.SetPattern("a(a|b)*c");
	m_TreeConstructer.ConstructSyntaxTree(&m_pSyntaxNode);
	std::cout << m_pSyntaxNode->ShowAllNode();
	m_TreeConstructer.ReleaseNode(m_pSyntaxNode);

	Output:
	       ---'c'
    ---'+'
                  ---'*'
                                ---'b'
                         ___'|'
                                ___'a'
           ___'+'
                  ___'a'


Example: Match patterns to regex:a(a|b)*
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



/////////////////////////////////////////////////////////////////////////////
