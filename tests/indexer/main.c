#include "indexer.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	int i;
	char *query;
	GList *result, *p;
	
	query = "Cthulhu Fucking";
	
	i_indexer_init();

	for(i=1; i<argc; i++) {
		printf("processing: %s\n", argv[i]);
		i_indexer_index_document(argv[i]);
	}
	
	result = i_indexer_query(query);

	for(p=result; p!=NULL; p=p->next) {
		printf("%s\n", (char*)p->data);
	}

	g_list_free(result);
	i_indexer_destroy();

	return 0;
}
