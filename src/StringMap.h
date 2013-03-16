#ifndef STRINGMAP_H
#define STRINGMAP_H

#include "List.h"

class StringMapBase
{
public:
    StringMapBase();
    StringMapBase( unsigned numEntries );
    ~StringMapBase();

    bool add( const char* key, void* value );
    void* lookup( const char* key );
    void remove( const char* key );

    void* first();
    void* next();
    void clear();
    bool empty();    

protected:
    bool allowDuplicates;

private:
    unsigned nextPrimeNumber( unsigned size );
    void construct(unsigned size);
    void destruct();
    unsigned hash( const char* str );
    int entries_alloc;
    struct Entry_t {
        char* key;
        void* value;
        struct Entry_t* next;
    }** entries;

    int index;
    bool _empty;
    Entry_t* pos;
};

template< class PTR >
class StringMap : protected StringMapBase
{
public:
    StringMap() 
    {
    }

    StringMap( unsigned Size ) : StringMapBase(Size)
    {
    }

    bool add( const char* key, PTR value ) {
        return StringMapBase::add( key, static_cast<void*>(value) );
    }

    PTR lookup( const char* key ) {
        return static_cast<const PTR>(StringMapBase::lookup( key ));
    }

    void remove( const char* key ) {
        StringMapBase::remove( key );
    }

    PTR first() {
        return static_cast<PTR>(StringMapBase::first());
    }

    const PTR next() const {
        StringMap<PTR>* ptr = const_cast<StringMap<PTR>*>(this);
        return (static_cast<const PTR>(ptr->StringMapBase::next()));
    }

    void clear() {
        StringMapBase::clear();
    }

    bool empty() {
        return StringMapBase::empty();
    }
    
};

template< class PTR >
class StringMultiMap : protected StringMapBase
{
public:
    StringMultiMap() 
    {
    }

    StringMultiMap( unsigned Size ) : StringMapBase(Size)
    {
    }

    
    void add( const char* key, PTR value ) {

        List<PTR>* list = 
            static_cast<List<PTR>* >( StringMapBase::lookup(key) );

        if ( list == 0 ) {
            list = new List<PTR>();
            StringMapBase::add( key, static_cast< void* >(list) );
        }

        list->addTail( value );
    }

    const List<PTR>* lookup( const char* key ) {
        List<PTR>* list = 
            static_cast< List<PTR>* >( StringMapBase::lookup(key) );
        if ( list == 0 ) {
            return &EmptyList;
        }
            
        return list;
    }


    List<PTR>* first() {
        return static_cast< List<PTR>* >(StringMapBase::first());
    }

    const List<PTR>* next() const {
        StringMultiMap< PTR >* ptr = const_cast<StringMultiMap< PTR >* >(this);
        return (static_cast<const List<PTR>* >(ptr->StringMapBase::next()));
    }

    void clear() {
        StringMapBase::clear();
    }

private:
    List<PTR> EmptyList;
};

#endif // STRINGMAP_H
