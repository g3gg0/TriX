/************************************************************************
 SeeR a C-scripting library v 0.95a
 http://przemekp.prv.pl/seer/        dev@przemekp.prv.pl

 File:consts.c
 Desc:constants (but without vcpu opcodes)

 Copyright (c) 1998-99,2002, Przemyslaw Podsiadly
 All rights reserved.
 See seer.h for license information.
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "seer_internal.h"
#include "mem.h"

pmem scExternals = { NULL, 0, 0 };


char *parse_error_concatstring = "Parse error at %s. Hint: concatenated strings like \"ab\" \"cd\" are not supported";
char *parse_error = "Parse error at %s";
char *invalid_operand_to = "Invalid operand%s to %s";
char *illegal_cast = "Illegal cast %s";
char *internal_compiler_error = "Internal compiler error%s (%d)";
char *illegal_type_conversion = "Illegal type conversion %s";

//signed unsigned short long const volatile
NameNInt Standard_Types[] = {
    {"int", 4},
    {"long", 8},                //has to be 1st (not 0th, but after int, used in declare.c)
    {"char", 1},
    {"fixed", 4},
    {"float", 4},
    {"double", 8},
    {"short", 2},
    {"register", 4},
    {"void", 0},
    {NULL, 0}                   //end delimeter
};                              //See expression.c:GetValType(...)-explicitly using& in expressi.c

char *Keywords[] = {
/****C****/
    "if",
    "else",
    "for",
    "while",
    "switch",
    "case",
    "return",
    "do",
    "typedef",
    "struct",
    "union",
    "enum",
    "register",
    "const",
    "sizeof",
    "break",
    "default",
    "unsigned",
    "signed",
    "static",
/****C++**/
    "this",
    "class",
    "public", "private", "protected",
    "operator",
    "new", "delete",
/****SeeR*/
    "import",
    "export",
    "secured",
    "inherit",
    "__vcpu",
    "typeof",
    "script",
    NULL                        //end delimeter
};
char *Special_Symbols[] = {
    "..",
    "\\\\",
    "<<=", ">>=",
    "##",
    "\\'", "\\\"",
    "->",
    "||", "&&",
    "//",
    "/*", "*/",
    "+=", "-=", "/=", "*=", "%=", "^=", "&=", "|=",
    ">>", "<<",
    "==", "!=", "<=", ">=",
    "++", "--",
    "[]",
/*   new C++ operators */
    "::",
/*   SeeR special operators - now not implemented */
//     "=>","<->","<>","<?>",":)","(:)",...
/*   rest */
    "!", "~",
    "{", "}" "[", "]",
    "(", ")",
    "*", "+", "-", "/", "%",
    "&", "|",
    ".",
    "\"", "'",
    ";",
    "?", ":",
    "#",
    "~",
    NULL                        //end delimeter
};
