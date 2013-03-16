#ifndef LIST_H
#define LIST_H

class ListBase
{   
public:
    ListBase();
    virtual ~ListBase();

    void addHead( void* item );
    void addTail( void* item );
    void clear();

    void* first();
    void* next();
    void* cur();

    void* last();
    void* prev();

    void remove();
    bool empty();
private:
    struct Entry {
        void* item;
        Entry* next;
        Entry* prev;
    };

    Entry _list;
    Entry* _pos;
};

template<class PTR>
class List : protected ListBase
{
public:
    void addHead( PTR item ) {
        ListBase::addHead( static_cast<void*>(item) );
    }

    void addTail( PTR item ) {
        ListBase::addTail( static_cast<void*>(item) );
    }

    void clear(bool del = false) {
        ListBase::clear();
    }

    PTR first() {
        return static_cast<PTR>(ListBase::first());
    }

    const PTR first() const {
        List<PTR>* ptr = const_cast<List<PTR>*>(this);
        return (static_cast<const PTR>(ptr->ListBase::first()));
    }

    const PTR next() const {
        List<PTR>* ptr = const_cast<List<PTR>*>(this);
        return (static_cast<const PTR>(ptr->ListBase::next()));
    }

    PTR last() {
        return static_cast<PTR>(ListBase::last());
    }

    PTR prev() {
        return static_cast<PTR>(ListBase::prev());
    }

    PTR cur() {
        return static_cast<PTR>(ListBase::cur());
    }

    void remove() {
        ListBase::remove();
    }

    bool empty() const {
        List<PTR>* ptr = const_cast<List<PTR>*>(this);
        return ptr->ListBase::empty();
    }
};

#endif /* LIST_H */
