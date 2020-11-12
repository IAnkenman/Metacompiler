#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "regex.h"
#include "IntSetMap.h"

//Value of the character that represents an empty transition.
#define EMPTY_CHAR (-1)
typedef enum {
	t_kleene,//0
	t_plus,//0
	t_list,//1
	t_sym,//1
	t_empty,//1
	t_concat,//2
	t_bar,//3
	t_openParam,//4
	t_closeParam,//5
	t_eof,//6
} tokTy;

const char* tokToText[10] = {
	"Kleene",
	"Plus",
	"List",
	"Sym",
	"Empty",
	"Concat",
	"Bar",
	"OpenParam",
	"CloseParam",
	"EOF"
};

const int precedenceMap[10] = {0, 0, 1, 1, 1, 2, 3, 4, 5, 6};//Some tokens have the same precedence, and so they are mapped onto identical values to reduce the parse table size.

const int parseTable [7][7] =
{
	{1, 1, 1, 1, 0, 1, 1},
	{1, 1, 1, 1, 1, 1, 1},
	{-1, -1, 1, -1, -1, 1, 1},
	{-1, -1, 1, 1, -1, 1, 1},
	{-1, -1, -1, -1, -1, -1, -1},
	{1, 1, 1, 1, 1, 1, 1},
	{-1, -1, -1, -1, -1, -1, 0}
};

//Enum that enumerates all non-terminals
typedef enum {
	n_concat,
	n_kleene,
	n_plus,
	n_bar,
	n_empty,
	n_sym,
	n_list
} NT;

const char* ntToText[7] = {
	"concat",
	"kleene",
	"plus",
	"bar",
	"empty",
	"sym",
	"list"
};

typedef struct {
	tokTy ty;
	void* meta;
} Token;

struct ListNode;
typedef struct ListNode ListNode;

struct ListNode {
	ListNode* next;
	Token tok;
};

//Genric stack that holds size_t values.
typedef struct {
	size_t idx;
	size_t arr[4096];
} Size_tStack;

//Genric stack that holds voidptr values.
typedef struct {
	size_t idx;
	void* arr[4096];
} voidptrStack;

//Adds a node to the linked list and returns the new node.
static ListNode* add (ListNode* tail, Token tok) {
	ListNode* newNode = malloc(sizeof(ListNode));
	newNode->next = NULL;
	newNode->tok = tok;
	if (tail) {
		tail->next = newNode;
	}
	return newNode;
}

static void printLinkedList(ListNode* head) {
	while (head) {
		printf(" --> %s", tokToText[head->tok.ty]);
		if (head->tok.ty == t_sym)
			printf("(%c)", (int)head->tok.meta);
		head = head->next;
	}
	printf(" --> NULL\n");
}

static IntSet* getList () {
	//TODO
	return NULL;
}

static size_t escape () {
	//TODO
	return 0;
}

typedef struct TaggedParseNode TaggedParseNode;

typedef struct {
	TaggedParseNode* left;
	TaggedParseNode* right;
} Concat;

typedef struct {
	TaggedParseNode* op;
} Kleene;

typedef struct {} Empty;

typedef struct {
	size_t c;
} Sym;

typedef struct {
	IntSet* list;
} List;

typedef Kleene Plus;
typedef Concat Bar;

//Union that represents a parse node (i.e Non-Terminal)
typedef union {
	Concat concat;
	Kleene kleene;
	Plus plus;
	Bar bar;
	Empty empty;
	Sym sym;
	List list;
} ParseNode;

//Struct that tags a parse node with its enumerated type info.
struct TaggedParseNode{
	NT tag;
	ParseNode node;
};

static inline TaggedParseNode* newNode() { return malloc(sizeof(TaggedParseNode)); };

static void printParseStack(Size_tStack* stk) {
	for (size_t i = 0; i < stk->idx; i += 2) {
		Token toPrint;
		toPrint.ty = stk->arr[i];
		toPrint.meta = (void*)stk->arr[i+1];
		printf("%s", tokToText[toPrint.ty]);
		if (toPrint.ty == t_sym)
			printf("(%c)", (int)toPrint.meta);
		printf(" | ");
	}
	printf("\n");
}

