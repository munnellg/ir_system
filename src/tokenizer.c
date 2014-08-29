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
		t->position = 0;
		t->text = NULL;
	}	
	return t;
}

GList *t_tokenize_str(char *str) {
	wchar_t *buffer = NULL;
	int len;

	len = strlen(str)+1;	
	buffer = (wchar_t *)malloc(sizeof(wchar_t) * len);
	mbstowcs(buffer, str, len);
	return t_tokenize_wstr(buffer);
}

/* Tokenizes a string. Need to find some way to unify this with
 * t_tokenize_file. There's too much repeat code */
GList* t_tokenize_wstr(wchar_t* str) {
	GList* l = NULL;
	TToken* t = NULL;
	wchar_t *text, c;
	int i, pos, mul, len;
	
	pos = 0;
	mul = 1;
	
	for( i=0; i<wcslen(str); i++ ) {
		c = str[i];

		if( text && (mul * DEFAULT_BUFFER_SIZE) <= (pos-t->position)) {
				text = (wchar_t* )realloc(text, sizeof(wchar_t*) * DEFAULT_BUFFER_SIZE * mul++);
		}

		if( iswalpha(c) ) {
			if(!text) {
				t = t_token_alloc();
				text = malloc(sizeof(wchar_t) * DEFAULT_BUFFER_SIZE);
				t->position = pos;
			}
			
			text[pos - t->position] = towlower(c);			
		} else if(text) {
			text[pos - t->position] = '\0';
			len = wcslen(text) * sizeof(text[0]);
			
			t->text = malloc(sizeof(char) * len);
			wcstombs(t->text, text, len);
			l = g_list_append(l, t);
			
			free(text);
			text = NULL;
			mul = 1;
		}
		pos++;
	}

	if(text) {
		text[pos - t->position] = '\0';
		len = wcslen(text) * sizeof(text[0]);
		t->text = malloc(sizeof(char) * len);
		wcstombs(t->text, text, len);
		l = g_list_append(l, t);
			
		free(text);
	}
	
	return l;
}

/* Tokenizes the contents of a file. Need to find some way to unify
 * this with t_tokenize_str. There's too much repeat code */
GList* t_tokenize_file(FILE* f) {
	GList* l = NULL;
	TToken* t = NULL;
	wchar_t *text, c;
	int pos, mul, len;

	if(!f) {
		return NULL;
	}

	text = NULL;
	pos = 0;
	mul = 1;
	
	while( ( c = fgetwc(f) ) != WEOF ) {
		
		if( text && (mul * DEFAULT_BUFFER_SIZE) <= (pos-t->position)) {
				text = (wchar_t* )realloc(text, sizeof(wchar_t*) * DEFAULT_BUFFER_SIZE * mul++);
		}

		if( iswalpha(c) ) {
			if(!text) {
				t = t_token_alloc();
				text = malloc(sizeof(wchar_t) * DEFAULT_BUFFER_SIZE);
				t->position = pos;
			}
			
			text[pos - t->position] = towlower(c);			
		} else if(text) {
			text[pos - t->position] = '\0';
			len = wcslen(text) * sizeof(text[0]);
			t->text = malloc(sizeof(char) * len);
			wcstombs(t->text, text, len);
			l = g_list_append(l, t);
			free(text);
			text = NULL;
			mul = 1;
		}
		pos++;
	}

	return l;
}

void t_free_list(GList* list) {
	g_list_free_full(list, (GDestroyNotify) t_token_free);
}
