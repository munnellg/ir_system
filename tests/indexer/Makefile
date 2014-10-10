all:
	gcc -g -Wall -pedantic indexer.c index.c tokenizer.c tokenize.c main.c $(pkg-config --cflags --libs glib-2.0) -o curator `pkg-config --cflags --libs glib-2.0`
