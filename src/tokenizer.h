#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include <stdlib.h>
#include <glib.h>

/**
 * A struct for storing information about an individual
 * token. Contains the text of the token, the number of chars that were
 * scanned before finding the first char of this token and the number of
 * tokens found before this one.
 */
typedef struct _ttoken {
	char* text;          /* The text comprising this token */
	int char_pos;        /* Number of chars before the beginning of this token */
	int word_pos;        /* The number of tokens found before this one */
} TToken;

/**
 * Stores information about the greater tokenization process. Contains
 * a list of tokens found by the tokenization process, the total
 * number of characters scanned and the number of tokens discovered
 */
typedef struct _ttokenizer {
	GList *tokens;        /* The list of tokens found by the tokenizer */
	int   chars_scanned;  /* The number of characters the tokenizer examined */
	int   tokens_scanned; /* The number of tokens the tokenizer found */
} TTokenizer;

/* TToken Methods */
TToken*       t_token_new      (char *text, int position, int wposition);
void          t_token_free     (TToken *token);

/* TTokenizer Method */
TTokenizer*   t_tokenizer_new  ();
void          t_tokenizer_free (TTokenizer *tokenizer);

#endif /* _TOKENIZER_H_ */
