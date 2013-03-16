#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "Logger.h"
#include "WordDatabase.h"
#include "WordFrequencyAnalyzer.h"

WordDatabase* _worddb = 0;

enum PhoneType
{
	a_in_dab,
	a_in_air,
	a_in_far,
	a_in_day,
	a_in_ado,
	ir_in_tire,
	b_in_nab,
	ch_in_ouch,
	d_in_pod,
	e_in_red,
	e_in_see,
	f_in_elf,
	g_in_fig,
	h_in_had,
	w_in_white,
	i_in_hid,
	i_in_ice,
	g_in_vegetably,
	c_in_act,
	l_in_ail,
	m_in_aim,
	ng_in_bang,
	n_in_and,
	oi_in_oil,
	o_in_bob,
	ow_in_how,
	o_in_dog,
	o_in_boat,
	oo_in_too,
	oo_in_book,
	p_in_imp,
	r_in_ire,
	sh_in_she,
	s_in_sip,
	th_in_bath,
	th_in_the,
	t_in_tap,
	u_in_cup,
	u_in_burn,
	v_in_average,
	w_in_win,
	y_in_you,
	s_in_vision,
	z_in_zoo,
	a_in_ami,
	n_in_francoise,
	r_in_der,
	ch_in_bach,
	eu_in_bleu,
	u_in_duboise,
	wa_in_noire
};

struct PhoneSet_t
{
	PhoneType id;
	const char* display;
	char isSyllable;
};

PhoneSet_t PhoneSet[] = { { a_in_dab, "ae", 1 }, { a_in_air, "ey", 1 }, { a_in_far, "ao", 1 }, { a_in_day, "ay", 1 }, { a_in_ado, "ah", 1 }, { ir_in_tire, "ire", 1 }, { b_in_nab, "b", 0 }, {
		ch_in_ouch, "ch", 0 }, { d_in_pod, "d", 0 }, { e_in_red, "e", 1 }, { e_in_see, "ee", 1 }, { f_in_elf, "f", 0 }, { g_in_fig, "g", 0 }, { h_in_had, "h", 0 }, { w_in_white, "wh", 0 }, { i_in_hid,
		"i", 1 }, { i_in_ice, "eye", 1 }, { g_in_vegetably, "g", 0 }, { c_in_act, "k", 0 }, { l_in_ail, "l", 0 }, { m_in_aim, "m", 0 }, { ng_in_bang, "ng", 0 }, { n_in_and, "n", 0 }, { oi_in_oil,
		"oy", 1 }, { o_in_bob, "aa", 1 }, { ow_in_how, "ow", 1 }, { o_in_dog, "ah", 1 }, { o_in_boat, "oh", 1 }, { oo_in_too, "oo", 1 }, { oo_in_book, "ooh", 1 }, { p_in_imp, "p", 0 }, { r_in_ire,
		"er", 0 }, { sh_in_she, "sh", 0 }, { s_in_sip, "s", 0 }, { th_in_bath, "dth", 0 }, { th_in_the, "th", 0 }, { t_in_tap, "t", 0 }, { u_in_cup, "uh", 1 }, { u_in_burn, "u", 1 }, { v_in_average,
		"v", 0 }, { w_in_win, "w", 0 }, { y_in_you, "y", 0 }, { s_in_vision, "zh", 0 }, { z_in_zoo, "z", 0 }, { a_in_ami, "a", 1 }, { n_in_francoise, "n", 0 }, { r_in_der, "r", 0 }, { ch_in_bach,
		"chh", 0 }, { eu_in_bleu, "eu", 1 }, { u_in_duboise, "u", 1 }, { wa_in_noire, "WA", 1 } };

WordFilter::WordFilter()
{
	partOfSpeech = 0xffff;
	minSyllables = 1;
	maxSyllables = 0xffff;
	stresses[0] = 0;
}

bool Word::match(WordFilter* filter)
{
	return (partOfSpeech & filter->partOfSpeech) != 0 && numSyllables >= filter->minSyllables && numSyllables <= filter->maxSyllables && matchesStress(filter->stresses);
}

WordDatabase::WordDatabase(const char* filename) :
		_wordMap(100000), m_filename(filename), m_close(false)
{
	_worddb = this;

}

