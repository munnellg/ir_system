#include "index.h"
#include "tokenize.h"
#include "string.h"

struct _document {
	char *name;
	int word_count;
	int char_count;
};

struct _posting {
	char  *doc_id;
	int    doc_term_freq;
	GList *positions;
};

struct _postinglist {
	int     col_term_freq;
	char   *token;
	GTree  *postings;
};

struct _index {
	GHashTable *lookup;
	GTree      *docs;
};

static void
add_to_docs_tree(IIndex *idx, TTokenizer *tokenizer, char *fname) {
	IDocument *doc;
	char      *fname_dup;
	
	doc = i_document_new(fname,
											 tokenizer->tokens_scanned,
											 tokenizer->chars_scanned);

	fname_dup = strdup(fname);
	
	g_tree_insert(idx->docs, fname_dup, doc);
}

static void
update_posting(IPostingList *pl, TToken *token, char *fname) {
	IPosting *p;
	char     *fname_dup;
	
	p = (IPosting *) g_tree_lookup(pl->postings, fname);

	if(!p) {
		p = i_posting_new(fname);
		fname_dup = strdup(fname);
		g_tree_insert(pl->postings, fname_dup, p);
	}

	p->doc_term_freq++;
	p->positions = g_list_prepend(p->positions,
																GINT_TO_POINTER(token->word_pos));
}

static void
update_posting_list(IIndex *idx, TToken *token, char *fname) {
	IPostingList *pl;
	char         *tmp;
		
	pl = (IPostingList*) g_hash_table_lookup(idx->lookup, token->text);
	
	if(!pl) {
		pl = i_posting_list_new(token->text);
		tmp = strdup(token->text);
		
		g_hash_table_insert(idx->lookup, tmp, pl);
	}
	
	update_posting(pl, token, fname);
	pl->col_term_freq++;
}

static void
add_to_lookup_table(IIndex *idx, TTokenizer *tokenizer, char *fname) {
	GList  *p;
	TToken *token;
	
	for(p = tokenizer->tokens; p!=NULL; p=p->next) {
		token = (TToken *) p->data;
		update_posting_list(idx, token, fname);
	}
}

IIndex*
i_index_new() {
	IIndex *idx;

	idx = malloc(sizeof(*idx));

	if(idx) {
		idx->docs = g_tree_new_full(
			(GCompareDataFunc)strcmp,
			NULL,
			(GDestroyNotify)free,
			(GDestroyNotify)i_document_free
		);
	
		idx->lookup = g_hash_table_new_full(
			g_str_hash,
			g_str_equal,
			(GDestroyNotify) free,
			(GDestroyNotify) i_posting_list_free
		);		
	}

	return idx;
}

IPostingList *
i_index_get_term_postings(IIndex *idx, char *term) {
	IPostingList *pl;
		
	pl = (IPostingList*) g_hash_table_lookup(idx->lookup, term);

	return pl;
}

GTree *
i_index_get_document_tree(IIndex *idx) {
	return idx->docs;
}

GHashTable *
i_index_get_lookup(IIndex *idx) {
	return idx->lookup;
}

int
i_index_add_document (IIndex *idx, char *fname) {
	TTokenizer *tokenizer;

	tokenizer = t_tokenize_file(fname);

	if(!tokenizer) {
		return 0;
	}

	add_to_docs_tree(idx, tokenizer, fname);
	add_to_lookup_table(idx, tokenizer, fname);
	
	t_tokenizer_free(tokenizer);
	
	return 1;
}

void
i_index_free (IIndex *idx) {

	if(idx->docs) {
		g_tree_destroy(idx->docs);
	}

	if(idx->lookup){
		g_hash_table_destroy(idx->lookup);
	}
	
	free(idx);
}

IDocument*
i_document_new (char *fname, int wcount, int ccount) {
	IDocument *doc;

	doc = malloc(sizeof(*doc));

	if(doc) {
		doc->name = strdup(fname);
		doc->word_count = wcount;
		doc->char_count = ccount;
	}
	
	return doc;
}

void
i_document_free (IDocument *doc) {
	if(doc->name) {
		free(doc->name);
		doc->name = NULL;
	}
	free(doc);
}

IPosting *
i_posting_new(char *fname) {
	IPosting *p;

	p = malloc(sizeof(*p));

	if(p) {
		p->doc_id = strdup(fname);
		p->doc_term_freq = 0;
		p->positions = NULL;
	}
	
	return p;
}

char *
i_posting_get_id(IPosting *p) {
	return p->doc_id;
}

void
i_posting_free(IPosting *posting) {
	if(posting->doc_id) {
		free(posting->doc_id);
	}

	if(posting->positions) {
		g_list_free(posting->positions);
	}
	
	free(posting);
}

IPostingList *
i_posting_list_new (char *text) {
	IPostingList *pl;

	pl = malloc(sizeof(*pl));

	if(pl) {
		pl->col_term_freq = 0;
		pl->token = strdup(text);
		pl->postings = g_tree_new_full(
			(GCompareDataFunc)strcmp,
			NULL,
			(GDestroyNotify)free,
			(GDestroyNotify)i_posting_free
		);
	}
	
	return pl;
}

char *
i_posting_list_get_token(IPostingList *pl) {
	return pl->token;
}

int
i_posting_list_get_freq(IPostingList *pl) {
	return pl->col_term_freq;
}

GTree *
i_posting_list_get_postings(IPostingList *p) {
	return p->postings;
}

int
i_posting_list_cmp_docs(IPostingList *p1, IPostingList *p2) {
	if(!p1 && !p2) {
		return 0;
	}

	if(p1 && !p2) {
		return 1;
	}

	if(!p1 && p2) {
		return -1;
	}
	
	return g_tree_nnodes(p1->postings) - g_tree_nnodes(p2->postings);
}

void
i_posting_list_free (IPostingList *posting_list) {
	if(posting_list->token) {
		free(posting_list->token);
	}

	if(posting_list->postings) {
		g_tree_destroy(posting_list->postings);
	}

	free(posting_list);
}

