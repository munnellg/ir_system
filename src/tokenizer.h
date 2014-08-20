#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>

#define DEFAULT_BUFFER_SIZE 12

typedef struct _TToken {
	int position;
	wchar_t* text;
} TToken;

GList* t_tokenize_str(wchar_t* str);
GList* t_tokenize_file(FILE* f);

void t_free_list(GList* list);

#endif /* _TOKENIZER_H_ */
