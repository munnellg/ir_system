#include "index.h"

static GHashTable *idx;
static GList *docs;

/* Allocate memory for a new IDocData structure */
IDocData *i_doc_data_new() {
	IDocData *d;

	if((d = malloc(sizeof(IDocData))) != NULL) {
		d->doc_id = -1;
		d->doc_length = 0;				
	}

	return d;
}

int i_doc_data_compare(IDocData *d1, IDocData *d2) {	
	return d1->doc_id - d2->doc_id;
}

void i_doc_data_free(IDocData *data) {
	if(data != NULL) {
		free(data);
	}
}

IPosting *i_posting_new() {
	IPosting *p;

	if( (p = malloc(sizeof(IPosting))) != NULL ) {
			p->doc_data = NULL;
			p->positions = NULL;
			p->term_freq = 0;
	}
	
	return p;
}

IPostingList *i_posting_list_new() {
	IPostingList *p;

	if( (p = malloc(sizeof(IPostingList))) != NULL ) {
			p->term = NULL;
			p->postings = NULL;
			p->term_freq = 0;
	}
		
	return p;
}

int i_posting_compare(IPosting* posting, int *doc_id) {
	return posting->doc_data->doc_id - *doc_id;
}

void i_posting_free(IPosting *posting) {
	g_list_free(posting->positions);
	free(posting); 
}

void i_posting_list_free(IPostingList *list) {
	if( list ) {
		if( list->term ) {
			free(list->term);
		}

		if( list->postings ) {
			g_list_free_full(list->postings, (GDestroyNotify) i_posting_free); 			
		}

		free(list); 
	}
}

int i_posting_list_compare(IPostingList *l1, IPostingList *l2) {
	int len1, len2;

	len1 = g_list_length(l1->postings);
	len2 = g_list_length(l2->postings);

	return len1 - len2;
}

int i_index_file(char* fname) {
	FILE *f;
	GList *tokens, *p, *p2;
	TToken *token;
	IPostingList *pl;
	IPosting *posting;
	IDocData *doc_data;
	int quark;

	f = fopen(fname, "r");
	
	if(!f) {
		return -1;
	}
	
	quark = g_quark_from_string(fname);

	tokens = t_tokenize_file(f);
	p = tokens;

	printf("Processing %s\n", fname);
	
	/* Store useful information about the file itself */
	doc_data = i_doc_data_new();
	doc_data->doc_id = quark;
	doc_data->doc_length = g_list_length(tokens);
	docs = g_list_append(docs, doc_data);
	
	while(p) {
		token = (TToken *)p->data;
		pl = (IPostingList*) g_hash_table_lookup(idx, token->text);
		
		if(!pl) {
			pl = i_posting_list_new();
			pl->term = strdup(token->text);
			g_hash_table_insert(idx, strdup(token->text), pl);
		}
		
		pl->term_freq++;
		p2 = g_list_find_custom(pl->postings, &quark, (GCompareFunc) i_posting_compare);
		
		if(!p2) {
			posting = i_posting_new();
			posting->doc_data = doc_data;
			pl->postings = g_list_append(pl->postings, posting);
		} else {
			posting = (IPosting *) p2->data;
		}
		
		posting->term_freq++;
		posting->positions = g_list_append(posting->positions, GINT_TO_POINTER(token->position));		
		p = p->next;
	}
	
	t_free_list(tokens);
	fclose(f);

	return 1;
}

static GList *i_index_retrieve_documents(char *query) {
	GList *tokens, *postings_list, *p;	
	TToken *t;
	IPostingList *list;
		
	postings_list = NULL;
	list = NULL;
	
	/* Tokenize the user's query */
	tokens = t_tokenize_str(query);


	printf("%d terms in query\n", g_list_length(tokens));
	
	for(p = tokens; p != NULL; p = p->next) {
		t = (TToken*) p->data;

		list = (IPostingList*) g_hash_table_lookup(idx, t->text);

		if(list) {
			postings_list = g_list_append(postings_list, list);
		} else {
			printf("No key found for %s\n", t->text);
		}
	}

	postings_list = g_list_sort(postings_list, (GCompareFunc) i_posting_list_compare);

	t_free_list(tokens);
	
	return postings_list;
}

static GList *i_index_intersect(GList *p1, GList *p2) {
	GList *result;
	IPosting *l1, *l2;
	
	result = NULL;
	
	while( p1 != NULL && p2 != NULL ) {
		l1 = (IPosting *) p1->data;
		l2 = (IPosting *) p2->data;

		if( i_doc_data_compare(l1->doc_data, l2->doc_data) == 0 ) {
			result = g_list_append(result, l1);
			p1 = p1->next;
			p2 = p2->next;
		} else if( i_doc_data_compare(l1->doc_data, l2->doc_data) < 0 ) {
			p1 = p1->next;
		} else {
			p2 = p2->next;
		}
	}

	return result;
}

static GList *i_index_intersect_many(GList *postings_list) {
	GList *result, *p;
	IPostingList *l;

	result = NULL;
	p = NULL;

	p = postings_list;	

	l = (IPostingList *) p->data;
	result = l->postings;
	p = p->next;

	while(p != NULL && result != NULL) {
		l = (IPostingList *) p->data;
		result = i_index_intersect(result, l->postings);
		p = p->next;
	}

	return result;
}

GList *i_index_query_boolean_and(char *query) {
	GList *postings_list, *intersected, *result, *p;
	char *doc_id;
	IPosting * posting;
	result = NULL;
	
	postings_list = i_index_retrieve_documents(query);
	intersected = i_index_intersect_many(postings_list);	
	
	for(p = intersected; p != NULL; p=p->next) {
		posting = (IPosting*) p->data;
		doc_id = (char *)g_quark_to_string(posting->doc_data->doc_id);
		result = g_list_append(result, doc_id);
	}

	g_list_free(postings_list);
	g_list_free(intersected);
	
	return result;
}

void i_index_initialize() {
	docs = NULL;
	if(idx == NULL) {
		idx = g_hash_table_new_full( g_str_hash,
																 g_str_equal,
																 (GDestroyNotify) free,
																 (GDestroyNotify) i_posting_list_free
			);
	}
}

void i_index_destroy() {
	g_hash_table_destroy(idx);
	idx = NULL;
	g_list_free_full(docs, (GDestroyNotify)i_doc_data_free);
}
