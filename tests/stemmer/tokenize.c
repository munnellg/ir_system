#include "tokenize.h"
#include "stem.h"
#include <locale.h>

#define DEFAULT_BUFFER_SIZE 12  /* Initiale buffer size */
#define ENVIRONMENT_LOCALE  ""  /* Locale in which tokenizer works */

static TToken  *g_token;        /** The token being processed */
static wchar_t *g_buffer;       /** Buffer for constructing tokens */
static int      g_buf_capacity; /** Current buffer capacity */
static int      g_scanning;     /** 1 if scanning for token start */

static int      g_stem_enabled = 0;

static void
wstr_to_lower(wchar_t *str) {
	int i;

	for(i=0; str[i]!='\0'; i++) {
		str[i] = towlower(str[i]);
	}
}

/**
 * Converts a wchar_t* string into a char* string.
 */
static char *
wstr_to_str(wchar_t *str) {
	char   *buf;
	size_t  len;

	/* Initialize the buffer to some value */
	buf = NULL;

	/* Ensure that str is valid */
	if(str) {
		/* Get length of buffer required to store char string */
		len = wcslen(str)*sizeof(str);

		/* Allocate memory and perform conversion */
		buf = malloc(sizeof(*buf) * len);
		wcstombs(buf, str, len);
	}

	/* Return character string */
	return buf;
}

/**
 * Converts a char* string into a wchar_t* string
 */
static wchar_t *
str_to_wstr(char *str) {
	wchar_t *buf;
	size_t   len;

	/* Initialize the buffer to some value */
	buf = NULL;

	/* Ensure that str is valid */
	if(str) {
		/* Get the length of the buffer needed for wchar_t string */
		len = strlen(str)+1;

		/* Allocate memory and perform conversion */
		buf = malloc(sizeof(*buf) * len);
		mbstowcs(buf, str, len);
	}

	/* Return the character string */
	return buf;
}

/**
 * Set a character in the buffer. Expands the buffer if there isn't
 * enough allocated memory to access the index.
 */
static void
set_buf_char_at(wchar_t wchar, int idx) {
	/* Check if the index is within range */
	if ( g_buf_capacity <= idx+1) {
		/* If index is out of range, double capacity and reallocate
		 * memory*/
		g_buf_capacity *= 2;
		g_buffer = realloc(g_buffer, sizeof(*g_buffer) * g_buf_capacity);
	}

	/* Set the character */
	g_buffer[idx] = wchar;
}

/**
 * Process an individual token character. Must first determine if c is
 * a valid token character. If so, append it to the buffer. Otherwise
 * generate a new token from the buffer and push it to the tokenizer's
 * list of tokens.
 */
static void
process_char(TTokenizer *tokenizer, wchar_t c) {
	wchar_t *tmp;
	
	/* Test if c is a valid token character */
	if( iswalpha(c) ) {
		/* If we were scanning for the first token character, then create
		 * a new token and begin parsing token */
		if(g_scanning) {
			g_token = t_token_new(NULL, tokenizer->chars_scanned, tokenizer->tokens_scanned++);
			g_scanning = 0;
		}

		/* Add the character to the buffer */
		set_buf_char_at(c, tokenizer->chars_scanned - g_token->char_pos);
	} else if( !g_scanning ) {
		/* If character isn't valid but we're parsing, then push token */

		/* Terminate string and push token to tokenizer */
		set_buf_char_at('\0', tokenizer->chars_scanned - g_token->char_pos);
		wstr_to_lower(g_buffer);
		if(g_stem_enabled) {
			tmp = s_stem(g_buffer);
			wcscpy(g_buffer, tmp);
			free(tmp);
		}
		g_token->text = wstr_to_str(g_buffer);
		tokenizer->tokens = g_list_prepend(tokenizer->tokens, g_token);

		/* Stop parsing and begin scanning */
		g_scanning = 1;
	}

	/* Keep track of characters processed */
	tokenizer->chars_scanned++;
}

