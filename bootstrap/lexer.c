#include "lexer.h"
#include "../clib/src/bitset.h"
#include "../clib/src/varArr.h"
#include "../clib/src/range.h"
#include "../clib/src/hashmap.h"

//Memory of the lexer
VarArr rules;// List of all rules generated.
VarArr lexers;// List of all lexers defined.
Range LexerRange;// Range based allocator used for objects that will last the lifetime of the lexer.

//Global variables of the lexer
char inputBuff[2048];// Buffers characters from the input file
int EOFFlag;// Marks if the EOF has been reached for the current file
size_t buffReadIndex;// Stores the index of the char buffer being read from
size_t charsLeft;// stores the number of charaters left in the char buffer.
FILE* file;// File currently being parsed by the lexer.
LEXER current_lexer;// The ID of the lexer being used for tokenization of the given file.

int nextByte() {
	if (EOFFlag && charsLeft == 0)
		return -1;
	
	if (buffReadIndex == 2048 && !EOFFlag) {
		size_t numRead = fread(inputBuff, 1, 2048, file);
		EOFFlag = feof(file);
		buffReadIndex = 0;
		charsLeft = numRead;
	}
	
	if (charsLeft == 0)
		return -2;
	
	int out = inputBuff[buffReadIndex];
	buffReadIndex++;
	charsLeft--;
	return out;
}

void lexer_init()
{
	rules = varArr_create();
	lexers = varArr_create();
	LexerRange = range_create(8192);
	
	file = NULL;
	current_lexer = 0;
}

void lexer_free()
{
	varArr_destroy(&rules);
	varArr_destroy(&lexers);
	range_destroy(&LexerRange);
	file = NULL;
}

void lexer_bindStream(FILE* stream)
{
	file = stream;
	EOFFlag = 0;
	charsLeft = 0;
	buffReadIndex = 0;
}

RULE lexer_makeRule(char * rule, int* status) {
	
}
