#include "indexer.h"
#include <string.h>

IIndex *g_index=NULL;

static int
to_list_node(gpointer key, gpointer value, gpointer user_data) {
	GList **list;

	list = (GList **)user_data;
	list[0] = g_list_prepend(list[0], value);
	list[1] = g_list_prepend(list[1], key);
	
	return 0;
}

static GList *
tree_to_sorted_list(GTree *tree, int return_keys) {
	GList **list;
	GList *tmp;
	
	list = malloc(sizeof(*list)*2);

	if(!list) {
		return NULL;
	}
	
	list[0] = NULL;
	list[1] = NULL;
	
	g_tree_foreach(tree, (GTraverseFunc)to_list_node, list);		

	tmp = (return_keys)? list[1] : list[0];
	tmp = g_list_reverse(tmp);
	
	g_list_free(list[(return_keys==0)]);
	free(list);
	return tmp;
}

static
GList *intersect(GList *l1, GList *l2) {
	GList     *p1, *p2, *result;
	char      *c1, *c2;
	int       cmp;

	p1 = l1;
	p2 = l2;
	
	result = NULL;
	
	while( p1 != NULL && p2 != NULL ) {
		c1 = (char *) p1->data;
		c2 = (char *) p2->data;

		cmp = strcmp(c1, c2);
		
		if( cmp == 0 ) {
			result = g_list_append(result, c1);
			p1 = p1->next;
			p2 = p2->next;
		} else if( cmp < 0 ) {
			p1 = p1->next;
		} else {
			p2 = p2->next;
		}
	}
	
	return result;
}

static
GList *intersect_many(GList *postings_lists) {
	GList *result, *p, *t1, *t2;
	IPostingList *pl;

	postings_lists = g_list_sort(postings_lists,
															 (GCompareFunc) i_posting_list_cmp_docs);
	postings_lists = g_list_reverse(postings_lists);
	
	result = NULL;
	p = postings_lists;	

	if(p) {
		pl = (IPostingList *) p->data;
		result = (pl)? tree_to_sorted_list(i_posting_list_get_postings(pl), 1) : NULL;
		p = p->next;
	}	

	for(; p!= NULL && result != NULL; p=p->next) {
		pl = (IPostingList *) p->data;
		t1 = result;
		t2 = (pl)? tree_to_sorted_list(i_posting_list_get_postings(pl), 1) : NULL;
		
		result = intersect(t1, t2);
		
		g_list_free(t1);
		g_list_free(t2);
	}

	return result;
}

static GList *
combine( GList *l1, GList *l2 ) {
	GList     *p1, *p2, *result;
	char      *c1, *c2;
	int       cmp;

	p1 = l1;
	p2 = l2;

	result = NULL;
	
	if(!p1) {
		result = g_list_copy(p2);
	}

	if(!p2) {
		result =  g_list_copy(p1);
	}
		
	while( p1 != NULL && p2 != NULL ) {
		c1 = (char *) p1->data;
		c2 = (char *) p2->data;

		cmp = strcmp(c1, c2);
		
		if( cmp == 0 ) {
			result = g_list_append(result, c1);
			p1 = p1->next;
			p2 = p2->next;
		} else if( cmp < 0 ) {
			result = g_list_append(result, c1);
			p1 = p1->next;
		} else {
			result = g_list_append(result, c2);
			p2 = p2->next;
		}
	}

	return result;
}

static GList *
combine_many(GList *postings_lists) {
	GList *result, *p, *t1, *t2;
	IPostingList *pl;

	postings_lists = g_list_sort(postings_lists,
															 (GCompareFunc) i_posting_list_cmp_docs);
	postings_lists = g_list_reverse(postings_lists);
	
	result = NULL;
	p = postings_lists;	

	if(p) {
		pl = (IPostingList *) p->data;
		result = (pl)? tree_to_sorted_list(i_posting_list_get_postings(pl), 1) : NULL;
		p = p->next;
	}	

	for(; p!= NULL && result != NULL; p=p->next) {
		pl = (IPostingList *) p->data;
		t1 = result;
		t2 = (pl)? tree_to_sorted_list(i_posting_list_get_postings(pl), 1) : NULL;
		
		result = combine(t1, t2);
		
		g_list_free(t1);
		g_list_free(t2);
	}

	return result;
}

static int
get_postings_list(char *key, char *value, gpointer user_data) {
	IPostingList *pl;
	GTree *tree;

	tree = (GTree *)user_data;
	
	pl = i_index_get_term_postings(g_index, value);

	g_tree_insert(tree, value, pl);
	
	return 0;
}

static GTree *
get_tokenizer_terms(TTokenizer *tokenizer) {
	GTree  *result;
	GList  *p;
	TToken *t;
	char   *dup1, *dup2;

	result = g_tree_new_full(
		(GCompareDataFunc)strcmp,
		NULL,
		(GDestroyNotify)free,
		(GDestroyNotify)free
	);
	
	for(p = tokenizer->tokens; p!=NULL; p=p->next) {
		t = (TToken *)p->data;
		dup1 = strdup(t->text);
		dup2 = strdup(t->text);
		g_tree_insert(result, dup1, dup2);
	}
	
	return result;
}

static GTree *
get_postings_lists_tree(TTokenizer *tokenizer) {
	GTree *terms;
	GTree *postings_lists;

	postings_lists = g_tree_new_full(
		(GCompareDataFunc)strcmp,
		NULL,
		NULL,
		NULL
	);
	
	terms = get_tokenizer_terms(tokenizer);
	g_tree_foreach(terms, (GTraverseFunc)get_postings_list, postings_lists);	
	g_tree_destroy(terms);

	return postings_lists;
}

static GList *
get_postings_lists_list(TTokenizer *tokenizer) {
	GTree *tree;
	GList *list;

	tree = get_postings_lists_tree(tokenizer);
	list = tree_to_sorted_list(tree, 0);
	g_tree_destroy(tree);

	return list;
}

static GList *
boolean_or( TTokenizer *tokenizer ) {
	GList *postings_lists, *result;
		
	postings_lists = get_postings_lists_list(tokenizer);
	result = combine_many(postings_lists);
		
	g_list_free(postings_lists);
	return result;
}

static GList *
boolean_and( TTokenizer * tokenizer ) {
	GList *postings_lists, *result;
		
	postings_lists = get_postings_lists_list(tokenizer);
	result = intersect_many(postings_lists);
		
	g_list_free(postings_lists);
	return result;
}

void
i_indexer_init () {
	if(!g_index) {
		g_index = i_index_new();		
	}
}

GList *
i_indexer_query(char *query) {
	TTokenizer *tokenizer;
	GList      *result;
	
	tokenizer = t_tokenize_str(query);
	result = NULL;
	
	if(tokenizer) {
		result = boolean_or(tokenizer);
	}

	t_tokenizer_free(tokenizer);
	
	return result;
}

int
i_indexer_index_document(char *fname) {
	return i_index_add_document(g_index, fname);
}

GList *
i_indexer_get_documents() {
	GList *result;

	result = tree_to_sorted_list(i_index_get_document_tree(g_index), 1);
	return result;
}

GList *
i_indexer_get_indexed_terms() {
	return g_hash_table_get_keys(i_index_get_lookup(g_index));
}

IPostingList *
i_indexer_get_term_posting_list(char *term) {
	return i_index_get_term_postings(g_index, term);
}

void
i_indexer_reset () {
	i_indexer_destroy();
	i_indexer_init();
}

void
i_indexer_destroy () {
	if(g_index) {
		i_index_free(g_index);
		g_index = NULL;		
	}
}
