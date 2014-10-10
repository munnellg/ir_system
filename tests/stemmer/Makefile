all:
	gcc -g -Wall -pedantic stem.c tokenizer.c tokenize.c main.c $(pkg-config --cflags --libs glib-2.0) -o stemmer `pkg-config --cflags --libs glib-2.0`
