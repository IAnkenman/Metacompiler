#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nfa.h"

typedef struct {
	IntPair* edges;//First value in the pair is the character, the next is the state to go to.
	size_t numEdges;
	size_t maxNumEdges;
} NFAState;

static NFAState newState() {
	NFAState out;
	out.edges = malloc(sizeof(IntPair) * 8);
	out.numEdges = 0;
	out.maxNumEdges = 8;
	return out;
}

NFA NFA_new()
{
	NFA out;
	out.numStates = 0;
	out.maxStates = 8;
	out.stateArr = malloc(8 * sizeof(NFAState));
	return out;
}

void NFA_destroy(NFA* nfa)
{
	for (size_t stateIdx = 0; stateIdx < nfa->numStates; stateIdx++) {
		NFAState* state = nfa->stateArr + stateIdx;
		free(state->edges);
	}
	free(nfa->stateArr);
	nfa->maxStates = 0;
	nfa ->numStates = 0;
	nfa->stateArr = NULL;
}


int NFA_newState(NFA* nfa) {
	int id = nfa->numStates;
	//Resize array if it is too small
	if (nfa->numStates == nfa->maxStates) {
		size_t newSize = nfa->maxStates * 2;
		void* newArr = realloc(nfa->stateArr, newSize * sizeof(NFAState));
		if (!newArr) {
			printf("Failed to reallocate NFA state arr.\n");
			return 0;
		}
		nfa->stateArr = newArr;
		nfa->maxStates = newSize;
	}
	
	((NFAState*)nfa->stateArr)[nfa->numStates] = newState();
	nfa->numStates++;
	return id;
}

void NFA_addEdge(NFA* nfa, int begID, int endID, int c) {
	NFAState* states = nfa->stateArr;
	NFAState* state = states+begID;
	IntPair pair;
	pair.a = c;
	pair.b = endID;
	if (state->numEdges == state->maxNumEdges) {//Resize array
		size_t newSize = state->maxNumEdges * 2;
		void* newArr = realloc(state->edges, newSize * sizeof(IntPair));
		if (!newArr) {
			printf("Could not resize NFA Edge array\n");
			return;
		} else {
			state->edges = newArr;
			state->maxNumEdges = newSize;
		}
	}
	state->edges[state->numEdges] = pair;
	state->numEdges++;
}

IntSet NFA_follow(NFA* nfa, int stateID, int c) {
	NFAState* states = nfa->stateArr;
	NFAState* state = &states[stateID];
	IntPair* edges = state->edges;
	IntSet out = IntSet_new();
	for (size_t i = 0; i < state->numEdges; i++) {
		if (c == edges[i].a) {
			IntSet_add(&out, edges[i].b);
		}
	}
	return out;
}

void printNFA(NFA* nfa) {
	printf("NFA has %lu states:\n", nfa->numStates);
	NFAState* nfaStateArr = nfa->stateArr;
	for (size_t stateIdx = 0; stateIdx < nfa->numStates; stateIdx++) {
		NFAState* state = nfaStateArr+stateIdx;
		printf("State %lu:", stateIdx);
		for (size_t edgeIdx = 0; edgeIdx < state->numEdges; edgeIdx++) {
			IntPair* edge = &state->edges[edgeIdx];
			printf(" {%c -> %i}", edge->a, edge->b);
		}
		printf("\n");
	}
}
