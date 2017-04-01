grammar ShellGrammar;

shell: command;

//
// RULES
//

command : command '&&' command # MultipleCommands
        | process '>' filePath # STDOUTToFile
        | process '<' filePath # FileToSTDIN
        | process # StandaloneProcess
        ;

process : process '|' process #Pipe
        | 'go' filePath #GoCommand
        | 'here' #HereCommand
        | 'run' filePath arguments #RunCommand
        | 'ls' (filePath)? #ListCommand
        ;

arguments: (filePath | string)*?;

filePath : QUOTED_FILEPATH #QuotedFilepath
         | ESCAPED_FILEPATH #EscapedFilepath
         ;

string : QUOTED_STRING #QuotedString
       | ESCAPED_STRING #EscapedString
       ;

//
// TERMINATORS
//

QUOTED_FILEPATH : '"' '~' '"'
                | '"' '~/' ([-.a-zA-Z0-9:\\ ] '/'?)* '"'
                | '"' '/'  ([-.a-zA-Z0-9:\\ ] '/'?)* '"'
                | '"' ([-.a-zA-Z0-9:\\ ] '/'?)+ '"'
                ;
ESCAPED_FILEPATH: '~'
                | '~/' (('\\ ' | [-.a-zA-Z0-9:\\] ) '/'? )*
                | '/' (('\\ ' | [-.a-zA-Z0-9:\\] ) '/'? )*
                | (('\\ ' | [-.a-zA-Z0-9:\\] ) '/'? )+
                ;

QUOTED_STRING: '"' ('\\"' | ~["] )*? '"';
ESCAPED_STRING: ('\\ ' | ~[ ] )+;

WS: ' '+ -> skip;
NL: ('\r' '\n' | '\n' | '\r') -> skip;
