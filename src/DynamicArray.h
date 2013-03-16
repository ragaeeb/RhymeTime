#ifndef _DYNAMIC_ARRAY_H
#define _DYNAMIC_ARRAY_H

#include <assert.h>
#include <stdlib.h>

template< class T >
class DynamicArray {

private:
    T* _array;
    unsigned _array_alloc;
    unsigned _array_used;
public:
    
    DynamicArray() {
        construct();
    }
    
    ~DynamicArray() {
        destruct();
    }

    void clear() {
        destruct();
        construct();
    }

    void construct() {
        _array_used = 0;
        _array_alloc = 1;
        _array = (T*)malloc(_array_alloc*sizeof(T));
    }

    void destruct() {
        free( _array );
        _array = 0;
    }

    void removeAt( unsigned index ) {
        assert( index < _array_used );
        memmove( &_array[index], &_array[index+1], ( _array_used - index - 1 ) * sizeof(T));
        _array_used--;
    }

    void resize( unsigned newSize ) {
        assert( newSize <= _array_alloc );
        _array_used = newSize;
    }

    void append( const T& t ) {
        if ( _array_used == _array_alloc ) {
            _array_alloc *= 2;
            _array = (T*)realloc(_array, _array_alloc * sizeof(T));
        }
        _array[_array_used++] = t;
    }

    unsigned size() {
        return _array_used;
    }

    T& operator[]( unsigned index ) {
        assert( index < _array_used );
        return _array[index];
    }
};


#endif // _DYNAMIC_ARRAY_H