static void printParseTree(TaggedParseNode* node) {
	switch (node->tag) {
		case n_concat:
			printf("(");
			printParseTree(node->node.concat.left);
			printf(") :: (");
			printParseTree(node->node.concat.right);
			printf(")");
			break;
		case n_kleene:
			printParseTree(node->node.kleene.op);
			printf("*");
			break;
		case n_plus:
			printParseTree(node->node.plus.op);
			printf("+");
			break;
		case n_bar:
			printf("(");
			printParseTree(node->node.concat.left);
			printf(") | (");
			printParseTree(node->node.concat.right);
			printf(")");
			break;
		case n_empty:
			printf("\\E");
			break;
		case n_sym:
			printf("%c", (int)node->node.sym.c);
			break;
		case n_list:
			printf("\\L");
			break;
	}
}

static void printResultStack(voidptrStack* stk) {
	printf("Result stack has %lu items:\n", stk->idx);
	for (size_t i = 0; i < stk->idx; i++) {
		printParseTree(stk->arr[i]);
		printf("\n");
	}
}

static inline void pushTok(Token tok, Size_tStack* stk) {
	stk->arr[stk->idx] = tok.ty;
	stk->idx++;
	stk->arr[stk->idx] = (size_t)tok.meta;
	stk->idx++;
}

static inline void pushResult(TaggedParseNode* p, voidptrStack* stk) {
	stk->arr[stk->idx] = p;
	stk->idx++;
}

static inline Token popTok(Size_tStack* stk) {
	Token out;
	stk->idx--;
	out.meta = (void*)stk->arr[stk->idx];
	stk->idx--;
	out.ty = stk->arr[stk->idx];
	return out;
}

static inline void* popResult(voidptrStack* stk) {
	void* out;
	stk->idx--;
	out = stk->arr[stk->idx];
	return out;
}

static inline Token top(Size_tStack* stk) {
	Token out;
	out.meta = (void*)stk->arr[stk->idx - 1];
	out.ty = stk->arr[stk->idx - 2];
	return out;
}

static inline void reduce(Size_tStack* tstk, voidptrStack* rstk) {
	Token tok = popTok(tstk);
	TaggedParseNode* newNT;
	TaggedParseNode* leftOp;
	TaggedParseNode* rightOp;
	IntSet* list;
	switch (tok.ty) {
		case t_kleene:
			newNT = newNode();
			leftOp = popResult(rstk);
			newNT->tag = n_kleene;
			newNT->node.kleene.op = leftOp;
			pushResult(newNT, rstk);
			break;
		case t_plus:
			newNT = newNode();
			leftOp = popResult(rstk);
			newNT->tag = n_plus;
			newNT->node.kleene.op = leftOp;
			pushResult(newNT, rstk);
			break;
		case t_list:
			newNT = newNode();
			newNT->tag = n_list;
			newNT->node.list.list = tok.meta;
			pushResult(newNT, rstk);
			break;
		case t_sym:
			newNT = newNode();
			newNT->tag = n_sym;
			newNT->node.sym.c = (size_t)tok.meta;
			pushResult(newNT, rstk);
			break;
		case t_empty:
			newNT = newNode();
			newNT->tag = n_empty;
			pushResult(newNT, rstk);
			break;
		case t_concat:
			rightOp = popResult(rstk);
			leftOp = popResult(rstk);
			newNT = newNode();
			newNT->tag = n_concat;
			newNT->node.concat.left = leftOp;
			newNT->node.concat.right = rightOp;
			pushResult(newNT, rstk);
			break;
		case t_bar:
			rightOp = popResult(rstk);
			leftOp = popResult(rstk);
			newNT = newNode();
			newNT->tag = n_bar;
			newNT->node.bar.left = leftOp;
			newNT->node.bar.right = rightOp;
			pushResult(newNT, rstk);
			break;
		case t_closeParam:
			if (top(tstk).ty != t_closeParam)
				printf("Unmatched ')'\n");
			else
				popTok(tstk);
			break;
		case t_openParam:
			printf("Reduce called on '('\n");
			break;
		case t_eof:
			printf("Reduce called on 'EOF'\n");
			break;
	}
}

