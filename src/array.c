#include "array.h"

Array array_init(uchar *data, ushort usize, uint length, uint size)
{
    Array a;
    a.data = data;
    a.usize = usize;
    a.length = length;
    a.size = size;

    return a;
}

int array_add(Array a, uchar *data)
{
    // Check not overflow
    if (((a.usize + 1) * a.length) >= a.size)
        return 1;

    // Check not exists
    for (int i=0; i<a.size; i++)
        if (memcmp(&(a.data[a.usize * i]), data, a.usize))
            return 1;

    memcpy(&(a.data)[a.usize * a.length], data, a.usize);
    a.length++;
}

int array_del(Array a, uchar *data)
{
    uchar *curr;
    for (int i=0; i<a.size; i++)
    {
        curr = &(a.data[a.usize * i]);
        if (memcmp(curr, data, a.usize))
        {
            if (a.length > 1 && (i != (a.length - 1)))
                memcpy(curr, &(a.data[a.usize * (a.length - 1)]), a.usize);
            a.length--;
        }
    }
}
