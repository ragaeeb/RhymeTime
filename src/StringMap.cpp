#include <string.h>
#include <stdlib.h>

#include "StringMap.h"

StringMapBase::StringMapBase()
{
    construct(499);
}

StringMapBase::StringMapBase( unsigned size )
{
    construct( nextPrimeNumber( size ) );
}

unsigned 
StringMapBase::nextPrimeNumber( unsigned size )
{
    if ( size == 0 ) {
        size = 2;
    }

    for ( unsigned i = 2; i < size; i++ ) {
        if ( ( size % i ) == 0 ) {
            size++;
            i = 2;
        }
    }

    return size;
}

void
StringMapBase::construct( unsigned size ) 
{
    entries_alloc = size; // prime
    entries = (Entry_t**)malloc( entries_alloc * sizeof(Entry_t*));
    memset( entries, 0, entries_alloc * sizeof(Entry_t*));
    pos = 0;
    index = 0;
    allowDuplicates = false;
    _empty = true;
}

void
StringMapBase::destruct()
{
    int i;
    for ( i = 0; i < entries_alloc; i++ ) {
        Entry_t* cur = entries[i];
        while( cur ) {
            Entry_t* next = cur->next;
            free( cur->key );
            free( cur );
            cur = next;
        }
    }

    free( entries );
}

void
StringMapBase::clear()
{
    destruct();
    construct(entries_alloc);
}

bool
StringMapBase::empty()
{
    return _empty;
}

StringMapBase::~StringMapBase()
{
    destruct();
}

bool
StringMapBase::add( const char* key, void* value )
{
    unsigned h = hash( key );

    Entry_t* cur = entries[h];
    if ( !allowDuplicates ) {
        while ( cur ) {
            if ( strcmp( cur->key, key ) == 0 ) {
                return false;
            }

            cur = cur->next;
        }
    }

    cur = (Entry_t*)malloc( sizeof(Entry_t));
    cur->key = _strdup( key );
    cur->value = value;
    cur->next = entries[h];
    entries[h] = cur;
    _empty = false;
    return true;
}

void*
StringMapBase::lookup( const char* key )
{
    unsigned h = hash( key );

    Entry_t* cur = entries[h];
    while ( cur ) {
        if ( strcmp( cur->key, key ) == 0 ) {
            return cur->value;
        }

        cur = cur->next;
    }
    return 0;
}

void 
StringMapBase::remove( const char* key )
{
    unsigned h = hash( key );

    Entry_t* cur = entries[h];
    Entry_t* prev = 0;
    while( cur ) {
        if ( strcmp( cur->key, key ) == 0 ) {
            if ( prev ) {
                prev->next = cur->next;
            } else {
                entries[h] = cur->next;
            }

            free( cur->key );
            free( cur );
            break;
        }

        prev = cur;
        cur = cur->next;
    }

    if ( pos != 0 && pos == cur ) {
        if ( prev ) {
            pos = prev;
        } else {
            pos = 0;
        }
    }
}

void*
StringMapBase::first()
{
    index = -1;
    pos = 0;
    return next();
}

void*
StringMapBase::next()
{
    for( ;; ) {
        if ( pos ) {
            pos = pos->next;
            if ( pos ) {
                return pos->value;
            }
        }

        if ( index == entries_alloc - 1 ) {
            return 0;
        }
        index++;
        pos = entries[index];
        if ( pos ) {
            return pos->value;
        }
    }
}

unsigned 
StringMapBase::hash( const char* str )
{
    unsigned hash = 5381;
    int c;
    while( ( c = ( unsigned char ) *str++ ) ) {
        hash = ( (hash << 5 ) + hash) + c; /* hash * 33 + c */
    }

    return hash % entries_alloc;
}
