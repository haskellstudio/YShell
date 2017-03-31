grammar ShellGrammar;

shell:  command;

command : 'go' PATH #GoCommand
        | 'here' #HereCommand
        | 'run' PATH #RunCommand
        ;


PATH: (~[^<>() ])+;

WS: ' '+ -> skip;
NL: ('\r' '\n' | '\n' | '\r') -> skip;