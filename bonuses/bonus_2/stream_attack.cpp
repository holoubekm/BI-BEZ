#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <openssl/evp.h>
using namespace std;

typedef unsigned char byte;

byte charToInt(byte in)
{
	if(in >= 'a' && in <= 'f')
		return in - 'a' + 10;
	if(in >= '0' && in <= '9')
		return in - '0';

	cout << "Not a hex" << endl;
	return 0;
}

byte* hexToBin(byte* in, int out_Len)
{
	byte* out = new byte[out_Len];
	for(int x = 0; x < out_Len; x++)
	{
		out[x] = (charToInt(in[x * 2]) << 4) + charToInt(in[x * 2 + 1]);
	}
	return out;
}

int main(int argc, char* argv[])
{
	// ST 10 a 1
	byte hex1[] = "378b79600be926d5490984311e29cfbdd0cb542712ce682c0ae62fd8544ed02c0360d5b2958d5538169c6e6d0fe123d41a5999360221c2b69581063b178b2b2c06fc68d95f5b833d002681be9fda486810802b340bed68c2524c9c3e4060c7a89fda482111892b290da821df5e4c93311f29ceb4de8d75201ace632107a82bde";
	byte hex2[] = "3d9b7f6017e02d9153479e39182581b982cc403c5f816d6017e02d914d409c3c0932cfbf83de063a109d6e6017e768d64f40943d4c28c8a9d0cf433a0c8b792b43fa29d65f07d0100960d3bf9dc84b2a1a9c6e2443dc2dd2524684344b3381ad91df482111892b2f05a829df1a489d3a1933c9f4d0e45268088f786012fd21c5";
	
	int len_Hex1 = strlen((char*)hex1);
	int len_Hex2 = strlen((char*)hex2);
	int bin_Len = len_Hex1 / 2;

	if(len_Hex1 % 2 == 1 || len_Hex1 != len_Hex2)
	{
		cout << "Lengths differs" << endl;
		return 1;
	}

	byte* bin1 = hexToBin(hex1, bin_Len);
	byte* bin2 = hexToBin(hex2, bin_Len);
	byte* xored = new byte[bin_Len];

	for(int x = 0; x < bin_Len; x++)
	{
		xored[x] = bin1[x] ^ bin2[x];
		// cout << hex << (int)xored[x] << " ";
	}

	const int num = 104;
	byte words[][num] = {" the ", " be ", " to ", " of ", " and ", " a ", " in ", " that ", " have ", " I ", " it ", " for ", " not ", " on ", " with ", " he ", " as ", " you ", " do ", " at ", " Word ", " this ", " but ", " his ", " by ", " from ", " they ", " we ", " say ", " her ", " she ", " or ", " an ", " will ", " my ", " one ", " all ", " would ", " there ", " their ", " what ", " Word ", " so ", " up ", " out ", " if ", " about ", " who ", " get ", " which ", " go ", " me ", " when ", " make ", " can ", " like ", " time ", " no ", " just ", " him ", " know ", " take ", " Word ", " people ", " into ", " year ", " your ", " good ", " some ", " could ", " them ", " see ", " other ", " than ", " then ", " now ", " look ", " only ", " come ", " its ", " over ", " think ", " also ", " Word ", " back ", " after ", " use ", " two ", " how ", " our ", " work ", " first ", " well ", " way ", " even ", " new ", " want ", " because ", " any ", " these ", " give ", " day ", " most ", " us "};
	
	for(int cur = 0; cur < num; cur++)
	{
		byte* word = words[cur];
		int word_Len = strlen((char*)word);
		cout << word_Len << endl;

		cout << endl << word << endl;
		for(int x = 0; x < bin_Len; x++)
		{
			// cout << (int)xored[x] << " ";
			cout << dec << x << ": ";
			for(int y = 0; y < word_Len; y++)
			{
				byte cur = xored[(x + y) % bin_Len] ^ word[y];
				cout << hex << cur;
			}cout << endl;
		}	
	}


	//Kontrola OT pro generovani dalsiho
	byte OT[] = "But the innate craft of the wilderness rose to guide his berserk rage. He remembered Techotl's warning of an ambush. It was quit";
	for(int x = 0; x < bin_Len; x++)
	{
		cout << (char)(xored[x] ^ OT[x]);
	}
	cout << endl;


	delete[] bin1;
	delete[] bin2;
	delete[] xored;
	return 0;
}