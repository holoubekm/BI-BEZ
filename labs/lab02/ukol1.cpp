#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <openssl/evp.h>
using namespace std;

int main(int argc, char* argv[])
{
	char hashFunction[]="sha256";

	EVP_MD_CTX ctx;
	const EVP_MD* type;
	unsigned char hash[EVP_MAX_MD_SIZE];
	int length;

	OpenSSL_add_all_digests();
	type = EVP_get_digestbyname(hashFunction);

	
	if(!type)
	{
		printf("Hash %s neexistuje.\n", hashFunction);
		exit(1);
	}


	int len = 3;
	unsigned char* text = new unsigned char[len];
	for(int x = 0; x < len; x++)
		text[x] = 0;

	bool carry = false;
	while(true)
	{
		text[0]++;
		carry = false;
		if(text[0] == 0)
			carry = true;

		for(int x = 1; x < len; x++)
		{
			if(carry)
			{
				text[x]++;
				carry = false;
			}

			if(text[x] == 0)
				carry = true;
		}

		EVP_DigestInit(&ctx, type);
		EVP_DigestUpdate(&ctx, text, len);
		EVP_DigestFinal(&ctx, hash, (unsigned int*) &length);

		//147 29 255

		if(hash[0] == 0xaa && hash[1] == 0xbb)
		{
			cout << "Nalezeno" << endl;

			for(int x = 0; x < len; x++)
			{
				cout << (int)text[x] << " ";
			}
			cout << endl;
			break;
		}
		// printf("Hash textu je: ");
		// for(i = 0; i < length; i++)
		// {
		// 	printf("%02x", hash[i]);
		// }
		// printf("\n");
	}

	return 0;
}