WordDatabase::~WordDatabase()
{

}

PartOfSpeech_t MobyToPartOfSpeech(char ch)
{
	switch (ch) {
	case 'N':
		return POS_Noun;
	case 'p':
		return POS_Plural;
	case 'h':
		return POS_NounPhrase;
	case 'V':
		return POS_VerbParticiple;
	case 't':
		return POS_VerbTransitive;
	case 'i':
		return POS_VerbIntrasitive;
	case 'A':
		return POS_Adjective;
	case 'v':
		return POS_Adverb;
	case 'C':
		return POS_Conjunction;
	case 'P':
		return POS_Preposition;
	case '!':
		return POS_Interjection;
	case 'r':
		return POS_Pronoun;
	case 'D':
		return POS_DefiniteArticle;
	case 'I':
		return POS_IndefiniteArticle;
	case 'o':
		return POS_Nominative;
	default:
		return POS_Unknown;
	}
}

static unsigned generateSyllables(const char* mobyText, unsigned short* pron, unsigned short maxSize, unsigned* pnumSyllables);

void WordDatabase::close()
{
	m_close = true;
}

void WordDatabase::run()
{
	// open file.
	char linebuff[100];
	FILE* file = fopen(m_filename, "rt");
	unsigned max = 0;

	// for each line,
	int count = 0;
	for (;;) {
		if (m_close) {
			break;
		}

		char* name;
		char* pos;
		char* pron;
		char* line;
		PartOfSpeech_t posType;

		line = fgets(linebuff, sizeof(linebuff), file);
		//LOGGER("Line:" << line);

		if (line == NULL) {
			break;
		}

		// tokenize. Should be 3 tokens.
		name = strtok(line, " \n\r");
		// create the word record (text is first token)
		if (name == NULL) {
			continue;
		}

		pos = strtok(NULL, " \n\r");
		if (pos == NULL) {
			continue;
		}

		pron = strtok(NULL, " \n\r");
		if (pron == NULL) {
			continue;
		}

		posType = MobyToPartOfSpeech(*pos);
		if (posType == POS_Unknown) {
			continue;
		}

		unsigned numSyllables = 0;
		unsigned short pronBuffer[32];
		unsigned numPron = generateSyllables(pron, pronBuffer, sizeof(pronBuffer) / sizeof(*pronBuffer) - 1, &numSyllables);
		if (numPron == 0) {
			continue;
		}

		if (numPron > max) {
			max = numPron;
			continue;
		}

		pronBuffer[numPron++] = 0xffff;

		Word* word = new Word;
		word->text = _strdup(name);
		word->partOfSpeech = posType;
		word->pron = (unsigned short*) malloc(numPron * sizeof(*pronBuffer));
		word->numSyllables = numSyllables;
		memcpy(word->pron, pronBuffer, numPron * sizeof(*pronBuffer));
		// print the record for debugging.
		//word->print();
		_wordMap.add(word->text, word);
		_wordArray.append(word);

		++count;
		emit progress(count);
	}

	LOGGER("MaxPhenomes: " << max);
	assert(max <= MAX_PHENOMES_IN_WORD);

	emit loadComplete();
	LOGGER( "words loaded:" << _wordArray.size());
}

void WordDatabase::addSynonym(Word* base, Word* synonym)
{
	LOGGER(base->text << "->" << synonym->text);
	base->synonyms.append(synonym);
}

bool WordDatabase::loadThesaurus(const char* thesaurus)
{
	FILE* file = fopen(thesaurus, "rt");
	if (file == 0) {
		LOGGER("Cannot load thesaurus:" << thesaurus);
		return false;
	}

	char linebuff[13 * 1024];

	for (;;) {

		char* line = fgets(linebuff, sizeof(linebuff), file);

		if (line == 0) {
			break;
		}

		char* token = strtok(line, ",\n");

		if (!token) {
			continue;
		}

		Word* root = lookup(token);
		if (!root) {
			continue;
		}

		token = strtok(NULL, ",\n");

		while (token) {
			Word* synonym = lookup(token);
			if (synonym) {
				addSynonym(root, synonym);
			}

			token = strtok(NULL, ",\n");
		}
	}

	LOGGER("Loaded thesaurus.");

	fclose(file);
	return true;
}