//Generates an NFA from the parse stack.
static IntPair generate(TaggedParseNode* root, NFA* nfa) {
#if 0
	printf("Generating %s NT\n", ntToText[root->tag]);
#endif
	IntPair out;
	IntPair left;
	IntPair right;
	IntSet* l;
	switch (root->tag) {
		case n_concat:
			left = generate(root->node.concat.left, nfa);
			right = generate(root->node.concat.right, nfa);
			NFA_addEdge(nfa, left.b, right.a, EMPTY_CHAR);
			out.a = left.a;
			out.b = right.b;
			break;
		case n_kleene:
			left = generate(root->node.kleene.op, nfa);
			out.a = NFA_newState(nfa);
			out.b = NFA_newState(nfa);
			NFA_addEdge(nfa, out.a, left.a, EMPTY_CHAR);
			NFA_addEdge(nfa, out.a, out.b, EMPTY_CHAR);
			NFA_addEdge(nfa, left.b, left.a, EMPTY_CHAR);
			NFA_addEdge(nfa, left.b, out.b, EMPTY_CHAR);
			break;
		case n_plus:
			left = generate(root->node.plus.op, nfa);
			out.a = NFA_newState(nfa);
			out.b = NFA_newState(nfa);
			NFA_addEdge(nfa, out.a, left.a, EMPTY_CHAR);
			NFA_addEdge(nfa, left.b, left.a, EMPTY_CHAR);
			NFA_addEdge(nfa, left.b, out.b, EMPTY_CHAR);
			break;
		case n_bar:
			left = generate(root->node.bar.left, nfa);
			right= generate(root->node.bar.right, nfa);
			out.a = NFA_newState(nfa);
			out.b = NFA_newState(nfa);
			NFA_addEdge(nfa, out.a, left.a, EMPTY_CHAR);
			NFA_addEdge(nfa, out.a, right.a, EMPTY_CHAR);
			NFA_addEdge(nfa, left.b, out.b, EMPTY_CHAR);
			NFA_addEdge(nfa, right.b, out.b, EMPTY_CHAR);
			break;
		case n_empty:
			out.a = NFA_newState(nfa);
			out.b = NFA_newState(nfa);
			NFA_addEdge(nfa, out.a, out.b, EMPTY_CHAR);
			break;
		case n_sym:
			out.a = NFA_newState(nfa);
			out.b = NFA_newState(nfa);
			NFA_addEdge(nfa, out.a, out.b, (size_t)root->node.sym.c);
			break;
		case n_list:
			out.a = NFA_newState(nfa);
			out.b = NFA_newState(nfa);
			l = root->node.list.list;
			for (size_t i = 0; i < l->size; i++) {
				NFA_addEdge(nfa, out.a, out.b, l->arr[i]);
			}
			break;
	}
#if 0
	printNFA(nfa);
#endif
	return out;
};

typedef struct {
	//Sorted array of IntSets representing powersets of the original NFA
	IntSet** sets;
	//Maps set array indexes to their DFA ID.
	int* mappings;
	//Maps DFA IDs to NFA powersets.
	IntSet** dfaToSet;
	size_t numSets;
	size_t maxSets;
} PowerSet;

PowerSet PowerSetCreate() {
	PowerSet out;
	out.sets = malloc(8*sizeof(IntSet*));
	out.mappings = malloc(8*sizeof(int));
	out.dfaToSet = malloc(8*sizeof(IntSet*));
	out.maxSets = 8;
	out.numSets = 0;
	return out;
}

/*
* Gets the DFA id of a set, or creates one if it dosen't already exist.
 */
