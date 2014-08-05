#ifndef _INDEX_H_
#define _INDEX_H_

#include "tokenizer.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _posting {
	int doc_id;
	int term_freq;
	GList *positions;
} IPosting;

typedef struct _postinglist {
	int term_freq;
	GList *postings;
} IPostingList;

GHashTable* i_index_file(GHashTable *index, int doc_id, FILE* f);

#endif /* _INDEX_H_ */