/**
 * Set up the environment variables required to analyse string, then
 * execute the appropriate parsing function.
 */
static TTokenizer *
execute_in_environment(TTokenizer* (*func)(void *), void *args, char *locale) {
	TTokenizer *tkzr;
	char *old_locale, *saved_locale;

	/* Set the locale */
	old_locale = setlocale (LC_ALL, NULL);
	saved_locale = strdup (old_locale);
	setlocale (LC_ALL, locale);

	/* Allocate memory to the buffer */
	g_buf_capacity = DEFAULT_BUFFER_SIZE;
	g_buffer = malloc(sizeof(*g_buffer) * g_buf_capacity);

	/* Begin in scan mode */
	g_scanning = 1;
	
	/* Execute the function and store the result */
	tkzr = func(args);

	/* Free the buffer */
	free(g_buffer);
	g_buf_capacity = 0;

	/* Reset the locale */
	setlocale (LC_ALL, saved_locale);
	free (saved_locale);

	/* Return the result */
	return tkzr;
}

/**
 * Decompose a string of type wchar_t into tokens.
 */
static TTokenizer *
tokenize_wide_string(void *arg) {
	TTokenizer *tkzr;
	int i;
	wchar_t *str, c;

	/* Cast the argument to the required type */
	str = (wchar_t*) arg;

	/* Create a new tokenizer */
	tkzr = t_tokenizer_new();

	/* Process each character in the string using the tokenizer */
	for( i=0; i<=wcslen(str); i++) {
		c = str[i];
		process_char(tkzr, c);
	}

	/* Return the result */
	return tkzr;
}

/**
 * Decompose a string of type char into tokens.
 */
static TTokenizer *
tokenize_string(void *arg) {
	wchar_t *buf;
	TTokenizer *result;
	char * str;

	/* Cast the argument to the appropriate type */
	str = (char *) arg;

	/* Just convert to a wchar_t string. No need for two string parsing
	 * functions */
	buf = str_to_wstr(str);

	/* Process */
	result = tokenize_wide_string(buf);

	/* Free memory (and maybe Tibet)*/
	free(buf);

	/* Return the result */
	return result;
}

void
t_set_stem_enabled(int enabled) {
	g_stem_enabled = enabled;
}

/**
 * Decompose the contents of a file into tokens
 */
static TTokenizer *
tokenize_file(void *arg) {
	FILE    *f;
	wchar_t c;
	char *fname;
	TTokenizer *tkzr;

	/* Cast the argument to a string */
	fname = (char*) arg;

	/* Create a new tokenizer */
	tkzr = t_tokenizer_new();

	/* Open the file */
	f = fopen(fname, "r ccs=UTF-8");	
	if(f) {
		/* Process each character of the file */
		while( ( c = fgetwc(f) ) != WEOF ) {
			process_char(tkzr, c);
		}
		/* Close the file */
		fclose(f);
	}

	/* Return the result */
	return tkzr;
}

/**
 * Tokenize a string. Acts as an interface to the tokenize_str function.
 */
TTokenizer *
t_tokenize_str(char *str) {
	/* Perform the tokenization in a suitable environment */
	return execute_in_environment(&tokenize_string, str, ENVIRONMENT_LOCALE);
}

/**
 * Tokenize a string. Acts as an interface to the tokenize_wstr function.
 */
TTokenizer *
t_tokenize_wstr(wchar_t* str) {
	/* Perform the tokenization in a suitable environment */
	return execute_in_environment(&tokenize_wide_string, str, ENVIRONMENT_LOCALE);
}

/**
 * Tokenize a file. Acts as an interface to the tokenize_file function
 */
TTokenizer *
t_tokenize_file(char *fname) {
	/* Perform the tokenization in a suitable environment */
	return execute_in_environment(&tokenize_file, fname, ENVIRONMENT_LOCALE);
}
