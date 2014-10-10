#ifndef _TOKENIZE_H_
#define _TOKENIZE_H_

#include "tokenizer.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>

/* Tokenization functions */
void          t_set_stem_enabled      (int enabled);
TTokenizer*   t_tokenize_str          (char* str);
TTokenizer*   t_tokenize_wstr         (wchar_t* str);
TTokenizer*   t_tokenize_file         (char *fname);

#endif /* _TOKENIZE_H_ */
