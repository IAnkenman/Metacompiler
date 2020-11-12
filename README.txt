This is a project to produce a metacompilation platform for a variety of reasons, such as:

1) To assist in the development of code generation tools that can produce code based on descriptive schemas
	in conjunction with already written code to automate tasks such as the generation of generic data structures or funtions
	that can't eaisly be done with the languages' existing support for generics, or provide these features for languages that lack them.
	
2) To assist in the development of domain-specific language features that can be embedded in already existing languages without the need for modifying
	the host compiler of the langage, in an effort to more eaisly express various programming models such as concurrency and networking features in a 
	more abstract manner.
	
3) To explore possible alternitives for storing and manipulating source code in a manner that can reduce compile times and disk usage, and help code analysis tools
	by pre-parsing and compressing files based on an understanding of their grammatical structure, instead of using generic compression algorithms that need to be
	converted back to textual source before any operations can be performed on them.
