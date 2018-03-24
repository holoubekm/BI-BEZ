#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <openssl/des.h>
#include <openssl/evp.h>

typedef unsigned char uchar;

using namespace std;

enum MODE
{
	ECB,
	CBC,
	NONE
};

class Data
{
public:
	Data(char* pdata, long plen) : len(plen)
	{
		data = new char[len];
		memcpy(data, pdata, len * sizeof(char));
	}

	Data(long plen) : len(plen)
	{
		data = new char[len];
	}

	~Data() { delete[] data; }
	char* data;
	long len;
};

// Data* DES_ECB(Data* data, const char Key[8], int mode)
// {
// 	int blocks = (data->len + 7) / 8;

// 	DES_cblock DES_Key;
// 	DES_key_schedule sched;

// 	memcpy(DES_Key, Key, 8);
// 	DES_set_odd_parity(&DES_Key);
// 	DES_set_key_checked(&DES_Key, &sched);

// 	uchar in_Block[8];
// 	uchar out_Block[8];
// 	Data* out = new Data(blocks * 8);

// 	for(int x = 0; x < blocks; x++)
// 	{
// 		for(int y = 0; y < 8 && x * 8 + y < data->len; y++)
// 			in_Block[y] = data->data[x * 8 + y];
// 		DES_ecb_encrypt(&in_Block, &out_Block, &sched, mode);
// 		for(int y = 0; y < 8; y++)
// 			out->data[x * 8 + y] = out_Block[y];
// 	}
// 	return out;
// }

Data* DES(Data* in, const char key[EVP_MAX_KEY_LENGTH], MODE mode, int way)
{
	OpenSSL_add_all_ciphers();
	EVP_CIPHER_CTX ctx;
	uchar init_Vec[EVP_MAX_IV_LENGTH] = "inicial. vektor";
	// const EVP_CIPHER* cipher = EVP_get_cipherbyname("DES");
	const EVP_CIPHER* cipher = (mode == CBC ? EVP_des_cbc() : EVP_des_ecb());
	if(!cipher)
	{
		cout << "Selected cipher can't be found" << endl;
		exit(1);
	}

	Data* out = new Data(((in->len + 7) / 8) * 8);
	int tmp_Len = 0;
	long st_Len = 0;

	if(way == DES_ENCRYPT)
	{
		EVP_EncryptInit(&ctx, cipher, (const uchar*)key, init_Vec);
		EVP_EncryptUpdate(&ctx, (uchar*)out->data, &tmp_Len, (uchar*)in->data, in->len);
		st_Len += tmp_Len;
		EVP_EncryptFinal(&ctx, (uchar*)(out->data + st_Len), &tmp_Len);
		st_Len += tmp_Len;
	}
	else
	{
		EVP_DecryptInit(&ctx, cipher, (const uchar*)key, init_Vec);
		EVP_DecryptUpdate(&ctx, (uchar*)out->data, &tmp_Len, (uchar*)in->data, in->len);
		st_Len += tmp_Len;
		EVP_DecryptFinal(&ctx, (uchar*)(out->data + st_Len), &tmp_Len);
		st_Len += tmp_Len;
	}
	return out;
}

void TestDES_ECB()
{
	char* str = new char[50];
	strcpy(str, "ahoj ahoj jak se vedeaaaa");
	Data*a = new Data(str, strlen(str));
	delete[] str;

	Data*b = DES(a, "aaaaaaaa", ECB, DES_ENCRYPT);
	Data*c = DES(b, "aaaaaaaa", ECB, DES_DECRYPT);
	cout << "len: " << c->len << endl;
	for(int x = 0; x < c->len; x++)
		cout << c->data[x];
	cout << endl;


	delete a;
	delete b;
	delete c;
}

int main(int argc, char* argv[])
{
	
	ifstream in("homer-simpson-cbc-enc.bmp", ifstream::binary | ifstream::in);
	if(!in.is_open())
		cout << "Can't open input file" << endl;
	
	char hPos[4];
	in.seekg(10, in.beg);
	in.read(hPos, 4);
	
	long hLen = 0;
	for(int x = 0; x < 4; x++)
	{
		// cout << hex << hPos[x] << " ";
		hLen += ((unsigned long)hPos[x]) << (8 * x);
	}
	// long hLen = (hPos[0] << 24) + (hPos[1] << 16) + (hPos[2] << 8) + hPos[3];
	char* header = new char[hLen];
	in.seekg(0, in.beg);
	in.read(header, hLen);

	in.seekg(0, in.end);
   	long dLen = in.tellg() - hLen;
	in.seekg(hLen, in.beg);
	char* data = new char[dLen];
	in.read(data, dLen);
	in.close();

	if(in.bad())
		cout << "Error occured during reading input" << endl;
	in.close();



	ofstream out("homer-simpson-cbc-dec.bmp", ifstream::binary | ifstream::out);
	out.write(header, hLen);

	Data* data_In = new Data(data, dLen);
	Data* data_Out = DES(data_In, "aaaaaaaa", CBC, DES_DECRYPT);

	out.write(data_Out->data, dLen);
	out.close();

	delete data_In;
	delete data_Out;
	delete[] data;
	delete[] header;
	return 0;
}