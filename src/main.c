#include "tokenizer.h"
#include "index.h"
#include "stem.h"
#include <stdio.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
	FILE *f;
	GList *p, *p2;
	GHashTable *index;
	GHashTableIter itr;
	gpointer key, value;
	IPosting *posting;
	IPostingList* pl;
	int i;
	char* token;
	
	index = NULL;
	
	if(argc < 2) {
		printf("Usage: ir <file1>...\n");
		return -1;
	} 
	
	for(i=1; i<argc; i++) {
		printf("Processing %d... ", i);
		f = fopen(argv[i], "r");
		index = i_index_file(index, i, f);		
		fclose(f);
		printf("OK\n");
	}

	g_hash_table_iter_init(&itr, index);

	while (g_hash_table_iter_next (&itr, &key, &value)) {
		token = (char*) key;
		pl = (IPostingList *) value;
		printf("%s, %d: {\n", token, pl->term_freq);
		p = pl->postings;
		while(p) {
			posting = (IPosting*)p->data;
			printf("\t%d, %d: {\n", posting->doc_id, posting->term_freq);
			p2 = posting->positions;
			printf("\t\t");
			while(p2) {
				printf("%d, ", GPOINTER_TO_INT(p2->data));
				p2 = p2->next;
			}
			printf("\n\t}\n");					
			p = p->next;
		}
		printf("}\n");
	}

	g_hash_table_destroy(index);
	return 0;
}
