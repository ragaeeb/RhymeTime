#include <assert.h>
#include "List.h"

ListBase::ListBase()
{
    _list.next = _list.prev = &_list;
    _list.item = 0;
    _pos = &_list;;
}

ListBase::~ListBase()
{
    clear();
}

void
ListBase::clear()
{
    Entry* cur = _list.next;

    while( cur != &_list ) {
        Entry* next = cur->next;
        delete cur;
        cur = next;
    }

    _list.next = _list.prev = &_list;
    _list.item = 0;
    _pos = &_list;;
}

void
ListBase::addHead( void* item )
{
    Entry* entry = new Entry;
    entry->item = item;

    entry->prev = &_list;
    entry->next = _list.next;
    entry->next->prev = entry;
    _list.next = entry;
}

void
ListBase::addTail( void* item )
{
    Entry* entry = new Entry;

    entry->item = item;
    entry->next = &_list;
    entry->prev = _list.prev;
    _list.prev->next = entry;
    _list.prev = entry;
}

void*
ListBase::first()
{
    _pos = _list.next;
    if ( _pos == &_list ) {
        return 0;
    }

    return _pos->item;
}

void* 
ListBase::next()
{
    _pos = _pos->next;
    if ( _pos == &_list ) {
        return 0;
    }

    return _pos->item;
}

void* 
ListBase::last()
{
    _pos = _list.prev;
    if ( _pos == &_list ) {
        return 0;
    }

    return _pos->item;
}

void*
ListBase::prev()
{
    _pos = _pos->prev;
    if ( _pos == &_list ) {
        return 0;
    }

    return _pos->item;
}

void*
ListBase::cur()
{
    return _pos->item;
}

void
ListBase::remove()
{
    if ( _pos == &_list ) {
        assert(0);
        return;
    }

    _pos->next->prev = _pos->prev;
    _pos->prev->next = _pos->next;
    Entry* entry = _pos->prev;
    delete _pos;
    _pos= entry;
}

bool
ListBase::empty()
{
    return _list.next == &_list;
}