void Word::printPron()
{
	unsigned i = 0;
	while (pron[i] != 0xffff) {
		char buffer[24];
		pronToText(pron[i], buffer);
		LOGGER(buffer);
		i++;
	}
}

void Word::print()
{
	QString partOfSpeechValue = partOfSpeech == POS_Noun ? "Noun" : partOfSpeech == POS_Plural ? "Plural" : partOfSpeech == POS_NounPhrase ? "NounPhrase" :
								partOfSpeech == POS_VerbParticiple ? "Verb-Participle" : partOfSpeech == POS_VerbTransitive ? "Verb-Trasitive" :
								partOfSpeech == POS_VerbIntrasitive ? "Verb-Intransitive" : partOfSpeech == POS_Adjective ? "Adjective" : partOfSpeech == POS_Adverb ? "Adverb" :
								partOfSpeech == POS_Conjunction ? "Conjuction" : partOfSpeech == POS_Preposition ? "Preposition" : partOfSpeech == POS_Interjection ? "Interjection" :
								partOfSpeech == POS_Pronoun ? "Pronoun" : partOfSpeech == POS_DefiniteArticle ? "DefiniteArticle" : partOfSpeech == POS_IndefiniteArticle ? "IndefiniteArticle" :
								partOfSpeech == POS_Nominative ? "Nominative" : "unknown";
	LOGGER("WORD, syllables: " << text << numSyllables << partOfSpeechValue);

	printPron();
}

void Word::pronToText(unsigned short pr, char* buffer)
{
	const char* which = "unknown";
	unsigned syllable = pr >> 14;
	pr &= 0x3fff;

	if (pr < sizeof(PhoneSet) / sizeof(*PhoneSet)) {
		which = PhoneSet[pr].display;
	}

	if (syllable > 0) {
		*buffer = syllable + '0';
		buffer++;
	}

	strcpy(buffer, which);
	if (syllable == 2) {
		while (*buffer) {
			*buffer = toupper(*buffer);
			buffer++;
		}
	}
}

bool Word::matchesStress(const char* stresses)
{
	if (stresses[0]) {
		unsigned stress = 0;
		unsigned pos = 0;
		for (;;) {
			if (pron[pos] == 0xffff || stresses[stress] == 0) {
				return true;
			}

			unsigned syllable = pron[pos] >> 14;

			if (syllable > 0) {
				if ((syllable == 2 || syllable == 3) && stresses[stress] == '0') {
					return false;
				}
				stress++;
			}
			pos++;
		}
	}
	return true;
}

