#ifndef storage_h
#define storage_h

#include "types.h"

#include <string.h>
#include <stdlib.h>

template <typename T>
class Storage
{
public:
    Storage(size_t count = 0) {
        this->count = count;
        buffer_size = count + sizeof(T);
        buffer = (T *)malloc(buffer_size);
    }
    ~Storage() { free(buffer); }
    
    unsigned Count() const { return (unsigned)count; }
    void Empty() { count = 0; }
    void Insert(T element);
    void Remove(unsigned index);
    T * GetPointer(unsigned index) { return &buffer[index]; } // TODO: how?
    
    T   operator[](unsigned index) { return buffer[index]; }
    T&  operator[](unsigned index) const { return buffer[index]; }
    operator T *() { return buffer; };
    operator const T *() const;
    
private:
    T * buffer = NULL;
    size_t buffer_size;
    size_t count;
};



template <typename T>
void Storage<T>::Insert(T element)
{
    ++count;
    if ( count * sizeof(T) > buffer_size ) {
        buffer_size += 16 * sizeof(T);
        buffer = (T *)realloc(buffer, buffer_size);
    }
    buffer[count - 1] = element;
}


template <typename T>
void Storage<T>::Remove(unsigned index)
{
    buffer[index] = buffer[--count];
}


#endif /* storage_h */
