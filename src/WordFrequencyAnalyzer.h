#ifndef WordFrequencyAnalyzer_H
#define WordFrequencyAnalyzer_H

#include "WordDatabase.h"
#include <map>

class WordFrequencyAnalyzer
{
public:
    WordFrequencyAnalyzer();
    ~WordFrequencyAnalyzer();

    void analyzeFile( const char* filename );
    void print();
    unsigned lookup( Word* word1, Word* word2 );

private:
    void insert( Word* word1, Word* word2 );
    typedef std::map<Word*, unsigned> FreqMap;
    typedef std::map<Word*, FreqMap*> WordMap;
    WordMap _wordMap;
};

extern WordFrequencyAnalyzer* _wordfreq;

#endif

