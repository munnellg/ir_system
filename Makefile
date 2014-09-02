all:
	gcc -g -Wall -pedantic src/resources.c src/irapp.c src/irappwin.c src/index.c src/tokenizer.c src/stem.c src/main.c $(pkg-config --cflags --libs glib-2.0 gtk+-3.0) -o ir_system `pkg-config --cflags --libs glib-2.0 gtk+-3.0`
