This is the draft for the metalanguage.

It is comprised of a common section, multiple lexer sections (optionally followed by a postproccesing section) and a parser segment (optionally followed by a transforms segment).

The common segment contains commonly used tokens that can be referenced by subsequent lexer definitions. Lexer tokens all begin with '$'.

The lexer sections contain sets of lexical rules used to match tokens in the input text.

The post process segment that follows can be used to map some a given token to another with a user provided function that determines the metadata stored in the token. (Useful for handling escape sequences)

The parser segment contain all the parse rules needed to build the parse tree. It allows using a subset of regular expression syntax to represnt rules more concisely.

	Example: grammar = (constants | modifiers)*

	Equivalent to in BNF:
		grammar = statements
		statements =	statements statements | constants | modifiers

When the 'or' operation is used (the bar: '|') rather than expanding the rule and adding the implied extra non-terminal characters, the parser treats it as a union of types both in the output code and the transforms segment of the grammar.
The parser also allows for the creation of tagged union types, which are useful for describing trasformations where the outputs may become ambiguous (explained below).
The #USING(id) syntax allows the parser to swap what lexer is being used to tokenize the input for a given match.

The repeat operation ('*') produces arrays of tokens in the output code and transforms segment rather than the traditonal linked list produced from BNF.

The transforms segment contains a list of transformations to be applied to the parse tree during construction. Trasformations can be described in two ways: 1) with basic mapping or 2) a full code segment.

	1) non_terminal -> <some subeset of the given non-terminal's component types>
	2) non_terminal: %root {
		##User code here
		%root = %newRoot ##Eventual assignment to the original variable specified at entry to determine the new type.
	}

The simple mapping is useful for removing unnecessary tokens from rules:
	Given the lexer :
		$semicolon ;

	Given the grammar:
		line = statement $semicolon
		statement = ## Other stuff ##

	The semicolon is unnecesssary and only used for seperation, so it can be removed from the output using a transform like so:

	line -> statement

	This allows output parse trees to be simpler, as the output parse tree will no longer have the semicolon included in it.

This can also cause ambiguity however:

	Given the lexer:
		$minus -
		$plus \+

	Given the grammar:
		op = op $minus op | op $plus op

	The mapping { op -> op op } would make the two operations appear identical. (and they would be output as such) this is where tagged unions come in handy

	If the grammar was expressed as such:
		op =	1: op $minus op
			2: op $plus op

	Then the mappings could be described seperatly as such:
		op[1] -> op op
		op[2] -> op op

Now the two possible parse options have the same resultant type, but will still be distinguished in both the output code and when constructing them in other transform rules

If a question mark is used in a parse rule a default can be constructed in a simple mapping as well:

	Given a grammar:
		some_line = operation postfix_op?

	A mapping could be decribed like:
		some_line -> operation postfix_op(## Constructor args ##)

If no default mapping is provided the statement would be expanded into a union type as if the '|' operation were used:

	With no mapping the above examle would be equivalent to:
		some_line = operation | operation postfix_op

Constructing an instance of a type is explained in detail later in this text.

Describing a transform using the code segment allows much more flexibility, as it is a fully fledged language with strong typing based on the parse rules.

Variables are declared by supplying a type and then an variable name starting with the '%' character.

A #SWITCH statement can be used for pattern matching the type of a variable.

#IF statements can be used and can be followed by any number of #ELSEIF clauses.

#FOR and #WHILE blocks can be used and terminated with #END

#DO blocks can be used and terminated with a #WHILE clause

All block statements are delimited using brackets '{' '}'.

If a variable is a numerical type standard mathematical operators work as expected ('+', '-', '*', '/')

Builtin types are #CHAR, #INT, #UINT, and #STRING

Each parse rule is its own unique type

New instances of nodes in the parse tree can be constructed in the code segment using lisp style syntax:
	
	Given grammar:
		equation = op
		op =	1: op $plus op
			2: op $minus op
			3: $number

	And transforms:
		op[1] -> op op
		op[2] -> op op

	A statement to define, generate, and assign the parse tree of "10 + 4 - 2"  to an equation node could look like:
		equation: %root {
			#INT %i = 10
			op %newNode = op[2]( op[1]( %i, 4 ), 2)
			%root = equation(%newNode)
		}

Notice how tagged union syntax was uses to determine what type of 'op' node to construct. This is because code in the transforms segment sees every type as its POST transformed type.

In the above example trying to define an 'op' node as 'op(10, 3)' or even 'op(10, $plus, 3)' would NOT work.

Using 'op(10)' DOES however work, as the type in this case can be inferred without ambiguity.

List types can also be created by appending '*' or '+' to a type for zero or non zero types respectively.

If a variable is assigned a type that represents a parser node its members can be accsessed with dot syntax. Using the above example:

	equation: %root: {
		op %foo = %root.op ##This works just fine.
	}

If the type is a union of possible nodes the variable must be matched with a case statement before accsessing its members:

	op: %root {
		#SWITCH %root
		#CASE 1
			op %foo = %root.op[0] ##Bracket syntax when used with the dot operator is *not* an instance of the tagged union syntax. Here they represent which instance of 'op' to get.
		#CASE 2
		##Some stuff
		#CASE 3
			$number %bar = %root.Snumber ##Lexer tokens are types and can be accessd as well
	}

Another example using bracket syntax as member access for clarity:

	Grammar:
		foo = bar bar

	Transform:
		foo: %root {
			bar %var1 = %root[0] ##Selects first instance of bar in foo:  foo = [bar]  bar
			bar %var2 = %root[1] ##Selects second instance of bar in foo: foo =  bar  [bar]
		}	
