#include "stem.h"
#include <wchar.h>
#include <wctype.h>

const static wchar_t vowels[] = L"aeiou";

static int
ends_with(wchar_t *word, wchar_t* substr) {
	int len_word, len_sub, offset;
	int i;

	len_word = wcslen(word);
	len_sub = wcslen(substr);
	offset = len_word - len_sub;
	
	if(offset < 0) {
		return 0;
	}

	for(i=0; i<len_sub; i++) {
		if( towlower(word[i+offset]) != towlower(substr[i])) {
			return 0;	
		}
	}

	return 1;
}

/* Checks if the character at index idx is a consonant. Here, the
 * definition of a consonant is a letter other than A, E, I, O, U and
 * other than Y preceded by a consonant */
static int
is_consonant(wchar_t *word, int idx) {
	int i;

	if(idx < 0) {
		return 0;
	}
	
	/* Make sure we haven't been passed a character that is not a
	 * letter*/
	if(!isalpha(word[idx])) {
		return -1;
	}

	/* First check the non-conditional vowels */
	for( i=0; i<wcslen(vowels); i++ ) {
		if( towlower(word[idx]) == vowels[i] ) {
			return 0;
		}
	}

	/* Check Y, our special case character */
	if( towlower(word[idx]) == 'y' && is_consonant(word, --idx) ) {
		return 0;
	}
	
	return 1;
}

/* Computes the measure of a string of characters. If we denote a
 * vowel to be v, a consonant to be c, a list of consecutive vowels to
 * be V and a list of consecutive consonants to be C, then the measure
 * m of a string is computed as [C](VC)^(m)[V] */
static int
compute_measure(wchar_t* word, int len) {
	int measure, state, is_vowel;
	int i;

	measure = 0;
	
	/* state is 1 for vowel and 0 for consonant */
	state = 0;
	
	for(i=0; i<len; i++) {
		/* Bit of error checking. Just ensure that we've not been passed
		 * sentence rather than a word */
		is_vowel = !is_consonant(word, i);
		if(is_vowel < 0) {
			return -1;
		}

		if( is_vowel ) {
			state = 1;
		} else {
			if(state == 1) {
				measure++;
			}
			state = 0;
		}
	}
	return measure;
}

static int
contains_vowel(wchar_t *word, int len) {	
	int i;

	for(i=0; i<len; i++) {
		if(is_consonant(word, i) == 0) {
			return 1;
		}
	}

	return 0;
}

static int
ends_double_consonant(wchar_t *word, int len) {

	/* Word must contain at least two characters in order to pass this
	 * test */
	if(len < 2) {
		return 0;
	}

	/* Run the test */
	return (towlower(word[len-1]) == towlower(word[len-2]) &&
					is_consonant(word, len-1) &&
					is_consonant(word, len-2));
}

static int
ends_consonant_vowel_consonant(wchar_t *word, int len) {
	int i;
	static const wchar_t exclude[] = L"wxy";

	/* Word must contain at least three characters in order to pass this
	 * test */
	if(len < 3) {
		return 0;
	}

	/* Check if the last character in the string is on our exclusion
	 * list. If so, then it will fail this test */
	for(i=0; i<wcslen(exclude); i++) {
		if(word[len-1] == exclude[i]) {
			return 0;
		}
	}

	/* Run the test and return the result */
	return (is_consonant(word, len-3) &&
					!is_consonant(word, len-2) &&
					is_consonant(word, len-1));
}

static void
step1a(wchar_t *word) {
	int len;
	
	len = wcslen(word);

	if(ends_with(word, L"SSES")) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"IES")) {
		word[len-2] = '\0';
	} else if(!ends_with(word, L"SS") && ends_with(word, L"S")) {
		word[len-1] = '\0';
	}
}

static void
step1b(wchar_t *word) {
	int len;

	len = wcslen(word);

	if(ends_with(word, L"EED") && compute_measure(word, len-4) > 0) {
		word[len-1] = '\0';
		return;
	} else if(ends_with(word, L"ED") && contains_vowel(word, len-3) ) {
		len = len-2;
		word[len] = '\0';
	} else if(ends_with(word, L"ING") && contains_vowel(word, len-4)) {
		len = len-3;
		word[len] = '\0';
	} else {
		return;
	}

	if(ends_with(word, L"AT")) {
		word[len] = 'e';
		word[len+1] = '\0';
	} else if(ends_with(word, L"BL")) {
		word[len] = 'e';
		word[len+1] = '\0';
	} else if(ends_with(word, L"IZ")) {
		word[len] = 'e';
		word[len+1] = '\0';
	} else if(ends_double_consonant(word, len) &&
						towlower(word[len-1]) != 'l' &&
						towlower(word[len-1]) != 's' &&
						towlower(word[len-1]) != 'z') {
		word[len-1] = '\0';
	} else if(compute_measure(word, len) == 1 && ends_consonant_vowel_consonant(word, len)) {
		word[len] = 'e';
		word[len+1] = '\0';
	}
}