static unsigned generateSyllables(const char* mobyText, unsigned short* pron, unsigned short maxSize, unsigned* pnumSyllables)
{
#if 0
	const char* moby_phones[] = {
		"&", //sounds like the "a" in "dab" V 0
		"(@)",//sounds like the "a" in "air" V 1
		"-",//sounds like the "ir" glide in "tire" V 2
		"@",//sounds like the "a" in "ado", cup     V3
		"@r",//sounds like the "u" in "burn"         V 4
		"A",//sounds like the "a" in "ami", far, bob V5
		"AU",//sounds like the "ow" in "how"         V6
		"D",//sounds like the "th" in "the"       S7
		"E",//sounds like the "e" in "red"        V8
		"I",//sounds like the "i" in "hid"        S9
		"N",//sounds like the "n" in "Francoise", ng in bang.      S10
		"O",//sounds like the "o" in "dog"        V11
		"Oi",//sounds like the "oi" in "oil"       V12
		"R",//sounds like the "r" in "Der"        S13
		"S",//sounds like the "sh" in "she"       S14
		"T",//sounds like the "th" in "bath"      S15
		"U",//sounds like the "oo" in "book"      V16
		"Y"//sounds like the "u" in "Dubois"     V17
		"Z",//sounds like the "s" in "vision"     S18
		"aI",//sounds like the "i" in "ice"        V19
		"b",//sounds like the "b" in "nab"        S20
		"d",//sounds like the "d" in "pod"        S21
		"dZ",//sounds like the "g" in "vegetably"  S22
		"eI",//sounds like the "a" in "day"        V23
		"f",//sounds like the "f" in "elf"        S24
		"g",//sounds like the "g" in "fig"        S25
		"h",//sounds like the "h" in "had"        S26
		"hw",//sounds like the "w" in "white"      S27
		"i",//sounds like the "e" in "see"        V28
		"j",//sounds like the "y" in "you"        S29
		"k",//sounds like the "c" in "act"        S30
		"l",//sounds like the "l" in "ail"        S31
		"m",//sounds like the "m" in "aim"        S32
		"n",//sounds like the "n" in "and"        S33
		"oU",//sounds like the "o" in "boat"       V34
		"p",//sounds like the "p" in "imp"        S35
		"r",//sounds like the "r" in "ire"        S36
		"s",//sounds like the "s" in "sip"        S37
		"t",//sounds like the "t" in "tap"        S38
		"tS",//sounds like the "ch" in "ouch"      S39
		"u",//sounds like the "oo" in "too"       V40
		"v",//sounds like the "v" in "average"    V41
		"w",//sounds like the "w" in "win"        S42
		"x",//sounds like the "ch" in "Bach"      S43
		"y",//sounds like the "eu" in "cordon bleu"   V44
		"z",//sounds like the "z" in "zoo"            S45
	};
#endif

	enum State
	{
		start, got_lparen, got_lparen_at, got_a, got_at, got_A, got_O, got_d, got_e, got_h, got_o, got_t, got_W, end
	} state = start;

	struct StateMachine_t
	{
		State state;
		// if ch is '?' then regardless of character:
		//     - output the phenome id given in action.
		char ch;
		// 0 or positive number:
		//      Output a phenome id. Increment position.
		// -1:  Increment position.
		// -2:  Mark next syllable as strong. Increment position.
		// -3:  Mark next syllable as weak. Increment position.
		char action;
		State nextState;
	} stateMachine[] = { { start, '&', a_in_dab, start }, { start, '(', -1, got_lparen }, { start, '[', -1, got_lparen }, { start, '@', a_in_air, start }, { got_lparen, '@', -1, got_lparen_at }, {
			got_lparen_at, ')', a_in_air, start }, { got_lparen_at, ']', a_in_air, start }, { start, '-', ir_in_tire, start }, { start, '@', -1, got_at }, { got_at, 'r', u_in_burn, start }, { got_at,
			'?', a_in_ado, start }, { start, 'A', -1, got_A }, { got_A, 'U', ow_in_how, start }, { got_A, '?', a_in_far, start }, { start, 'D', th_in_the, start }, { start, 'E', e_in_red, start }, {
			start, 'I', i_in_hid, start }, { start, 'N', ng_in_bang, start }, { start, 'O', -1, got_O }, { got_O, 'i', oi_in_oil, start }, { got_O, '?', o_in_dog, start }, { start, 'R', r_in_der,
			start }, { start, 'S', sh_in_she, start }, { start, 'T', th_in_bath, start }, { start, 'U', oo_in_book, start }, { start, 'Y', u_in_duboise, start },
			{ start, 'V', v_in_average, start }, // french? not in readme,
			{ start, 'W', -1, got_W }, // not in readme
			{ got_W, 'A', wa_in_noire, start }, { start, 'Z', s_in_vision, start }, { start, 'b', b_in_nab, start }, { start, 'd', -1, got_d }, { got_d, 'Z', g_in_vegetably, start }, { got_d, '?',
					d_in_pod, start }, { start, 'a', -1, got_a }, { got_a, 'I', i_in_ice, start },
			{ got_a, '?', a_in_dab, start }, // due to errors in mobydict.
			{ start, 'e', -1, got_e }, { got_e, 'I', a_in_day, start }, { start, 'f', f_in_elf, start }, { start, 'g', g_in_fig, start }, { start, 'h', -1, got_h }, { got_h, 'w', w_in_white, start },
			{ got_h, '?', h_in_had, start }, { start, 'i', e_in_see, start }, { start, 'j', y_in_you, start }, { start, 'k', c_in_act, start }, { start, 'l', l_in_ail, start }, { start, 'm', m_in_aim,
					start }, { start, 'n', n_in_and, start }, { start, 'o', -1, got_o }, { got_o, 'U', o_in_boat, start }, { start, 'p', p_in_imp, start }, { start, 'r', r_in_ire, start }, { start,
					's', s_in_sip, start }, { start, 't', -1, got_t }, { got_t, 'S', ch_in_ouch, start }, { got_t, '?', t_in_tap, start }, { start, 'u', oo_in_too, start }, { start, 'v', v_in_average,
					start }, { start, 'w', w_in_win, start }, { start, 'x', ch_in_bach, start }, { start, 'y', eu_in_bleu, start }, { start, 'z', z_in_zoo, start }, { start, '/', -1, start }, { start,
					'_', -1, start }, { start, '\'', -2, start }, { start, ',', -3, start }, { start, 0, -1, end } };

#define BUFFER_SIZE 64
	const char* pos = mobyText;
	unsigned numPhones = 0;
	unsigned numSyllables = 0;
	unsigned syllableType = 1; // normal stress

	while (state != end) {
		// get next character.
		char ch = *pos;
		bool found = false;

		if (numPhones == maxSize) {
			assert(0);
			return 0;
		}

		//LOGGER("ch: " << ch << "state" << state);

		// search state machine for what to do.
		for (unsigned i = 0; i < sizeof(stateMachine) / sizeof(*stateMachine); i++) {
			if (stateMachine[i].state == state && (stateMachine[i].ch == ch || stateMachine[i].ch == '?')) {
				state = stateMachine[i].nextState;
				if (stateMachine[i].ch == '?') {
					pron[numPhones++] = stateMachine[i].action;
					if (PhoneSet[stateMachine[i].action].isSyllable) {
						pron[numPhones - 1] |= syllableType << 14;
						syllableType = 1;
						numSyllables++;
					}
				} else {
					if (stateMachine[i].action >= 0) {
						pron[numPhones++] = stateMachine[i].action;
						if (PhoneSet[stateMachine[i].action].isSyllable) {
							pron[numPhones - 1] |= syllableType << 14;
							syllableType = 1;
							numSyllables++;
						}
						pos++;
					} else if (stateMachine[i].action == -1) {
						pos++;
					} else if (stateMachine[i].action == -2) {
						// next syllable is strong.
						syllableType = 2;
						pos++;
					} else if (stateMachine[i].action == -3) {
						// next syllable is weak.
						syllableType = 3;
						pos++;
					}
				}
				found = true;
				break;
			}
		}

		if (!found) {
			// error.
			LOGGER("Couldn't parse" << pos << "in" << mobyText);
			return 0;
		}
	}

	*pnumSyllables = numSyllables;

	return numPhones;
}

