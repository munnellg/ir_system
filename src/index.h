#ifndef _INDEX_H_
#define _INDEX_H_

#include "tokenizer.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _docdata {
	int doc_id;
	int doc_length;
} IDocData;

typedef struct _posting {
	IDocData *doc_data;
	int term_freq;
	GList *positions;
} IPosting;

typedef struct _postinglist {
	char* term;
	int term_freq;
	GList *postings;
} IPostingList;

int i_index_file(char *fname);
GSList *i_index_query_boolean_and(char *query);
void i_index_initialize();
void i_index_destroy();

IPosting *i_posting_new();
int i_posting_compare(IPosting *posting, int *doc_id);
void i_posting_free(IPosting *posting); 

IPostingList *i_posting_list_new();
int i_posting_list_compare(IPostingList *l1, IPostingList *l2);
void i_posting_list_free(IPostingList *list);

IDocData *i_doc_data_new();
int i_doc_data_compare(IDocData *d1, IDocData *d2);
void i_doc_data_free(IDocData *data);

GSList *i_docs_get_list();

#endif /* _INDEX_H
_ */
