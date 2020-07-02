#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

struct token {
	size_t id;
	size_t meta;
};

typedef size_t DFA;

//The regular expressions that define these tokens are in regex.h
enum token_t : unsigned long long {
	lexer_header = 0,
	parser_header,
	postprocess_header,
	transforms_heaer,
	common_header,
	id,
	whitespace,
	user_comment,
	newline,
	lexer_token,
	regex_symbol,
	open_bracket,
	close_bracket,
	inverted_open_bracket,
	open_curl,
	close_curl,
	open_brace,
	close_brace,
	star,
	plus,
	question,
	bar,
	empty_string,
	dash,
	lit_uint,
	comma,
	map_symbol,
	builtin_parameter,
	equals,
	enum_tag,
	empty_sym,
	builtin_macro,
	using_header,
	default_builtin,
	__eof = 0x8000000000000000,
	__symbol_error, //Given when a token is not recognised.
};

struct lexer_context {
	FILE * input;
};

struct lexer_context lexer_new();
void lexer_destroy(struct lexer_context *);

//TODO Remember to discard all tokens labled 'discard' when switching lexers
//Parser driven lexical analysis
