#include <stdio.h>
#include <stdlib.h>
#include "minako.h"

/* each nonterminal symbol has a function here 
 * the functions with suffix p use the given token
 * as the first token. The versions without suffix
 * p use the next token. (They call the p version with
 * yylex() as argument)
 */
void program();
void functionDefinition(int token);
int functionCall();
int statementList();
int block();
int statement(int token);
void ifStatement();
int returnStatement();
int printfStatement();
int type(int token);
int statAssignment();
int assignmentp(int token);
int assignment();
int exprp(int token);
int expr();
int simpExprp(int token);
int simpExpr();
int termp(int token);
int term();
int factorp(int token);
int factor();

#define MAP
void map(int token, int newline);

/** 
 * @brief Prints an error message and exits the program.
 * @param token The token that was actually read
 * @param expected A stirng describing what was expected
 */
void error(int token, char* expected)
{
    fprintf(stderr, "\n\nError in line %d. Expected %s, got ", yylineno, expected);
    map(token, 1);
    fprintf(stderr, "!\n\n");

    if(yyin != stdin)
    {
        fclose(yyin);
    }

    exit(-1);
}

/**
 * Prints the best representation for the given token.
 * If newline is 0 a newline is printed after the representation.
 */
void map(int token, int newline)
{
#ifdef MAP
    char* arr[] = { "AND", "OR", "EQ", "NEQ", "LEQ", "GEQ", "LSS", "GRT",
    "KW_BEGIN", "KW_BOOLEAN", "KW_DO", "KW_ELSE", "KW_END", "KW_FLOAT",
    "KW_FOR", "KW_IF", "KW_INT", "KW_PRINTF", "KW_RETURN", "KW_VOID",
    "KW_WHILE", "CONST_INT", "CONST_FLOAT", "CONST_BOOLEAN", "ID" };

    if(token > 256)
    {
        if(token == CONST_INT)
        {
            fprintf(stderr, "(int)%d ", yylval.intValue);
        }
        else if(token == CONST_FLOAT)
        {
            fprintf(stderr, "(float)%f ", yylval.floatValue);
        }
        else if(token == CONST_BOOLEAN)
        {
            fprintf(stderr, "(bool)%s ", yylval.intValue ? "true" : "false");
        }
        else
        {
            fprintf(stderr, "%s ", arr[token - 257]);
        }
    }
    else
    {
        fprintf(stderr, "%c ", (char)token);
    }

    if(token == ';' || newline == 0 || token == '}' || token == '{')
    {
        fprintf(stderr, "\n");
    }
#endif
}

void program()
{
    int token = yylex();
    // map(token, 1);
    if(token == EOF)
    {
        return;
    }
    else
    {
        functionDefinition(token);
        program();
    }
}

void functionDefinition(int token)
{
    // functionDefinition:
    // type <ID> "(" (<ID>(","<ID>)*)? ")"
    // ("{"|KW_BEGIN)
    // statementList()
    // ("}"|KW_END)

    // type
    token = type(token);

    // <ID>
    if(token != ID)
    {
        error(token, "ID");
    }

    // "("
    token = yylex();
    // map(token, 1);
    if(token != '(')
    {
        error(token, "(");
    }

    // ")"
    token = yylex();
    // map(token, 1);
    if(token == ')')
    {
        token = yylex();
        // map(token, 1);
    }
    else
    {
        // (<ID> (","<ID>)*)?
        while(token != ')')
        {
            if(token != ID)
            {
                error(token, "ID");
            }

            token = yylex();
            // map(token, 1);
            if(token == ')')
            {
                token = yylex();
                // map(token, 1);
                break;
            }
            else if(token != ',')
            {
                error(token, ",");
            }

            token = yylex();
            // map(token, 1);
        }
    }

    // ("{"|KW_BEGIN)
    if(token != KW_BEGIN && token != '{')
    {
        error(token, "{");
    }

    // statementList()
    token = statementList();

    // ("}"|KW_END)
    if(token != KW_END && token != '}')
    {
        error(token, "}");
    }

}

int statementList()
{
    int res = block();
    if(!res)
    {
        return statementList();
    }
    return res;
}

int block()
{
    int token = yylex();
    // map(token, 1);
    switch(token)
    {
        case '{':
        case KW_BEGIN:
        {
            token = statementList();
            // map(token, 1);
            if ((token != '}') && (token != KW_END))
            {
                error(token, "}");
            }
            return 0;
        }
        default:
        {
            return statement(token);
        }
    }
}

