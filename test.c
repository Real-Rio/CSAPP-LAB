#include<stdio.h>
#include<stdlib.h>
void main() {
    char str[1024];
    sprintf(str,"%d ",sizeof(size_t));
    sprintf(str,"%s","fuckyouall");
    printf("%s",str);
}