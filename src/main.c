#include "irapp.h"
#include <locale.h>

int main(int argc, char* argv[]) {
	FILE *f;
	GList *l, *p;
	TToken *t;
	int i;
	
	setlocale(LC_ALL, "");

	if(argc < 2) {
		return -1;
	}
	
	for( i = 1; i < argc; i++ ) {
		f = fopen(argv[i], "r ccs=UTF-8");

		if(!f) {
			printf("Unable to open file %s", argv[i]);
			continue;
		}
	
		l = t_tokenize_file(f);

		for(p = l; p!=NULL; p=p->next) {
			t = (TToken *)p->data;
			wprintf(L"%d: %ls\n", t->position, t->text);
		}

		t_free_list(l);		
		fclose(f);
	}

	return 0;
	
/*	return g_application_run(G_APPLICATION(ir_app_new()), argc, argv); */
}