int statement(int token)
{
    switch(token)
    {
        case KW_IF:
        {
            ifStatement();
            break;
        }
        case KW_RETURN:
        {
            token = returnStatement();
            if(token != ';')
            {
                error(token, "; 1");
            }
            break;
        }
        case KW_PRINTF:
        {
            token = printfStatement();
            if(token != ';')
            {
                error(token, "; 2");
            }
            break;
        }
        case ID:
        {
            token = yylex();
            // map(token, 1);
            if(token == '=')
            {
                token = statAssignment();
                if(token != ';')
                {
                    error(token, "; 3");
                }
            }
            else if(token == '(')
            {
                token = functionCall();
                if(token != ';')
                {
                    error(token, "; 4");
                }
            }
            break;
        }
        case '}':
        // forgot: KW_END
        case KW_END:
        {
            return '}';
        }
        default:
        {
            error(token, "statement");
        }
    }
    return 0;
}

void ifStatement()
{
    int token = yylex();
    // // map(token, 1);
    if(token != '(')
    {
        error(token, "(");
    }

    token = assignment();

    if(token != ')')
    {
        error(token, ")");
    }

    block();
}

int returnStatement()
{
    int token = yylex();
    // map(token, 1);
    if(token != ';')
    {
        return assignmentp(token);
    }
    return token;
}

int printfStatement()
{
    int token = yylex();
    // map(token, 1);
    if(token != '(')
    {
        error(token, "(");
    }

    token = assignment();
    if(token != ')')
    {
        error(token, "(");
    }
    // forgot: return new token because 
    // we know here that the last token is (
    token = yylex();
    // map(token, 1);
    return token;
}

int statAssignment()
{
    return assignment();
}

int assignmentp(int token)
{
    if(token != ID)
    {
        return exprp(token);
    }
    else
    {
        token = exprp(token);
        if(token == '=')
        {
            return assignment();
        }
        return token;
    }
}

int assignment()
{
    int token = yylex();
    // map(token, 1);
    return assignmentp(token);
}

int exprp(int token)
{
    token = simpExprp(token);
    switch(token)
    {
        case EQ:
        case NEQ:
        case LEQ:
        case GEQ:
        case GRT:
        case LSS:
        {
            return simpExpr();
        }
        default:
        {
            return token;
        }
    }
}

int expr()
{
    int token = yylex();
    // map(token, 1);
    return exprp(token);
}


int simpExprp(int token)
{
    if(token == '=')
    {
        return token;
    }

    if(token == '-')
    {
        token = term();
    }
    else
    {
        token = termp(token);
    }

    while(token == '+' || token == '-' || token == OR)
    {
        token = term();
    }

    return token;
}

int simpExpr()
{
    int token = yylex();
    // map(token, 1);
    return simpExprp(token);
}

int termp(int token)
{
    token = factorp(token);
    while((token == '*') || (token == '/') || (token == AND))
    {
        token = factor();
        // map(token, 1);
    }

    return token;
}

int term()
{
    int token = yylex();
    // map(token, 1);
    return termp(token);
}

int factorp(int token)
{
    switch(token)
    {
        case CONST_INT:
        case CONST_BOOLEAN:
        case CONST_FLOAT:
        {
            token = yylex();
            // map(token, 1);
            return token;
        }
        case ID:
        {
            token = yylex();
            // map(token, 1);
            if (token == '(')
            {
                return functionCall();
            }
            return token;
        }
        case '(':
        {
            token = assignment();
            if(token != ')')
            {
                error(token, ")");
            }
            token = yylex();
            // map(token, 1);
            return token;
        }
        default:
        {
            error(token, "factor");
            return -1;
        }
    }
}

int factor()
{
    int token = yylex();
    // map(token, 1);
    return factorp(token);
}

int functionCall()
{
    int token = yylex();
    // map(token, 1);

    /* if the next token is )
     * then there are no arguments in
     * this functioncall, so we return */
    if(token == ')')
    {
        token = yylex();
        // map(token, 1);
        return token;
    }

    /* else we parse for an assignment */
    token = assignment();

    while(token == ',')
    {
        token = assignment();
    }

    return token;
}

int type(int token)
{
    switch(token)
    {
        case KW_BOOLEAN:
        case KW_FLOAT:
        case KW_INT:
        case KW_VOID:
        {
            int token = yylex();
            // map(token, 1);
            return token;
        }
        default:
        {
            error(token, "type");
            return -1;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        yyin = stdin;
    }
    else
    {
        yyin = fopen(argv[1], "r");
        if (yyin == 0)
        {
            fprintf(stderr,
                    "Fehler: Konnte Datei %s nicht zum lesen oeffnen.\n",
                    argv[1]);
            exit(-1);
        }
    }

    program();

    return 0;
}
