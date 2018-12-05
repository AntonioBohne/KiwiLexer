#include "Tokenizer.h"

struct MemBlock lexMem;

/*Initializes the statically allocated memory block so that it 
can replace any dynamic allocation memory.*/
void initTokenizerMemory(char *ptr, size_t size)
{
	lexMem.memory = ptr;
	lexMem.memsize = size;
	lexMem.used = 0;
}

char *readAll(FILE *fl, unsigned int *sizeread, int sector)
{
	char *rdata = NULL;
	int count = 1;
	do {
		fseek(fl, 0, SEEK_SET);
		rdata = realloc(rdata, sizeof(char) * (sector * count));
		*sizeread = fread(rdata, 1, (sector * count), fl);
		count++;
	} while (feof(fl) == 0);

	rdata = realloc(rdata, *sizeread);
	return rdata;
}

int skipchar(char *ptr, char until, int limit)
{
	int x = 0;
	for (; x < limit; ptr++, x++) {
		if (*ptr == until)
			return x;
	}
	return x;
}

struct LinkList *mem_newLinkToken(char *id, int idsize)
{
	struct LinkList *retval = kimalloc(sizeof(struct LinkList),
		&lexMem);
	retval->value = kicalloc(sizeof(char) * (idsize + 1), &lexMem);
	memcpy(retval->value, id, idsize);
	retval->next = NULL;
	return retval;
}


struct LinkList *mem_createTokenizer(char *grammar, int gramsize)
{
	struct LinkList *retval = NULL;
	int x = 0; 
	for (; x < gramsize; grammar++, x++) {
		if (*grammar == TOKEN_LIMIT) {
			grammar++;
			x++;
			int len = skipchar(grammar, TOKEN_LIMIT, gramsize - x);
			struct LinkList *tok = mem_newLinkToken(grammar, len);
			len++; 
			x += len;
			grammar += len;
			appendToList(tok, &retval);
		}
	}
	return retval;
}

/*
Returns a pointer to the string inside the tokenizer if it exists
or NULL if it does not.
*/
char *contains(char *val, int valsz, struct LinkList *tokenizer)
{
	struct LinkList *tmphead = tokenizer;
	char *retval;
	while (tokenizer != NULL) {
		if ((strncmp(val, tokenizer->value, valsz) == 0) &&
			(strlen(tokenizer->value) == valsz)) {
			retval = tokenizer->value;
			tokenizer = tmphead;
			return retval;
		}
			
	}
	tokenizer = tmphead;
	return NULL;
}

void mem_printTokenizer(struct LinkList *head)
{
	struct LinkList *tmphead = head;
	char prntcount = 0;
	while (head != NULL) {
		if (prntcount >= 6) {
			prntcount = 0;
			printf("\n");
		}
		printf("<TOKEN: \"%s\"> ", (char *)head->value);
		head = head->next;
		prntcount++;
	}
	head = tmphead;
}



