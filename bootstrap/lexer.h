/*
 * This is the handwritten lexer being used to bootsrap the metacompiler.
 * It can parse simple regexs in order to generate the DFAs requied for more
 * complex lexical analysis without the need to handcode them.
 */

#pragma once
#include <stddef.h>
#include <stdio.h>

struct token {
	size_t id;
	size_t meta;
};

typedef size_t RULE;
typedef size_t LEXER;

/*
 * Initializes the lexer
 */
void lexer_init();

/*
 * Frees all memory held by the lexer.
 */
void lexer_free();

/*
 * Makes a rule from a regex.
 */
RULE lexer_makeRule(char * rule, int* err);

/*
 * Binds a set of rules to define a lexer.
 */
LEXER lexer_bindRuleset(RULE* rules, size_t arrSize);

/*
 * Binds a FILE object to the context that will be used as an input stream.
 * The lexer does not handle opening or closing the file object, this must be handled by the calling code
 */
void lexer_bindStream(FILE* file);

/*
 * Returns the next token in the input stream, or EOF if no more exist.
 */
struct token lexer_next(LEXER lex);
