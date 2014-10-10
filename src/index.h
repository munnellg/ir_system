#ifndef _INDEX_H_
#define _INDEX_H_

#include <glib.h>

typedef struct _document    IDocument;
typedef struct _posting     IPosting;
typedef struct _postinglist IPostingList;
typedef struct _index       IIndex;

IIndex*       i_index_new                 ();
IPostingList* i_index_get_term_postings   (IIndex *idx, char *term);
GTree*        i_index_get_document_tree   (IIndex *idx);
GHashTable*   i_index_get_lookup          (IIndex *idx);
int           i_index_add_document        (IIndex *idx, char *fname);
void          i_index_free                (IIndex *idx);

IDocument*    i_document_new              (char *fname, int wcount, int ccount);
void          i_document_free             (IDocument *doc);

IPosting*     i_posting_new               (char *fname);
char*         i_posting_get_id            (IPosting *posting);
void          i_posting_free              (IPosting *posting);

IPostingList* i_posting_list_new          (char *text);
char*         i_posting_list_get_token    (IPostingList *posting_list);
int           i_posting_list_get_freq     (IPostingList *posting_list);
GTree*        i_posting_list_get_postings (IPostingList *p);
int           i_posting_list_cmp_docs     (IPostingList *p1, IPostingList *p2);
void          i_posting_list_free         (IPostingList *posting_list);
#endif /* _INDEX_H_ */
