all:
	gcc -g -Wall -pedantic src/index.c src/tokenizer.c src/stem.c src/main.c -o ir `pkg-config --cflags --libs glib-2.0`