int PowerSetGet(PowerSet* pset, IntSet* nset) {
	//If powerset is empty
	if (pset->numSets == 0) {
		pset->sets[0] = nset;
		pset->mappings[0] = 0;
		pset->dfaToSet[0] = nset;
		pset->numSets++;
		return 0;
	} else {
		//Binary search
		size_t l = 0;
		size_t r = pset->numSets - 1;
		while (l != r) {
			size_t m = (l + r + 1) / 2;
			if (IntSet_compare(nset, pset->sets[m]) < 0)
				r = m - 1;
			else
				l = m;
		}
		size_t idx = l;
		//If a matching set already exists, return its node id
		if (IntSet_compare(nset, pset->sets[idx]) == 0) {
			return pset->mappings[idx];
		}
		//Otherwise add a new set
		else {
			if (pset->numSets == pset->maxSets) {
				size_t newSize = pset->maxSets * 2;
				IntSet** newSetArr = realloc(pset->sets, sizeof(IntSet*) * newSize);
				int* newMapArr = realloc(pset->mappings, sizeof(int) * newSize);
				IntSet** newDfaMapArr = realloc(pset->dfaToSet, sizeof(IntSet*) * newSize);
				if (newSetArr && newMapArr && newDfaMapArr) {
					pset->sets = newSetArr;
					pset->mappings = newMapArr;
					pset->dfaToSet = newDfaMapArr;
					pset->maxSets = newSize;
				}
				else {
					printf("Could not resize a powerset array\n");
					return 0;
				}
			}
			if (IntSet_compare(pset->sets[idx], nset) < 0)
				idx++;
			IntSet** src = pset->sets+idx;
			IntSet** dest = pset->sets+idx+1;
			size_t num = pset->numSets - idx;
			//Move set pointers up
			memmove(dest, src, num * sizeof(IntSet));
			int* msrc = pset->mappings+idx;
			int* mdest = pset->mappings+idx+1;
			//Move ID mappings up
			memmove(mdest, msrc, num * sizeof(int));
			pset->sets[idx] = nset;
			pset->mappings[idx] = pset->numSets;
			pset->dfaToSet[pset->numSets] = nset;
			int out = pset->numSets;
			pset->numSets++;
			return out;
		}
	}
}

//Takes a set of starting IDs from a given nfa, and adds all the e-closures to the set.
void closeOn(IntSet* start, NFA* nfa) {
	IntSet toCheck = IntSet_new();
	for (size_t i = 0; i < start->size; i++) {
		IntSet_add(&toCheck, start->arr[i]);
	}
	while (toCheck.size > 0) {
		toCheck.size--;
		int following = toCheck.arr[toCheck.size];
		//Get a list of all the e-transitions from the node being checked, then iterate through and check if the node has not yet been found
		IntSet eTransitions = NFA_follow(nfa, following, EMPTY_CHAR);
		for (size_t i = 0; i < eTransitions.size; i++) {
			if (!IntSet_has(start, eTransitions.arr[i])) {
				IntSet_add(start, eTransitions.arr[i]);
				IntSet_add(&toCheck, eTransitions.arr[i]);
			}
		}
		IntSet_destroy(&eTransitions);
	}
	IntSet_destroy(&toCheck);
}

NFA constructDFA(NFA* nfa, int startState) {
	//Init
	int dfaStart = 0;
	IntSet toTrack = IntSet_new();
	IntSetMap powersetContainer = IntSetMap_new();
	
	//While DFA nodes need edges tracked
	while (toTrack.size > 0) {
		toTrack.size--;
		int tracking = toTrack.arr[toTrack.size];
		printf("Tracking DFA node %i\n", tracking);
	//Get all NFA nodes that the DFA node represents.
		IntSet* baseSet = &powersetContainer.setStorage[tracking];
		unsigned char checkedChars[32];
		for (size_t i = 0; i < 32; i++)
			checkedChars[i] = 0;
	//Select a character that has an outgoing edge, and add all nodes that it leads to in from any node in the NFA set to a new set.
		for (size_t node = 0; node < baseSet->size; node++) {
			
	//Perform e-closure on the new set.
	
	//Register a new DFA edge as going from the starting dfa node to the DFA node represented by the new set of NFA states.
	
	//If the above process creates a new DFA node, add it to the list to be tracked.
	//Repeat above process for all characters.
		}
	}
}

