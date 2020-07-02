#include "lexer.h"

//	Linked list of byte ranges the edge travels on
static struct charsetNode {
	struct charsetNode * next;
	char start;
	char end;
};

//	Linked list of edges for a DFA state.
static struct dfaEdgeNode {
	struct dfaEdgeNode * next;
	struct charsetNode * moveOn;
	unsigned long long goTo;
};

static struct dfaEdgeNode * createDfaNode() {
	return malloc(sizeof(struct dfaEdgeNode));
};

//	Linked list of edges for a NFA state.
static struct nfaEdgeNode {
	struct nfaEdgeNode * next;
	struct charsetNode * moveOn; //Will be null if it represents an empty transition
};

static struct nfaEdgeNode * createNfaNode() {
	return malloc(sizeof(struct nfaEdgeNode));
};