unsigned getCommonSuffixLength(Word* word1, Word* word2)
{
	unsigned pos1 = 0;
	unsigned pos2 = 0;
	unsigned count = 0;

	while (word1->pron[pos1] != 0xffff) {
		pos1++;
	}

	while (word2->pron[pos2] != 0xffff) {
		pos2++;
	}

	while (pos1 > 0 && pos2 > 0 && word1->pron[pos1] == word2->pron[pos2]) {
		count++;
		pos1--;
		pos2--;
	}

	return count;
}

static Word* rootWord = 0;

int suffixCompare(const void* pfirst, const void* psecond)
{
	Word* first = *((Word**) pfirst);
	Word* second = *((Word**) psecond);
	unsigned count1 = getCommonSuffixLength(rootWord, first);
	unsigned count2 = getCommonSuffixLength(rootWord, second);
	if (count1 > count2) {
		return -1;
	} else if (count1 < count2) {
		return 1;
	} else {
		return 0;
	}
}

unsigned MinSuffixContainingSyllable(Word* word)
{
	unsigned pos = 0;
	unsigned lastSyllable = 0;
	while (word->pron[pos] != 0xffff) {
		if (word->pron[pos] >> 14) {
			lastSyllable = pos;
		}
		pos++;
	}

	return pos - lastSyllable;
}

unsigned CountPhenomes(Word* word)
{
	unsigned pos = 0;
	while (word->pron[pos] != 0xffff) {
		pos++;
	}

	return pos;
}

