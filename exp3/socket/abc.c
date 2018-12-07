#include <stdio.h>
#include <string.h>

int main(){
    int v1;
    int v2;
    int v3;
    int v4;
    unsigned int v6;
    unsigned long num[0x18f4d];
    memset(num, 0, 0x18f4d*sizeof(unsigned long));
    num[0] = 9029;
    num[1] = 1 + 9029;
    num[2] = 4 + 9029;
    num[3] = 9 + 9029;
    num[4] = 16 +9029;
    for(int i=5;i<0x18f4d;i++){
        v1 = num[i-1];
        v2 = v1 - (unsigned long)num[i-2];
        v3 = num[i-3];
        v4 = v3 - (unsigned long)num[i-4] + v2;
        v6 = v4 + 4660*(unsigned long)num[i-5];
        num[i] = (unsigned long)v6;
        printf("num: %d, value: %u\n", i, v6);
    }
    return 0;
}
