#include <stdio.h>
int isLessOrEqual(int x, int y)
{
    int signX = x >> 31, signY = y >> 31;
    int res = x + (~y) + 1;
    printf("%d %d %d\n", !(x ^ y), x & (!y), res >> 31);
    return (!(x ^ y)) | (signX & (!signY)) | (res >> 31);
}

int main()
{
    int a = isLessOrEqual(0x80000000, 0x7fffffff);
    printf("%d\n", a);
}