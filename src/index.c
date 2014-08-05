#include "index.h"

static IPosting *i_new_posting() {
	IPosting *p;

	p = malloc(sizeof(IPosting));
	
	p->doc_id = -1;
	p->term_freq = 0;
	p->positions = NULL;
		
	return p;
}

static IPostingList *i_new_posting_list() {
	IPostingList *p;

	p = malloc(sizeof(IPostingList));
	
	p->term_freq = 0;
	p->postings = NULL;
		
	return p;
}

static int i_posting_compare(IPosting* posting, int *doc_id) {
	return posting->doc_id - *doc_id;
}

static void i_posting_free(IPosting *posting) {	
	g_list_free(posting->positions);
	free(posting); 
}

static void i_posting_list_free(IPostingList *list) {
	g_list_free_full(list->postings, (GDestroyNotify) i_posting_free); 
	free(list); 
}

GHashTable *i_index_file(GHashTable *index, int doc_id, FILE *f) {
	GList *tokens, *p, *p2;
	TToken *token;
	IPostingList *pl;
	IPosting *posting;
	
	if(index == NULL) {
		index = g_hash_table_new_full( g_str_hash,
																	 g_str_equal,
																	 (GDestroyNotify) free,
																	 (GDestroyNotify) i_posting_list_free
			);
	}

	tokens = t_tokenize_file(f);
	p = tokens;

	while(p) {
		token = (TToken *)p->data;
		pl = (IPostingList*) g_hash_table_lookup(index, token->text);		
		if(!pl) {
			pl = i_new_posting_list();
			g_hash_table_insert(index, strdup(token->text), pl);
		}
		
		pl->term_freq++;
		p2 = g_list_find_custom(pl->postings, &doc_id, (GCompareFunc) i_posting_compare);
		
		if(!p2) {
			posting = i_new_posting();
			posting->doc_id = doc_id;
			pl->postings = g_list_append(pl->postings, posting);
		} else {
			posting = (IPosting *) p2->data;
		}

		posting->term_freq++;
		posting->positions = g_list_append(posting->positions, GINT_TO_POINTER(token->position));		
		p = p->next;
	}
	
	t_free_list(tokens);
	return index;
}
