#ifndef _INDEXER_H_
#define _INDEXER_H_

#include "index.h"
#include "tokenize.h"

void          i_indexer_init                 ();
GList*        i_indexer_query                (char *query);
int           i_indexer_index_document       (char *fname);
GList*        i_indexer_get_documents        ();
GList*        i_indexer_get_indexed_terms    ();
IPostingList* i_indexer_get_term_posting_list(char *term);
void          i_indexer_reset                ();
void          i_indexer_destroy              ();
#endif /* _INDEXER_H_ */
