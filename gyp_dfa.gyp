{
  'targets': [
    {
      'target_name': 'dfa',
      'type': 'executable',
      'sources': [
        'dfa_unittest.cc',
        'DFA.cc',
        'DFA.h',
        'Lex.cc',
        'Lex.h',
        'Node.cc',
        'Node.h',
        'Token.cc',
        'Token.h',
        'TreeConstructer.cc',
        'TreeConstructer.h',
      ],
    },
  ],
}
