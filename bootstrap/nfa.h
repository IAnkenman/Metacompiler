#pragma once
#include "IntSet.h"

typedef struct {
	int a;
	int b;
} IntPair;

/*
 * Struct to store and maintain NFA graphs
 */
typedef struct {
	void* stateArr;
	size_t numStates;
	size_t maxStates;
} NFA;

/*
 * Adds an edge to the graph.
 */
void NFA_addEdge(NFA* nfa, int begID, int endID, int c);

/*
 * Creates a new graph.
 */
NFA NFA_new();

/*
 * Frees all memory held by a graph
 */
void NFA_destroy(NFA* nfa);

/*
 * Creates a new node in the graph
 */
int NFA_newState(NFA* nfa);

/*
 * Follows all edges from a given node that act on a specific character
 */
IntSet NFA_follow(NFA* nfa, int stateID, int c);

/*
 * Prints a representation of an NFA graph.
 */
void printNFA(NFA* nfa);
