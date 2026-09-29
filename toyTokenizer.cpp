#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
using namespace std;

#define MAX_TOKENS 8192

// Tokens names: dictionary of all legal symbols/words in our mini-language
enum TokenType { 
    TOK_A,
    TOK_B,
    TOK_C,
    TOK_EOF,
    TOK_UNKNOWN
};

// declare a token structure
struct Token {
    TokenType tokenType;
    string lexeme;
    int line;
    int column;
};


/******************************************************/
/* Helping function to display the token as a string */
const char* tokenTypeName(TokenType t) {
    switch (t) {
        case TOK_A: return "A";
        case TOK_B: return "B";
        case TOK_C: return "C";
        case TOK_EOF: return "ENDFILE";
        default: return "Unknown";
    }
}

// Tokenizer structure
struct Tokenizer {
    FILE *infp;
    int line;
    int column;
    int cur;
    int hasCur;
};

// --- Basic char reading with position tracking ---
void tkInit(Tokenizer *tk, FILE * infp) {
    tk->infp = infp;
    tk->line = 1;
    tk->column = 1;
    tk->cur = 0;
    tk->hasCur = 0;
}

int getChar(Tokenizer *tk) {
    return fgetc(tk->infp);
}

int peekChar(Tokenizer *tk) {
    if (!tk->hasCur) {
        tk->cur = getChar(tk);
        tk->hasCur = 1; // peeked
    }
    return tk->cur;
}

void consumeChar(Tokenizer *tk) {
    if (!tk->hasCur) { // char was peeked but not consumed
        tk->cur = getChar(tk);
        tk->hasCur = 1; // peeked
    }
    
    if (tk->cur == '\n') {
        tk->line++;
        tk->column = 1;
    }
    else if (tk->cur != EOF) {
        tk->column++;
    }
    
    tk->hasCur = 0; // so next peekChar will read fresh new char
}


/*******************************************************************
LookupKeyword - a simple lookup code for keywords in the language: */
TokenType lookupKeywords (int c) {
    switch (c) {
        case 'a': return TOK_A;
        case 'b': return TOK_B;
        case 'c': return TOK_C;
        default: return TOK_UNKNOWN;
    }
}

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic
expressions */
int tokenize(Tokenizer *tk, Token *tokens, int maxTokens) {
    int count = 0;

    while (true) {
        if (count >= maxTokens) {
            cerr << "Tokenizer error: too many tokens" << endl;
            exit(1);
        }
        
        int c = peekChar(tk);
        int ln = tk->line;
        int col = tk->column;

        if (c == EOF) {
            tokens[count].tokenType = TOK_EOF;
            tokens[count].lexeme = "";
            tokens[count].line = ln;
            tokens[count].column = col;
            return count + 1;
        }

        // Single symbol: operator or puctuation
        tokens[count].tokenType = lookupKeywords(c);
        tokens[count].lexeme = string(1, char(c));
        tokens[count].line = ln;
        tokens[count].column = col;

        consumeChar(tk);    
        count++;
    }
}