#include "stem.h"


static int ends_with(char *word, char* substr) {
	int len_word, len_sub, offset;
	int i;

	len_word = strlen(word);
	len_sub = strlen(substr);
	offset = len_word - len_sub;
	
	if(offset < 0) {
		return 0;
	}

	for(i=0; i<len_sub; i++) {
		if( tolower(word[i+offset]) != tolower(substr[i])) {
			return 0;	
		}
	}

	return 1;
}

/* Checks if the character at index idx is a consonant. Here, the
 * definition of a consonant is a letter other than A, E, I, O, U and
 * other than Y preceded by a consonant */
static int is_consonant(char *word, int idx) {
	const static char vowels[] = "aeiou";
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
	for( i=0; i<strlen(vowels); i++ ) {
		if( tolower(word[idx]) == vowels[i] ) {
			return 0;
		}
	}

	/* Check Y, our special case character */
	if( tolower(word[idx]) == 'y' && is_consonant(word, --idx) ) {
		return 0;
	}
	
	return 1;
}

/* Computes the measure of a string of characters. If we denote a
 * vowel to be v, a consonant to be c, a list of consecutive vowels to
 * be V and a list of consecutive consonants to be C, then the measure
 * m of a string is computed as [C](VC)^(m)[V] */
static int compute_measure(char* word, int len) {
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

static int contains_vowel(char *word, int len) {	
	int i;

	for(i=0; i<len; i++) {
		if(is_consonant(word, i) == 0) {
			return 1;
		}
	}

	return 0;
}

static int ends_double_consonant(char *word, int len) {

	/* Word must contain at least two characters in order to pass this
	 * test */
	if(len < 2) {
		return 0;
	}

	/* Run the test */
	return (tolower(word[len-1]) == tolower(word[len-2]) &&
					is_consonant(word, len-1) &&
					is_consonant(word, len-2));
}

static int ends_consonant_vowel_consonant(char *word, int len) {
	int i;
	static const char exclude[] = "wxy";

	/* Word must contain at least three characters in order to pass this
	 * test */
	if(len < 3) {
		return 0;
	}

	/* Check if the last character in the string is on our exclusion
	 * list. If so, then it will fail this test */
	for(i=0; i<strlen(exclude); i++) {
		if(word[len-1] == exclude[i]) {
			return 0;
		}
	}

	/* Run the test and return the result */
	return (is_consonant(word, len-3) &&
					!is_consonant(word, len-2) &&
					is_consonant(word, len-1));
}

static void step1a(char *word) {
	int len;
	
	len = strlen(word);

	if(ends_with(word, "SSES")) {
		word[len-2] = '\0';
	} else if(ends_with(word, "IES")) {
		word[len-2] = '\0';
	} else if(!ends_with(word, "SS") && ends_with(word, "S")) {
		word[len-1] = '\0';
	}
}

static void step1b(char *word) {
	int len;

	len = strlen(word);

	if(ends_with(word, "EED") && compute_measure(word, len-4) > 0) {
		word[len-1] = '\0';
		return;
	} else if(ends_with(word, "ED") && contains_vowel(word, len-3) ) {
		len = len-2;
		word[len] = '\0';
	} else if(ends_with(word, "ING") && contains_vowel(word, len-4)) {
		len = len-3;
		word[len] = '\0';
	} else {
		return;
	}

	if(ends_with(word, "AT")) {
		word[len] = 'e';
		word[len+1] = '\0';
	} else if(ends_with(word, "BL")) {
		word[len] = 'e';
		word[len+1] = '\0';
	} else if(ends_with(word, "IZ")) {
		word[len] = 'e';
		word[len+1] = '\0';
	} else if(ends_double_consonant(word, len) &&
						tolower(word[len-1]) != 'l' &&
						tolower(word[len-1]) != 's' &&
						tolower(word[len-1]) != 'z') {
		word[len-1] = '\0';
	} else if(compute_measure(word, len) == 1 && ends_consonant_vowel_consonant(word, len)) {
		word[len] = 'e';
		word[len+1] = '\0';
	}
}

static void step1c(char *word) {
	int len;
	len = strlen(word);
	
	if(ends_with(word, "Y") && contains_vowel(word, len-2) > 0) {
		word[len-1] = 'i';
	}
}

static void step2(char *word) {
	int len;
	len = strlen(word);
	
	if(ends_with(word, "ATIONAL") && compute_measure(word, len-7) > 0) {
		strcpy(word+len-7, "ate\0");
	} else	if(ends_with(word, "TIONAL") && compute_measure(word, len-6) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, "ENCI") && compute_measure(word, len-4) > 0) {
		strcpy(word+len-1, "e\0");
	} else if(ends_with(word, "ANCI") && compute_measure(word, len-4) > 0) {
		strcpy(word+len-1, "e\0");
	} else if(ends_with(word, "IZER") && compute_measure(word, len-4) > 0) {
		word[len-1] = '\0';
	} else if(ends_with(word, "ABLI") && compute_measure(word, len-4) > 0) {
		strcpy(word+len-1, "e\0");
	} else if(ends_with(word, "ALLI") && compute_measure(word, len-4) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, "ENTLI") && compute_measure(word, len-5) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, "ELI") && compute_measure(word, len-3) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, "OUSLI") && compute_measure(word, len-5) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, "IZATION") && compute_measure(word, len-7) > 0) {
		strcpy(word+len-7, "ize\0");
	} else if(ends_with(word, "ATION") && compute_measure(word, len-5) > 0) {
		strcpy(word+len-5, "ate\0");
	} else if(ends_with(word, "ATOR") && compute_measure(word, len-4) > 0) {
		strcpy(word+len-4, "ate\0");
	} else if(ends_with(word, "ALISM") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, "IVENESS") && compute_measure(word, len-7) > 0) {
		word[len-4] = '\0';
	} else if(ends_with(word, "FULNESS") && compute_measure(word, len-7) > 0) {
		word[len-4] = '\0';
	} else if(ends_with(word, "OUSNESS") && compute_measure(word, len-7) > 0) {
		word[len-4] = '\0';
	} else if(ends_with(word, "ALITI") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, "IVITI") && compute_measure(word, len-5) > 0) {
		strcpy(word+len-3, "e\0");
	} else if(ends_with(word, "BILITI") && compute_measure(word, len-6) > 0) {
		strcpy(word+len-6, "ble\0");
	}
}

