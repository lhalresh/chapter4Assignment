#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <string>

using namespace std;

#include "toyTokenizer.cpp"

struct Parser {
    Token* tokens;
    int count;
    int pos;
};

bool prCheck(const Parser &p, TokenType t) {
    if (p.pos >= p.count) {
        return false;
    }

    return p.tokens[p.pos].tokenType == t;
}

void prAdvance(Parser &p) {
    if (p.pos < p.count) {
        p.pos++;
    }
}


bool parseA(Parser &p, string &errorMessage) {

    if (!prCheck(p, TOK_A)) {
        errorMessage = "expected 'a'";
        return false;
    }

    prAdvance(p);

    while (prCheck(p, TOK_A)) {
        prAdvance(p);
    }

    return true;
}


bool parseB(Parser &p, string &errorMessage) {

    if (!prCheck(p, TOK_B)) {
        errorMessage = "expected 'b'";
        return false;
    }

    prAdvance(p);

    while (prCheck(p, TOK_B)) {
        prAdvance(p);
    }

    return true;
}


bool parseC(Parser &p, string &errorMessage) {

    if (!prCheck(p, TOK_C)) {
        errorMessage = "expected 'c'";
        return false;
    }

    prAdvance(p);

    return true;
}


bool parseS(Parser &p, string &errorMessage) {

    if (!parseA(p, errorMessage)) {
        return false;
    }

    // If there is a c, this must be A C B.
    if (prCheck(p, TOK_C)) {

        if (!parseC(p, errorMessage)) {
            return false;
        }

        if (!parseB(p, errorMessage)) {
            return false;
        }
    }

    return true;
}


bool isNewline(const Token &t) {
    return t.tokenType == TOK_UNKNOWN &&
           (t.lexeme == "\n" || t.lexeme == "\r");
}


int main(int argc, char** argv) {

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <source-file>" << endl;
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");

    if (!fp) {
        cerr << "Cannot open file: " << argv[1] << endl;
        return 1;
    }

   
    Tokenizer tk;
    tkInit(&tk, fp);

    Token tokens[MAX_TOKENS];

    int tokenCount = tokenize(&tk, tokens, MAX_TOKENS);

    fclose(fp);

   
    cout << "Tokens:" << endl;

    for (int i = 0; i < tokenCount; i++) {

        const Token* t = &tokens[i];

        cout << " Token(type="
             << tokenTypeName(t->tokenType)
             << ", lexeme='"
             << t->lexeme
             << "', "
             << t->line
             << ":"
             << t->column
             << ")" << endl;
    }

  
    int i = 0;

    while (i < tokenCount &&
           tokens[i].tokenType != TOK_EOF) {

        // Skip newline tokens
        while (i < tokenCount && isNewline(tokens[i])) {
            i++;
        }

        if (i >= tokenCount ||
            tokens[i].tokenType == TOK_EOF) {
            break;
        }

        int start = i;
        int line = tokens[i].line;

        // Find the end of this line
        int end = i;

        while (end < tokenCount &&
               tokens[end].tokenType != TOK_EOF &&
               !isNewline(tokens[end])) {
            end++;
        }

        Parser parser;
        parser.tokens = tokens;
        parser.count = end;
        parser.pos = start;

        string errorMessage;

        bool valid = parseS(parser, errorMessage);

        cout << endl;
        cout << "Parsing line " << line << ":" << endl;

        // Everything on the line was successfully consumed.
        if (valid && parser.pos == end) {

            cout << "No errors found" << endl;

        }
        else {

            const Token* badToken = &tokens[parser.pos];

            if (valid) {
                errorMessage = "unexpected token after valid input";
            }

            cout << "Parser error at line "
                 << badToken->line
                 << " column "
                 << badToken->column
                 << ": "
                 << errorMessage
                 << " (got "
                 << tokenTypeName(badToken->tokenType)
                 << ", lexeme='"
                 << badToken->lexeme
                 << "')"
                 << endl;
        }

        // Move to the next line.
        i = end;

        while (i < tokenCount && isNewline(tokens[i])) {
            i++;
        }
    }

    return 0;
}