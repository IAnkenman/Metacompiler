#include <stdio.h>
#include "regex.h"

const char* testRegexs[5] = {
	"i",
	"int",
	"float",
	"\\n-|\\E[0-9]",
	"[A-Za-z_][A-Za-z0-9_]*",
};


int main () {
	Range mem = range_create(4096);
	DFA* dfa = create_dfa(testRegexs[3], &mem);
	print_dfa(dfa);
}
