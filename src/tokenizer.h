#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_BUFFER_SIZE 12

typedef struct _TToken {
	int position;
	char* text;
} TToken;

GList* t_tokenize_str(char* str);
GList* t_tokenize_file(FILE* f);

void t_free_list(GList* list);

#endif /* _TOKENIZER_H_ */
