#include "Parser.h"

/*Load production dev_alphparser into memory. This is the alphabet 
used to parse files that denote grammatical productions.*/
struct LinkList dev_parsertok = { PROD_SIGNAL, NULL };
struct LinkList dev_alphparser = { PROD_END, &dev_parsertok };

/*
Creates the head for the production. This involves allocating the string
that should be returned whenever the rules for this production are met
during parsing. 
*/
struct Production *newProHead(char *prodname, int namesize, struct
	MemBlock *mem)
{
	struct Production *retval = kimalloc(sizeof(struct Production),
		mem);
	if (retval == NULL) {
		return NULL;
	}
	retval->name = kicalloc(sizeof(char) * namesize + 1, mem);
	if (retval->name == NULL) {
		popMemory(sizeof(struct Production), mem);
		return NULL;
	}
	memcpy(retval->name, prodname, namesize);
	retval->next = NULL;
	retval->rules = NULL;
	return retval;
}

/*Tries to add a new rule to the memory block. Returns zero if it was
not able to do it and non-zero if it was able to do so.*/
int addRule(char *val, struct MemBlock *mem, 
	struct Production *prod)
{
	char **ptr = kimalloc(sizeof(char *), mem);
	if (ptr == NULL) {
		return 0;
	}
	else {
		*ptr = val;
		prod->rulesize++;
		return 1;
	}
}

/*
Creates a new production with its rules. 
*/
struct Production *newProduction(struct KiwiInput *input,
	struct LinkList *alphabet, struct MemBlock *parsemem,
	struct MemBlock *lexmem)
{
	struct Production *retval;
	char *gramptr;
	int appendres;
	struct Token token;

	token = lexNext(input, &dev_alphparser, lexmem);
	if (strcmp(token.type, STRING_ID) != 0) {
		return NULL;
	}
	else {}

	retval = newProHead(token.value, strlen(token.value), parsemem);
	if (retval == NULL) {
		return NULL;
	}
	else {}
	retval->rules = (char **)parsemem->memory;

	token = lexNext(input, &dev_alphparser, lexmem);
	if (strcmp(token.type, PROD_SIGNAL) != 0) {
		freeMemory(parsemem);
		return NULL;
	}
	else {}

	__rulelexstart__:
	token = lexNext(input, &dev_alphparser, lexmem);
	gramptr = nul_alphabetContains(token.value, alphabet);
	if (gramptr != NULL) {
		appendres = addRule(gramptr, parsemem, retval);
		if (appendres == 0) {
			return NULL;
		}
		else {
			goto __rulelexstart__;
		}
	}
	else {}
	for (uint8_t x = 0; x < BUILT_IN_AMNT; x++) {
		if (strcmp(*BUILT_IN_TYPES[x], token.value) == 0) {
			appendres = addRule(*BUILT_IN_TYPES[x], parsemem,
				retval);
			if (appendres == 0) {
				return NULL;
			}
			else {
				/*I have sinned.*/
				goto __rulelexstart__;
			}
		}
		else {}
	}
	if (strcmp(token.type, PROD_END) == 0) {
		return retval;
	}
	else {
		return NULL;
	}
}

/*
Adds the production into the LinkedList structure. It appends it to the
list so that the matching hierarchy works as in the written file. 
(Whenever there are two possible productions the first one to be 
declared on the file will be the one chosen).
*/
void addProduction(struct Production **head,
	struct Production *node)
{
	if (*head == NULL) {
		*head = node;
	}
	struct Production *tmphead = *head;
	while ((*head)->next != NULL) {
		*head = (*head)->next;
	}
	(*head)->next = node;
	*head = tmphead;
}

/*Parses all productions and returns the head of the linked list
containing all productions and their rules. Returns NULL if it failed.
*/
struct Production *newParser(struct KiwiInput *input,
	struct LinkList *alphabet, struct MemBlock *parsemem,
	struct MemBlock *lexmem)
{
	struct Production *prodhead;
	struct Production *nexthead;

	prodhead = newProduction(input, alphabet, parsemem, lexmem);
	if (prodhead == NULL)
		return NULL;
	while (1) {
		nexthead = newProduction(input, alphabet, parsemem, lexmem);
		if (nexthead == NULL) {
			return prodhead;
		}
		else {
			addProduction(&prodhead, nexthead);
		}
	}
}

/*
Compares the array of tokens with all the diferent productions inside
the dev_alphparser and returns the production name and its size in tokens.
Bigger productions are preffered over smaller ones so if a match has
been already been found and the next production has less lexemes
or components it will be skipped(not analyzed). Productions that appear
first have precedence over productions later on inside the Parser 
(a LinkedList)
*/
struct Match parseNext(struct Production *parser, 
	struct TokenArray *tokens)
{
	uint8_t matchsize = 0;
	char *prodname = NULL;
	struct Production *tmphead = parser;
	while (parser != NULL) {
		if ((matchsize < parser->rulesize) && 
			(parser->rulesize <= tokens->size)) {
			for (uint8_t x = 0; x < parser->rulesize; x++) {
				const char *rule = *(parser->rules + x);
				const char *token = (tokens->token + x)->type;
				if (strcmp(rule, token) != 0) {
					goto __nextloop__;
				}
				else {}
			}
			matchsize = parser->rulesize;
			prodname = parser->name;
		}
		else {}
		__nextloop__:
		parser = parser->next;
	}
	parser = tmphead;

	struct Match retval;
	retval.id = prodname;
	retval.size = matchsize;
	return retval;
}
