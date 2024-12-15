
#ifndef AVLTREE_H_DYNAMICARRAY_H
#define AVLTREE_H_DYNAMICARRAY_H
#define INIT_SIZE 2
template<class T>
class DynamicArray
{
public:
    T* array;
    long size;
    long max;

    T& operator[](long index)
    {
        if(index > max)
        {
            //throw
        }
        return array[index];
    }
    const T& operator[](long index)const
    {
        if(index > max)
        {
            //throw
        }
        return array[index];
    }

    ~DynamicArray()
    {
        delete[] array;
    }

    DynamicArray(): array(new T[INIT_SIZE]), size(0), max(2) {}
    DynamicArray(const long size): array(new T[size]), size(0), max(size){}

    void push(T& data)
    {
        if(size == max)
        {
            enlarge();
        }
        array[size++] = data;
    }

    void enlarge()
    {
        T* newArray = new T[2 * size];
        for(long i = 0 ; i <  max ; ++i)
        {
            newArray[i] = array[i];// need to check if an exception is thrown
        }
        delete[] array;
        array = newArray;
        max *= 2;
    }

    void decreaseSize()
    {
        T* newArray = new T[size * 2];
        for(long i = 0 ; i <  size ; ++i)
        {
            newArray[i] = array[i];
        }

        delete[] array;
        array = newArray;
        max = size * 2;
    }

    void remove()
    {
        --size;
        if((size * 4) <= max)
        {
            decreaseSize();
        }
    }

};

#endif //AVLTREE_H_DYNAMICARRAY_H