static void
step1c(wchar_t *word) {
	int len;
	len = wcslen(word);
	
	if(ends_with(word, L"Y") && contains_vowel(word, len-2) > 0) {
		word[len-1] = 'i';
	}
}

static void
step2(wchar_t *word) {
	int len;
	len = wcslen(word);
	
	if(ends_with(word, L"ATIONAL") && compute_measure(word, len-7) > 0) {
		wcscpy(word+len-7, L"ate\0");
	} else	if(ends_with(word, L"TIONAL") && compute_measure(word, len-6) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"ENCI") && compute_measure(word, len-4) > 0) {
		wcscpy(word+len-1, L"e\0");
	} else if(ends_with(word, L"ANCI") && compute_measure(word, len-4) > 0) {
		wcscpy(word+len-1, L"e\0");
	} else if(ends_with(word, L"IZER") && compute_measure(word, len-4) > 0) {
		word[len-1] = '\0';
	} else if(ends_with(word, L"ABLI") && compute_measure(word, len-4) > 0) {
		wcscpy(word+len-1, L"e\0");
	} else if(ends_with(word, L"ALLI") && compute_measure(word, len-4) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"ENTLI") && compute_measure(word, len-5) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"ELI") && compute_measure(word, len-3) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"OUSLI") && compute_measure(word, len-5) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"IZATION") && compute_measure(word, len-7) > 0) {
		wcscpy(word+len-7, L"ize\0");
	} else if(ends_with(word, L"ATION") && compute_measure(word, len-5) > 0) {
		wcscpy(word+len-5, L"ate\0");
	} else if(ends_with(word, L"ATOR") && compute_measure(word, len-4) > 0) {
		wcscpy(word+len-4, L"ate\0");
	} else if(ends_with(word, L"ALISM") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"IVENESS") && compute_measure(word, len-7) > 0) {
		word[len-4] = '\0';
	} else if(ends_with(word, L"FULNESS") && compute_measure(word, len-7) > 0) {
		word[len-4] = '\0';
	} else if(ends_with(word, L"OUSNESS") && compute_measure(word, len-7) > 0) {
		word[len-4] = '\0';
	} else if(ends_with(word, L"ALITI") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"IVITI") && compute_measure(word, len-5) > 0) {
		wcscpy(word+len-3, L"e\0");
	} else if(ends_with(word, L"BILITI") && compute_measure(word, len-6) > 0) {
		wcscpy(word+len-6, L"ble\0");
	}
}

static void
step3(wchar_t *word) {
	int len;
	len = wcslen(word);
	
	if(ends_with(word, L"ICATE") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else	if(ends_with(word, L"ATIVE") && compute_measure(word, len-5) > 0) {
		word[len-5] = '\0';
	} else if(ends_with(word, L"ALIZE") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"ICITI") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"ICAL") && compute_measure(word, len-4) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"FUL") && compute_measure(word, len-3) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"NESS") && compute_measure(word, len-4) > 0) {
		word[len-4] = '\0';
	}
}

static void
step4(wchar_t *word) {
	int len;
	len = wcslen(word);
	
	if(ends_with(word, L"AL") && compute_measure(word, len-2) > 1) {
		word[len-2] = '\0';
	} else	if(ends_with(word, L"ANCE") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, L"ENCE") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, L"ER") && compute_measure(word, len-2) > 1) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"IC") && compute_measure(word, len-2) > 1) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"ABLE") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, L"IBLE") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, L"ANT") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"EMENT") && compute_measure(word, len-5) > 1) {
		word[len-5] = '\0';
	} else if(ends_with(word, L"MENT") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, L"ENT") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if( (ends_with(word, L"SION") || ends_with(word, L"TION")) && compute_measure(word, len-4) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"OU") && compute_measure(word, len-2) > 1) {
		word[len-2] = '\0';
	} else if(ends_with(word, L"ISM") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"ATE") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"ITI") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"OUS") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"IVE") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, L"IZE") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	}
}

static void
step5(wchar_t *word) {
	int len;
	len = wcslen(word);
	
	if(ends_with(word, L"E") && compute_measure(word, len-1) > 1) {
		word[len-1] = '\0';
	} else if(ends_with(word, L"E") && compute_measure(word, len-1) == 1 && !ends_consonant_vowel_consonant(word, len-1)) {
		word[len-1] = '\0';
	}

	if(ends_with(word, L"L") && compute_measure(word, len) > 1 && ends_double_consonant(word, len)) {
		word[len-1] = '\0';
	}
}

wchar_t*
s_stem(wchar_t *word) {
	wchar_t* copy;

	copy = malloc(sizeof(*copy) * (wcslen(word)+1));
	wcscpy(copy, word);	
	
	step1a(copy);
	step1b(copy);
	step1c(copy);
	step2(copy);
	step3(copy);
	step4(copy);
	step5(copy);
	
	return copy;
}
