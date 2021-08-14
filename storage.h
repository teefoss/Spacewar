#ifndef storage_h
#define storage_h

#include <string.h>
#include <stdlib.h>


template<typename T>
class Storage
{
public:
    Storage(unsigned count = 0) {
        buffer = (T *)calloc(count, sizeof(*buffer));
        num_elements = count;
    }
    ~Storage() { free(buffer); }
    
    unsigned Count() const { return num_elements; }
    void Empty() { num_elements = 0; }
    void Append(T element) { Insert(num_elements, element); }
    void Insert(unsigned i, T element);
    void Remove(unsigned i);
    
    T operator[](unsigned i) { return buffer[i]; }
    T& operator[](unsigned i) const { return buffer[i]; }
    operator T *() { return buffer; };
    operator const T *() const;
    
    // TODO:
    // Particle * p = &particles[i] results in 'cannot take addr of temp obj'?
    
private:
    T * buffer = NULL;
    unsigned num_elements;
};


template <typename T>
void Storage<T>::Insert(unsigned i, T element)
{
    size_t size = sizeof(T);
        
    buffer = (T *)realloc(buffer, (num_elements + 1) * size);
    
    memmove(&buffer[i + 1], &buffer[i], size * (num_elements - i));
    memmove(&buffer[i], &element, size);
    ++num_elements;
}


template <typename T>
void Storage<T>::Remove(unsigned i)
{
    memmove(&buffer[i], &buffer[i + 1], sizeof(T) * (num_elements - i - 1));
    --num_elements;
}

#endif /* storage_h */
