#include "tokenizer.h"

/* Worker method that frees up memory that has been allocated to a
 * token. This is used for freeing data in a GList */
static void t_token_free(TToken* token) {
	free(token->text);
	free(token);
}

/* Allocate memory for a new token object and return a pointer to the
 * object. By default, this new token will have a text buffer of size
 * DEFAULT_BUFFER_SIZE and a position of -1 */
TToken* t_token_alloc() {
	TToken* t;

	if( (t = malloc(sizeof (TToken))) != NULL) {
		t->position = -1;
		t->text = malloc(sizeof(wchar_t) * DEFAULT_BUFFER_SIZE);
		t->text[0] = '\0';		
	}	
	return t;
}

/* Tokenizes a string. Need to find some way to unify this with
 * t_tokenize_file. There's too much repeat code */
GList* t_tokenize_str(wchar_t* str) {
	GList* l = NULL;
	TToken* t = NULL;
	char c;
	int i, nw, pos, mul;
	
	nw = 0;
	pos = 0;
	mul = 1;
	
	for( i=1, c=str[0]; c != '\0'; c=str[i++] ) {

		if( nw && (mul * DEFAULT_BUFFER_SIZE) >= (pos-t->position)) {
				t->text = realloc(t->text, sizeof(wchar_t) * DEFAULT_BUFFER_SIZE * ++mul);
		}

		if( iswalpha(c) ) {
			if(!nw) {
				t = t_token_alloc();
				t->position = pos;
			}
			
			t->text[pos - t->position] = tolower(c);
			nw = 1;
		} else if(nw) {
			t->text[pos - t->position] = '\0';
			l = g_list_append(l, t);
			nw = 0;
		}
		pos++;
	}

	if(nw) {
		t->text[pos - t->position] = '\0';
		l = g_list_append(l, t);
	}
	
	return l;
}

/* Tokenizes the contents of a file. Need to find some way to unify
 * this with t_tokenize_str. There's too much repeat code */
GList* t_tokenize_file(FILE* f) {
	GList* l = NULL;
	TToken* t = NULL;
	wchar_t c;
	int nw, pos, mul;

	if(!f) {
		return NULL;
	}
	
	nw = 0;
	pos = 0;
	mul = 1;
	
	while( ( c = fgetwc(f) ) != WEOF ) {
		
		if( nw && (mul * DEFAULT_BUFFER_SIZE) <= (pos-t->position)) {
				t->text = (wchar_t* )realloc(t->text, sizeof(wchar_t*) * DEFAULT_BUFFER_SIZE * ++mul);
		}

		if( iswalpha(c) ) {
			if(!nw) {
				t = t_token_alloc();
				t->position = pos;
			}
			
			t->text[pos - t->position] = towlower(c);
			nw = 1;
		} else if(nw) {
			t->text[pos - t->position] = '\0';
			l = g_list_append(l, t);
			nw = 0;
			mul = 1;
		}
		pos++;
	}

	return l;
}

void t_free_list(GList* list) {
	g_list_free_full(list, (GDestroyNotify) t_token_free);
}
