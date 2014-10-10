#include "tokenizer.h"
#include "string.h"

/**
 * Allocates memory for a new TToken struct.
 */
TToken*
t_token_new(char *text, int char_pos, int word_pos) {
	TToken *t;

	/* Allocate memory */
	t = malloc(sizeof(*t));

	/* Initialize attributes if allocation was successful */
	if(t) {
		t->text = text;
		t->char_pos = char_pos;
		t->word_pos = word_pos;
	}

	return t;
}

/**
 * Frees all memory allocated to TToken struct
 */
void
t_token_free(TToken *token) {

	/* Don't try to deallocate a NULL pointer */
	if(token) {
		/* Deallocate the string */
		if(token->text) {
			free(token->text);
		}
		/* Deallocate the TToken*/
		free(token);
	}
}

/**
 * Allocates memory for a new TTokenizer struct.
 */
TTokenizer *
t_tokenizer_new() {
	TTokenizer *tzr;

	/* Allocate memory for the new TTokenizer struct */
	tzr = malloc(sizeof(*tzr));

	/* Initialize struct attributes if allocation successful */
	if(tzr) {
			tzr->tokens = NULL;
			tzr->chars_scanned = 0;
			tzr->tokens_scanned = 0;
	}
	
	return tzr;
}

/**
 * Frees all memory allocated to a TTokenizer struct.
 */
void
t_tokenizer_free(TTokenizer *tokenizer) {
	/* Don't deallocate a NULL pointer */
	if(tokenizer) {
		/* Free the linked list of tokens */
		if(tokenizer->tokens) {
			g_list_free_full(tokenizer->tokens, (GDestroyNotify) t_token_free);			
		}

		/* Free the tokenizer */
		free(tokenizer);
	}
}
