#include <stdio.h>
#include <string.h>

#include "Logger.h"
#include "WordFrequencyAnalyzer.h"

WordFrequencyAnalyzer* _wordfreq = 0;

WordFrequencyAnalyzer::WordFrequencyAnalyzer() {

}

WordFrequencyAnalyzer::~WordFrequencyAnalyzer() {
	// note: memory leak.
}

unsigned WordFrequencyAnalyzer::lookup(Word* word1, Word* word2)
{
	unsigned score = 0;
	WordMap::iterator wordIter = _wordMap.find(word1);
	if (wordIter != _wordMap.end()) {
		score++; // one point just for being there.
		FreqMap* freqMap = wordIter->second;
		FreqMap::iterator freqIter = freqMap->find(word2);
		if (freqIter != freqMap->end()) {
			score += freqIter->second;
		}
	}

	return score;
}

void WordFrequencyAnalyzer::insert(Word* word1, Word* word2)
{
	WordMap::iterator wordIter = _wordMap.find(word1);
	FreqMap* freqMap;

	if (wordIter == _wordMap.end()) {
		freqMap = new FreqMap;
		_wordMap.insert(WordMap::value_type(word1, freqMap));
	} else {
		freqMap = wordIter->second;
	}

	FreqMap::iterator freqIter = freqMap->find(word2);
	if (freqIter == freqMap->end()) {
		freqMap->insert(FreqMap::value_type(word2, 1));
	} else {
		freqIter->second++;
	}
}

void WordFrequencyAnalyzer::print()
{
	WordMap::iterator wordIter = _wordMap.begin();
	while (wordIter != _wordMap.end()) {

		FreqMap::iterator freqIter = wordIter->second->begin();
		while (freqIter != wordIter->second->end()) {
			printf(" %10d %32s %32s\n", freqIter->second, wordIter->first->text,
					freqIter->first->text);
			freqIter++;
		}

		wordIter++;
	}
}

void WordFrequencyAnalyzer::analyzeFile(const char* filename)
{
	// open file.
	FILE* file = fopen(filename, "rt");
	if (file == 0) {
		fprintf(stderr, "Couldn't open %s\n", filename );
		return;
	}

		// for each line in the file,
	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), file)) {
		// remove punctuation
		RemovePunctuation(buffer);

		// tokenize.
		char* token = strtok(buffer, " \t");
		Word* previousWord = 0;

		// for each word,
		while (token) {
			Word* word = _worddb->lookup(token);
			// if the word is known, and there was a previous word,
			if (word && previousWord) {
				// add a record for the pair.
				insert(previousWord, word);
			}

			// set previous word to current word.
			previousWord = word;

			token = strtok(NULL, " \t");
		}
	}

	// close file.
	fclose(file);
}
