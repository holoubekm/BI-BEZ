#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <openssl/evp.h>
using namespace std;

int main(int argc, char* argv[])
{
	unsigned char ot1[1024] = "ahoj pro rc4.";
	unsigned char ot2[1024] = "Text pro rc1.";
  unsigned char st1[1024];  // sifrovany text
  unsigned char st2[1024];  // sifrovany text
  unsigned char key[EVP_MAX_KEY_LENGTH] = "Muj klic";
  unsigned char iv[EVP_MAX_IV_LENGTH] = "inicial. vektor"; 
  const char cipherName[] = "RC4";
  const EVP_CIPHER * cipher;
 
  OpenSSL_add_all_ciphers();
  /* sifry i hashe by se nahraly pomoci OpenSSL_add_all_algorithms() */
  cipher = EVP_get_cipherbyname(cipherName);
  if(!cipher) {
    printf("Sifra %s neexistuje.\n", cipherName);
    exit(1);
  }
 
  int ot1Length = strlen((const char*) ot1);
  int ot2Length = strlen((const char*) ot2);
  int st1Length = 0;
  int st2Length = 0;
  int tmpLength = 0;
 
  EVP_CIPHER_CTX ctx; // struktura pro kontext
 
 
  /* Sifrovani */
  EVP_EncryptInit(&ctx, cipher, key, iv);  // nastaveni kontextu pro sifrovani
  EVP_EncryptUpdate(&ctx,  st1, &tmpLength, ot1, ot1Length);  // sifrovani ot
  st1Length += tmpLength;
  EVP_EncryptFinal(&ctx, st1 + st1Length, &tmpLength);  // dokonceni (ziskani zbytku z kontextu)
  st1Length += tmpLength;
 
	printf("OT1: %s\n", ot1);
	cout << "ST1: ";
	for(int x= 0; x < st1Length; x++)
		printf("%02x", st1[x]);
	cout << endl;

 
  tmpLength = 0;

  EVP_EncryptInit(&ctx, cipher, key, iv);  // nastaveni kontextu pro sifrovani
  EVP_EncryptUpdate(&ctx,  st2, &tmpLength, ot2, ot1Length);  // sifrovani ot
  st2Length += tmpLength;
  EVP_EncryptFinal(&ctx, st2 + st2Length, &tmpLength);  // dokonceni (ziskani zbytku z kontextu)
  st2Length += tmpLength;

  printf("OT2: %s\n", ot2);
	cout << "ST2: ";
	for(int x= 0; x < st2Length; x++)
		printf("%02x", st2[x]);
	cout << endl;


// unsigned char ot1[1024] = "tttt pro rc4.";
// unsigned char ot2[1024] = "Text pro rc1.";
 cout << "Diff: ";

 int* diff = new int[min(st2Length, st1Length)];
 for(int x= 0; x < min(st2Length, st1Length); x++)
{
 	// cout << st1[x] << " - ";
 	// cout << st2[x]<< " ";
 	int d = ((int)((unsigned char)st1[x] ^ st2[x])) % 256;
 	// d += d < 0 ? 256 : 0;
 	diff[x] = d;

 	cout << (char)((d ^ ot2[x])%256) << " ";
 }
 
 for(int x= 0; x < min(st2Length, st1Length); x++)
 	cout << (int)(((unsigned char)ot1[x]))<< " ";
 cout << endl;
 //T_p
 

  /* Desifrovani */
  EVP_DecryptInit(&ctx, cipher, key, iv);  // nastaveni kontextu pro desifrovani
  EVP_DecryptUpdate(&ctx, ot1, &tmpLength,  st1, st1Length);  // desifrovani st
  ot1Length += tmpLength;
  EVP_DecryptFinal(&ctx, ot1 + ot1Length, &tmpLength);  // dokonceni (ziskani zbytku z kontextu)
  ot1Length += tmpLength;
  return 0;
}