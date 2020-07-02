static const char * rules[] = {
	"#LEXER", //lexer_header
	"#PARSER", //parser_header
	"#POSTPROCESS", //postprocess_header
	"#TRANSFORMS", //transforms_header
	"#COMMON", //common_header
	"[a-zA-Z_][a-zA-Z0-9_]*", //id
	"[\\0x00-\\0x09\\0x0B-\\0x20\\0x7F]", //whitespace #DISCARD
	"##[^\\0x0A]*", //user_comment #DISCARD
	"\\0x0A", //newline
	"$[a-zA-Z_][a-zA-Z0-9_]*", //lexer_token
	"[^*\\[\\]+?\\\\()|{}#\\0x00-\\0x1F]", //regex_symbol
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
	"[0-9]+", //lit_uint
	",", //comma
	"->", //map_symbol
	"#[a-zA-Z_][a-zA-Z0-9_]*", //builtin_parameter
	"=", //equals
	"[0-9]+:", //enum_tag
	"@", //empty_sym
	"#[a-zA-Z_][a-zA-Z0-9_]*", //builtin_macro
	"#USING", //using_header
	"#DEFAULT", //default_builtin
};
