#include "irapp.h"
#include <locale.h>

void run_query(char *query) {
	GSList *docs, *p;

	printf("Running query %s\n", query);
	docs = i_index_query_boolean_and(query);
	p = docs;
	while (p) {
		printf("%s\n", (char*)p->data);
		p = p->next;
	}
	printf("\n");
	g_slist_free(docs);
}

int main(int argc, char* argv[]) {
/*	int i;
	i_index_initialize();
	
	if(argc < 2) {
		printf("No files passed as argument\n");
		return -1;
	}

	for(i=1; i<argc; i++) {
		i_index_file(argv[i]);
	}

	run_query("Test query 1");
	run_query("Test query 1");
	run_query("Cthulhu");
	run_query("Yig");
	
	i_index_destroy();	

	return 0;*/
	return g_application_run(G_APPLICATION(ir_app_new()), argc, argv);
}