bool WordDatabase::findRhymes(DynamicArray<Word*>& results, const char* whichword, WordFilter* filter, WordArray* wordList)
{
	// look up word.
	Word* word = lookup(whichword);
	if (word == 0) {
		return false;
	}

	//word->print();

	if (wordList == 0) {
		wordList = &_wordArray;
	}

	unsigned minLength = MinSuffixContainingSyllable(word);

	// for each other word that is not equal to the word,
	for (unsigned i = 0; i < wordList->size(); i++) {
		if (word == (*wordList)[i]) {
			continue;
		}

		if (filter && !(*wordList)[i]->match(filter)) {
			continue;
		}

		// if they share any suffix,
		unsigned count = getCommonSuffixLength(word, (*wordList)[i]);
		if (count > minLength) {
			// add it to the results.
			results.append((*wordList)[i]);
		}
	}

	rootWord = word;
	if (results.size() > 0) {
		qsort(&results[0], results.size(), sizeof(Word*), suffixCompare);
	}

	return true;
}

Word*
WordDatabase::lookup(const char* whichword)
{
	Word* found = _wordMap.lookup(whichword);
	if (!found) {
		char* dup = _strdup(whichword);
		_strlwr(dup);
		found = _wordMap.lookup(dup);
		free(dup);
	}

	return found;
}

void WordDatabase::filter(WordArray& results, WordFilter* filter)
{
	for (unsigned i = 0; i < _wordArray.size(); i++) {
		if (_wordArray[i]->match(filter)) {
			results.append(_wordArray[i]);
		}
	}
}

void RemovePunctuation(char* text)
{
	// remove all characters except "'A-Za-z "
	unsigned i = 0;
	unsigned deleted = 0;
	for (;;) {

		if ((text[i] >= 'A' && text[i] <= 'Z') || (text[i] >= 'a' && text[i] <= 'z') || text[i] == '\'' || text[i] == ' ' || text[i] == 0) {
			if (deleted) {
				text[i - deleted] = text[i];
			}

		} else {
			deleted++;
		}

		if (text[i] == 0) {
			break;
		}

		i++;
	}
}

bool WordDatabase::makeWords(WordArray& results, const char* text)
{
	char* copy = _strdup(text);
	char* token;
	unsigned failedWords = 0;

	RemovePunctuation(copy);

	token = strtok(copy, " \t\r\n");
	while (token) {
		Word* word = lookup(token);
		if (word) {
			results.append(word);
		} else {
			LOGGER("Couldn't find word for %s\n" << token);
			failedWords++;
		}

		token = strtok(NULL, " \t\r\n");
	}

	return failedWords == 0;
}

unsigned WordDatabase::getSynonyms(WordArray& results, const char* wordtext)
{
	Word* word = lookup(wordtext);
	if (word == 0) {
		return 0;
	}

	for (unsigned i = 0; i < word->synonyms.size(); i++) {
		results.append(word->synonyms[i]);
	}

	return word->synonyms.size();
}

WordAssembler::WordAssembler()
{

}

WordAssembler::~WordAssembler()
{

}

void WordAssembler::addSolution(WordArray& phrase)
{
	Solution solution;
	solution.length = phrase.size();
	solution.words = new Word*[solution.length];
	memcpy(solution.words, &phrase[0], solution.length * sizeof(Word*));
	solution.score = 0;

	if (_wordfreq) {
		Word* previous = 0;
		for (unsigned i = 0; i < solution.length; i++) {
			Word* cur = solution.words[i];
			if (previous) {
				solution.score += _wordfreq->lookup(previous, cur);
			}

			previous = cur;
		}
	}

	if (concepts) {
		for (unsigned j = 0; j < solution.length; j++) {
			for (unsigned i = 0; i < concepts->size(); i++) {
				if ((*concepts)[i] == solution.words[j]) {
					solution.score += 32;
				}
			}
		}
	}

	if (solution.score == 0) {
		return;
	}

	if (0) {
		printf("%2d ", solution.score);
		for (int k = solution.length - 1; k >= 0; k--) {
			printf("%s ", solution.words[k]->text);
		}
		printf(" [");
		for (int k = solution.length - 1; k >= 0; k--) {
			solution.words[k]->printPron();
		}

		printf("]\n");
	}

	if ((solutions.size() % 10000) == 0) {
		LOGGER("Found %d solutions...." << solutions.size());
	}

	solutions.append(solution);
}

