#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
using namespace std;

#define TO_BYTE(x) (x) & 255
#define SWAP(x,y) do { (x)^=(y); (y)^=(x); (x)^=(y); } while (0)
 
static unsigned char A[256];
static int i=0, j=0;
 
void inicializace_sifry(const char *heslo) {
    size_t delka = strlen(heslo);
    for (i=0; i<256; i++)
        A[i] = i;
    for (i=0; i<256; i++) {
        // cout << j << " ";
        j = TO_BYTE(j + A[TO_BYTE(i)] + heslo[j % delka]);
        SWAP(A[TO_BYTE(i)], A[j]);
    }
    i = 0; j = 0;
}
 
unsigned char sifruj_byte(unsigned char c) {
    int k;
    i = TO_BYTE(i+1);
    j = TO_BYTE(j + A[i]);
    cout << endl << (int)A[i] << endl;
    cout << (int)A[j] << endl;
    SWAP(A[i], A[j]);
    cout << (int)A[i] << endl;
    cout << (int)A[j] << endl;
    k = TO_BYTE(A[i] + A[j]);
    return c ^ A[k];
}

int main(int argc, char* argv[])
{
    inicializace_sifry("ahoj");
    sifruj_byte('a');
    cout << endl;
    cout << endl;
    cout << j << endl;
    cout << endl;
    inicializace_sifry("ahoj");
    return 0;
}