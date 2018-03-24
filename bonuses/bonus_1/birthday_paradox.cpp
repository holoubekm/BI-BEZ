#include <stdint.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <map>
#include <cstring>
#include <set>
#include "openssl/sha.h"

using namespace std;
typedef unsigned char byte;

static char smenka[] = "V Praze dne 24.12.2086. Za tuto smenku zaplatim bez odporu %d Kc. holoumar";

// static uint32_t hardcoded_result_a = 1282599353;
// static uint32_t hardcoded_hash[7] = {170, 233, 149, 197, 23, 200, 59};
// static uint32_t hardcoded_result_b = 1274267038;


void sha256(char* string, uint16_t len)
{
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, len);
    SHA256_Final((byte*)string, &sha256);
}

void findCollision(int k)
{
	uint32_t size = 256 * 256 * 256 * 32;
	byte* data = new byte[size * 7];
	uint8_t* pos = new uint8_t[size];
	for(uint32_t x = 0; x < size; x++)
		pos[x] = 0;

	cout << "Allocated" << endl;

	char len = strlen(smenka);
	char* string = new char[len + 10];

	uint32_t delta = 350000000;
	uint32_t min = k * delta;
	uint32_t max = (k + 1) * delta;

	cout << "Computing from: " << min << ", to: " << max << endl;
	for(uint32_t x = min; x < max; x++)
	{
		sprintf(string, smenka, x);
		sha256(string, len - 1 + floor(log10(x)));

		// uint32_t a = (byte)'a';
		// uint32_t b = (byte)'a';
		// uint32_t c = (byte)'a';
		// pos[i] = 1;
		// strcpy((char*)data + i + 0, "aaaaaaa");
		// strcpy((char*)data + i + 7, "aaaaaaa");
		uint32_t a = (byte)string[0];
		uint32_t b = (byte)string[1];
		uint32_t c = (byte)string[2];
		uint32_t i = (((a << 8) + b) << 8) + c;
	    for(int y = 0; y < 7; y++)
			data[i + pos[i] * 7 + y] = string[y];

		uint8_t p = pos[i];
		for(int j = 0; j < p; j++)
		{
			bool same = true;
			for(int v = 0; v < 7; v++)
			{
				// cout << data[i + j * 7 + v] << endl;
				if(data[i + j * 7 + v] != data[i + p * 7 + v])
					same = false;
			}
			if(same)
			{
				cout << "Found collision: " << x << endl;
				for(int v = 0; v < 7; v++)
					cout << hex << (int)(byte)string[v] << " ";
				cout << endl;
			}
		}
		pos[i]++;
		if(pos[i] >= 31)
		{
			pos[i]--;
		}

		if(x % 50000000 == 0)
		{
			// cout << (string + 39) << endl;
			cout << "50000000x" << endl;
		}
	}

	delete[] data;
	delete[] pos;
	delete[] string;
}

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		cout << argv[0] << " iter" << endl;
		return 1;
	}

	int k = atoi(argv[1]);
	findCollision(k);

	//Potvrzeni "spatnych" vysledu
	char* out = new char[strlen(smenka)];
	strcpy(out, "V Praze dne 24.12.2086. Za tuto smenku zaplatim bez odporu 1282599353 Kc. holoumar");
	cout << out << endl;
	sha256(out, strlen(out) - 1);
	for(int x = 0; x < 7; x++)
		cout << hex << (int)(uint8_t)out[x] << " ";
	cout << endl;
	strcpy(out, "V Praze dne 24.12.2086. Za tuto smenku zaplatim bez odporu 1274267038 Kc. holoumar");
	cout << out << endl;
	sha256(out, strlen(out) - 1);
	for(int x = 0; x < 7; x++)
		cout << hex << (int)(uint8_t)out[x] << " ";
	cout << endl;
	delete[] out;

	return 0;
}