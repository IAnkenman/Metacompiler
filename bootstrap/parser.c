//The regular expressions that define these tokens are in regex.h
enum token_t : size_t {
	lexer_tok = 0,
	parser_tok,
	postprocess_tok,
	transforms_tok,
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
static const char * rules[34] = {
	"lexer", //lexer_tok
	"parser", //parser_tok
	"postprocess", //postprocess_tok
	"transforms", //transforms_tok
	"common", //common_tok
	"[a-zA-Z_][a-zA-Z0-9_]*", //id
	"[\\0x00-\\0x09\\0x0B-\\0x20\\0x7F]", //whitespace #DISCARD
	"##[^\\0x0A]*", //user_comment #DISCARD
	"\\0x0A", //newline
	"$[a-zA-Z_][a-zA-Z0-9_]*", //lexer_token
	"[^*[\\]+?\\\\()|{}#\\0x00-\\0x1F]", //regex_symbol
	"\\[", //open_bracket
	"\\]", //close_bracket
	"[^", //inverted_open_bracket
	"\\{", //open_curl
	"\\}", //close_curl
	"\\(", //open_brace
	"\\)", //close_brace
	"\\*", //star
	"\\+", //plus
	"\\?", //question
	"\\|", //bar
	"\\\\E", //empty_string
	"-" ,//dash
	"[0-9][0-9]*", //lit_uint
	",", //comma
	"->", //map_symbol
	"#[a-zA-Z_][a-zA-Z0-9_]*", //builtin_parameter
	"=", //equals
	"[0-9][0-9]*:", //enum_tag
	"@", //empty_sym
	"#[a-zA-Z_][a-zA-Z0-9_]*", //builtin_macro
	"#USING", //using_header
	"#DEFAULT", //default_builtin
};
