#ifndef ACCBUFFER_H
#define ACCBUFFER_H

#include <cstring>
#include <QDebug>

template <class T>
class AccBuffer
{
public:
    explicit AccBuffer(int size=1024000)
      : size(size), used(0),
        head(new T[size]), tail(head)
    {}

    ~AccBuffer()
    { delete [] head; }

    inline bool avail(int data_size) const
    {
        qDebug() << "avail" << used;
        return used >= data_size;
    }

    void putData(T data[], int data_size)
    {
        if (data_size + used > size)
            return;

        memcpy(tail, data, data_size * sizeof(T));
        tail += data_size;
        used += data_size;
    }

    void getData(T data[], int data_size)
    {
        if (data_size > used)
            return;

        memcpy(data, head, data_size * sizeof(T));
        tail -= data_size;
        used -= data_size;
        memcpy(head, head + data_size, used * sizeof(T)); // Shift
    }

private:
    int size, used;
    T *head, *tail;
};

#endif // ACCBUFFER_H