NFA* RegEx_parseString(const char * str) {
	ListNode* head = NULL;
	ListNode* tail = NULL;
	
	printf("Parsing RegEx\n");
	//If NULL was passed, return
	if (!str) {
		printf("NULL was passed as argument for the RegEx string\n");
		return NULL;
	}
	
	//Tokenize input
	printf("Tokenizing the input string\n");
	size_t c;
	Token tok;
	while ((c = *str++)) {
		tok.meta = (void*)c;
		switch (c) {
			case '*':
				tok.ty = t_kleene;
				break;
			case '+':
				tok.ty = t_plus;
				break;
			case '(':
				tok.ty = t_openParam;
				break;
			case ')':
				tok.ty = t_closeParam;
				break;
			case '[':
				tok.ty = t_list;
				tok.meta = getList();
				break;
			case '|':
				tok.ty = t_bar;
				break;
			case '\\':
				tok.meta = (void*)escape();
				if ((size_t)tok.meta == 'E')
					tok.ty = t_empty;
				break;
			default:
				tok.ty = t_sym;
				break;
		}
		tail = add(tail, tok);
		if (!head)
			head = tail;
	}
	
	//If the string had nothing in it, return.
	if (!head) {
		printf("The string given was empty\n");
		return NULL;
	}
	
	//Add EOF token
	tok.meta = 0;
	tok.ty = t_eof;
	add(tail, tok);
	
	printLinkedList(head);
	
	//Scan the list of tokens and inject concatinations where needed.	
	printf("Adding concatinations\n");
	ListNode* current = head;
	ListNode* prev = NULL;
	char producedResult = 0;
	ListNode* toInsert;
	while (current) { 
		switch (current->tok.ty) {
			case t_kleene:
			case t_plus:
			case t_sym:
			case t_list:
			case t_empty:
				if (producedResult) {
					toInsert = malloc(sizeof(ListNode));
					toInsert->tok.ty = t_concat;
					prev->next = toInsert;
					toInsert->next = current;
				}
				producedResult = 1;
				break;
			case t_closeParam:
				producedResult = 1;
				break;
			case t_bar:
				producedResult = 0;
				break;
			case t_openParam:
				if (producedResult) {
					toInsert = malloc(sizeof(ListNode));
					toInsert->tok.ty = t_concat;
					prev->next = toInsert;
					toInsert->next = current;
				}
				producedResult = 0;
				break;
			case t_concat:
			case t_eof:
				break;
		}
		prev = current;
		current = current->next;
	}
	
	printLinkedList(head);
	
	//Push EOF onto the stack
	Size_tStack tstkMem;
	voidptrStack rstkMem;
	tstkMem.idx = 0;
	rstkMem.idx = 0;
	Size_tStack* tokenStack = &tstkMem;
	voidptrStack* resultStack = &rstkMem;
	pushTok(tok, tokenStack);
	
	
	//After concatinations are added the token list represents an operator grammar, so an operator precedence parser can be used.
	current = head;
	char loop = 1;
	char err = 0;
	while (loop) {
		Token stackTok = top(tokenStack);
		Token lookToken = current->tok;
		int stackTokPrec = precedenceMap[stackTok.ty];
		int lookTokPrec = precedenceMap[lookToken.ty];
		int precedence = parseTable[stackTokPrec][lookTokPrec];
#if 0
		printf("Looking at: %s\n", tokToText[lookToken.ty]);
		printParseStack(tokenStack);
		printResultStack(resultStack);
#endif
		//Zero indicates an error or succsess
		if (precedence == 0) {
			//If the parser exited for any reason other than matching EOF, or if there is more than one result on the result stack, indicate an error.
			if (!(lookToken.ty == t_eof && stackTok.ty == t_eof) || resultStack->idx != 1) {
				err = 1;
			}
			loop = 0;
		}
		if (precedence > 0)
			reduce(tokenStack, resultStack);
		else {
			pushTok(lookToken, tokenStack);
			current = current->next;
		}
		
	}
	
	printf("Parsing done\n");
	if (err) {
		printf("There was a parser error\n");
		return NULL;
	}
	
	//Generate NFA
	NFA nfa = NFA_new();
	IntPair nfaPair = generate(popResult(resultStack), &nfa);
	printNFA(&nfa);
	
	//Convert NFA to DFA
	printf("Converting NFA to DFA\n");
	NFA* dfa = malloc(sizeof(NFA));
	*dfa = constructDFA(&nfa, nfaPair.a);
	
	//Free all memory
	printNFA(dfa);
	return dfa;
}
