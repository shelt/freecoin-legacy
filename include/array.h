#ifndef ARRAY_H
#define ARRAY_H

typedef struct
{
    uint length;
    ushort usize;
    uchar *data;
} Array;

Array array_init(uchar *data, ushort usize, uint length, uint size);
int array_add(Array a, uchar *data);
int array_add(Array a, uchar *data);


#endif