void WordAssembler::enumerateSolutions(unsigned depth, PartOfSpeech_t POS[], unsigned length, unsigned numSyllables, WordArray& phrase, const char* rhyme, const char* stresses)
{
	unsigned syllablesUsed = 0;
	if (stresses) {
		for (unsigned i = 0; i < phrase.size(); i++) {
			syllablesUsed += phrase[i]->numSyllables;
		}
	}

	// loop from N to 1.
	for (int i = numSyllables - 1; i >= 0; i--) {
		// for each n-syllable word matching that LAST part of speech that rhymes with the given
		// word, add to solution and recurse to find rest of solution.
		WordArray rhymes;
		WordArray* choices = &table[length - depth - 1][i];
		if (rhyme) {
			// a rhyme was specified. Narrow the choices to those that match the rhyme.
			_worddb->findRhymes(rhymes, rhyme, NULL, choices);
			choices = &rhymes;
		}

		for (unsigned j = 0; j < choices->size(); j++) {

			if (stresses) {
				if (!(*choices)[j]->matchesStress(&stresses[numSyllables - syllablesUsed - (*choices)[j]->numSyllables])) {
					continue;
				}
			}

			phrase.append((*choices)[j]);

			// if it is not possible to recurse then just output the phrase.
			if (length - depth - 1 == 0 && numSyllables - syllablesUsed == (*choices)[j]->numSyllables) {
				addSolution(phrase);
			} else if (length - depth - 1 > 0 && (int) numSyllables - syllablesUsed - (int) (*choices)[j]->numSyllables > 0) {
				enumerateSolutions(depth + 1, POS, length, numSyllables, phrase, 0, stresses);
			}

			phrase.resize(phrase.size() - 1);
		}
	}
}

int WordAssembler::solutionCompare(const void* pfirst, const void* psecond)
{
	Solution* first = (Solution*) pfirst;
	Solution* second = (Solution*) psecond;
	if (first->score > second->score) {
		return -1;
	} else if (first->score < second->score) {
		return 1;
	} else {
		return 0;
	}
}

void WordAssembler::run(PartOfSpeech_t POS[], unsigned length, unsigned numSyllables, const char* rhymesWith, const char* stresses, WordArray* concepts)
{
	// make a 2-D table of results that is length x numSyllables large.
	table = new WordArray*[length];
	for (unsigned i = 0; i < length; i++) {
		table[i] = new WordArray[numSyllables];
	}

	WordFilter filter;
	WordArray results;

	this->concepts = concepts;

	// for each part of speech given,
	for (unsigned i = 0; i < length; i++) {
		// find all words that are between 1 and numSyllables long.
		filter.maxSyllables = numSyllables;
		filter.partOfSpeech = POS[i];

		_worddb->filter(results, &filter);

		// separate them and store into their correct location in the table.
		for (unsigned j = 0; j < results.size(); j++) {
			table[i][results[j]->numSyllables - 1].append(results[j]);
		}

		results.resize(0);
	}

	if (0) {
		for (unsigned i = 0; i < length; i++) {
			for (unsigned j = 0; j < numSyllables; j++) {
				LOGGER("-- POS:" << POS[i] << "length: " << j+1);
				for (unsigned k = 0; k < table[i][j].size(); k++) {
					table[i][j][k]->print();
				}
			}
		}
	}

	WordArray phrase;

	// okay now we can enumerate solutions.
	enumerateSolutions(0, POS, length, numSyllables, phrase, rhymesWith, stresses);

	// destroy the table of results.
	for (unsigned i = 0; i < length; i++) {
		delete[] table[i];
	}

	delete[] table;

	LOGGER("Sorting...");

	if (solutions.size() > 0) {
		qsort(&solutions[0], solutions.size(), sizeof(Solution), solutionCompare);
	}

	// output the phrase.
	for (unsigned i = 0; i < solutions.size(); i++) {
		printf("%2d ", solutions[i].score);
		for (int k = solutions[i].length - 1; k >= 0; k--) {
			printf("%s ", solutions[i].words[k]->text);
		}
		printf(" [");
		for (int k = solutions[i].length - 1; k >= 0; k--) {
			solutions[i].words[k]->printPron();
		}

		printf("]\n");
	}
}