static void step3(char *word) {
	int len;
	len = strlen(word);
	
	if(ends_with(word, "ICATE") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else	if(ends_with(word, "ATIVE") && compute_measure(word, len-5) > 0) {
		word[len-5] = '\0';
	} else if(ends_with(word, "ALIZE") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, "ICITI") && compute_measure(word, len-5) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, "ICAL") && compute_measure(word, len-4) > 0) {
		word[len-2] = '\0';
	} else if(ends_with(word, "FUL") && compute_measure(word, len-3) > 0) {
		word[len-3] = '\0';
	} else if(ends_with(word, "NESS") && compute_measure(word, len-4) > 0) {
		word[len-4] = '\0';
	}
}

static void step4(char *word) {
	int len;
	len = strlen(word);
	
	if(ends_with(word, "AL") && compute_measure(word, len-2) > 1) {
		word[len-2] = '\0';
	} else	if(ends_with(word, "ANCE") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, "ENCE") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, "ER") && compute_measure(word, len-2) > 1) {
		word[len-2] = '\0';
	} else if(ends_with(word, "IC") && compute_measure(word, len-2) > 1) {
		word[len-2] = '\0';
	} else if(ends_with(word, "ABLE") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, "IBLE") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, "ANT") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, "EMENT") && compute_measure(word, len-5) > 1) {
		word[len-5] = '\0';
	} else if(ends_with(word, "MENT") && compute_measure(word, len-4) > 1) {
		word[len-4] = '\0';
	} else if(ends_with(word, "ENT") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if( (ends_with(word, "SION") || ends_with(word, "TION")) && compute_measure(word, len-4) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, "OU") && compute_measure(word, len-2) > 1) {
		word[len-2] = '\0';
	} else if(ends_with(word, "ISM") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, "ATE") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, "ITI") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, "OUS") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, "IVE") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	} else if(ends_with(word, "IZE") && compute_measure(word, len-3) > 1) {
		word[len-3] = '\0';
	}
}

static void step5(char *word) {
	int len;
	len = strlen(word);
	
	if(ends_with(word, "E") && compute_measure(word, len-1) > 1) {
		word[len-1] = '\0';
	} else if(ends_with(word, "E") && compute_measure(word, len-1) == 1 && !ends_consonant_vowel_consonant(word, len-1)) {
		word[len-1] = '\0';
	}

	if(ends_with(word, "L") && compute_measure(word, len) > 1 && ends_double_consonant(word, len)) {
		word[len-1] = '\0';
	}
}

char* s_stem(char *word) {
	char* copy;

	copy = malloc(sizeof(char) * strlen(word));
	strcpy(copy, word);	
	
	step1a(copy);
	step1b(copy);
	step1c(copy);
	step2(copy);
	step3(copy);
	step4(copy);
	step5(copy);
	return copy;
}
