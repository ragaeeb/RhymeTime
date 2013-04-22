#ifndef WordDatabase_H
#define WordDatabase_H

#include "DynamicArray.h"
#include "StringMap.h"
#include <QObject>
#include <QRunnable>

enum PartOfSpeech_t {
    POS_Noun              = 0x0001,
    POS_Plural            = 0x0002,
    POS_NounPhrase        = 0x0004,
    POS_VerbParticiple    = 0x0008,
    POS_VerbTransitive    = 0x0010,
    POS_VerbIntrasitive   = 0x0020,
    POS_Adjective         = 0x0040,
    POS_Adverb            = 0x0080,
    POS_Conjunction       = 0x0100,
    POS_Preposition       = 0x0200,
    POS_Interjection      = 0x0400,
    POS_Pronoun           = 0x0800, 
    POS_DefiniteArticle   = 0x1000,
    POS_IndefiniteArticle = 0x2000,
    POS_Nominative        = 0x4000,
    POS_Nouns             = POS_Noun|POS_Plural|POS_NounPhrase|POS_Pronoun,
    POS_Verbs             = POS_VerbParticiple|POS_VerbTransitive|POS_VerbIntrasitive,
    POS_Unknown           = 0xffff
};

#define MAX_PHENOMES_IN_WORD 32
#define MAX_SYLLABLES_IN_WORD 32

struct WordFilter {
    WordFilter();
    unsigned partOfSpeech;
    unsigned minSyllables;
    unsigned maxSyllables;
    
    // stresses: eg. "0101010101" is iambic pentameter.
    char stresses[MAX_SYLLABLES_IN_WORD+1];
};

class Word;
typedef DynamicArray<Word*> WordArray;

class Word {
    public:
    char* text;
    PartOfSpeech_t partOfSpeech;

    unsigned char numSyllables;

    // pronunciation. Upper 2 bits: 
    // 0= no syllable, 
    // 1 = begins no stress.
    // 2 = begins primary stress, 
    // 3 = begins secondary stress.
    // terminated by 0xffff
    unsigned short* pron;
    void print();
    void printPron();
    void pronToText( unsigned short pr, char* buffer );
    bool match( WordFilter* filter );
    bool matchesStress( const char* stresses );

    WordArray synonyms;
};


class WordDatabase : public QObject, public QRunnable
{
	Q_OBJECT

    StringMap<Word*> _wordMap;
    DynamicArray<Word*> _wordArray;
    const char* m_filename;
    bool m_close;

signals:
	void progress(int progress);
	void loadComplete();

public:
	WordDatabase(const char* filename);
	~WordDatabase();

	void close();
    void run();
    bool findRhymes( DynamicArray<Word*>& results, const char* word, WordFilter* filter,
            WordArray* wordList = 0);
    Word* lookup( const char* whichword );
    void filter( WordArray& results, WordFilter* filter );
    bool makeWords( WordArray& results, const char* text );
    bool loadThesaurus( const char* thesaurus );
    void addSynonym( Word* base, Word* synonym );
    unsigned getSynonyms( WordArray& results, const char* wordtext );
};

class WordAssembler
{
public:
    WordAssembler();
    ~WordAssembler();
    void run( PartOfSpeech_t POS[], unsigned length, unsigned numSyllables,
            const char* rhymesWith, const char* stresses, WordArray* concepts);
private:
    void enumerateSolutions( unsigned depth, PartOfSpeech_t POS[], 
        unsigned length, unsigned numSyllables, WordArray& phrase, 
        const char* rhyme, const char* stresses );
    WordArray** table;
    struct Solution {
        unsigned char length;
        unsigned score;
        Word** words;
    };
    WordArray* concepts;
    void addSolution( WordArray& array );
    DynamicArray<Solution> solutions;
    static int solutionCompare( const void* pfirst, const void* psecond );
};

void
RemovePunctuation( char* text );

extern WordDatabase* _worddb;


#endif // WordDatabase_H
