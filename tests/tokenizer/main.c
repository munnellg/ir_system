#include "tokenize.h"
#include <glib.h>

void
print_list(TTokenizer *l) {
	GList  *p;
	TToken *t;
	
	for(p=l->tokens; p!=NULL; p=p->next){
		t = (TToken *)p->data;
		printf("%s, %d, %d\n",
					 t->text,
					 t->char_pos,
					 t->word_pos
			);
	}

	printf("Tokens processed: %d\n", l->tokens_scanned);
	printf("Characters processed: %d\n", l->chars_scanned);
}

void
test_string(char *str) {
	TTokenizer *l;
	
	l = t_tokenize_str(str);
	print_list(l);
	t_tokenizer_free(l);
}

void
test_file(char *fname) {
	TTokenizer *l;
	
	l = t_tokenize_file(fname);
	t_tokenizer_free(l);
}

int main(int argc, char* argv[]) {
	int i;

	if(argc>=2) {
		for(i=1; i<argc; i++) {
			printf("Processing %s\n", argv[i]);
/*			test_file(argv[i]); */
		}
	} else {
		test_string( "This is a test string. "
								"Antidisestablishmentarianism. That was a very, very long word. "
								"Hölderlin. That word contained a UTF-8 encoded character.");
	}
	return 0;
}
