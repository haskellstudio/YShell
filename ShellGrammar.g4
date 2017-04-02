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
        | 'ls' (filePath)? #ListCommand
        | 'exit' #ExitCommand
        | filePath arguments #RunProgram
        ;

arguments: (filePath | string)*?;

string : QUOTED_STRING #QuotedString
       | ESCAPED_STRING #EscapedString
       ;

       //
       // TERMINATORS
       //

filePath : QUOTED_FILEPATH #QuotedFilepath
         | ESCAPED_FILEPATH #EscapedFilepath
         ;

QUOTED_FILEPATH : '"' '~' '"'
                | '"' ('~/' | '/')  ([-.a-zA-Z0-9:\\ ] '/'?)* '"'
                | '"' ([-.a-zA-Z0-9:\\ ] '/'?)+ '"'
                ;

ESCAPED_FILEPATH: '~'
                | ('~/' | '/') (('\\ ' | [-.a-zA-Z0-9:\\] ) '/'? )*
                | (('\\ ' | [-.a-zA-Z0-9:\\] ) '/'? )+
                ;

QUOTED_STRING: '"' ('\\"' | ~["] )*? '"';
ESCAPED_STRING: ('\\ ' | ~[ ] )+;

WS: ' '+ -> skip;
NL: ('\r' '\n' | '\n' | '\r') -> skip;
