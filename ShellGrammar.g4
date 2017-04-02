grammar ShellGrammar;

shell: command;

//
// RULES
//

command : command ';' command #SequenceCommand
        | command '&&' command # AndCommand
        | command '&' #BGCommand
        | command '||' command # OrCommand
        | command '>' filePath # STDOUTToFile
        | command '2>' filePath # STDERRToFile
        | command '>>' filePath # STDOUTAppendToFile
        | command '<' filePath # FileToSTDIN
        | command '|' command #Pipe
        | 'cd' filePath? #CDCommand
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
ESCAPED_STRING: ('\\ ' | '\\;' | ~[ ;] )+;

WS: ' '+ -> skip;
NL: ('\r' '\n' | '\n' | '\r') -> skip;
