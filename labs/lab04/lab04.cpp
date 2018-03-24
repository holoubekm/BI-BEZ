#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <iomanip>
#include <cstring>
// #include <file.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include <arpa/inet.h> 

typedef unsigned char byte;

using namespace std;

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

void doSeal(const char rsa_Path[], const char out_Path[], const char in_Path[], const EVP_CIPHER* cipher, byte iv[EVP_MAX_IV_LENGTH])
{
    uint32_t cipher_Type = EVP_CIPHER_nid(cipher);
	RSA* rsa_Pkey 	= NULL;
    EVP_PKEY* pkey 	= EVP_PKEY_new();
    EVP_CIPHER_CTX ctx;

    byte* ek;
    int ek_Len;

    FILE* rsa_Pkey_File;
    rsa_Pkey_File = fopen(rsa_Path, "r");
    if(!rsa_Pkey_File)
    {
    	cout << "Can't open public keyfile" << endl;
    	return;
    }

    PEM_read_RSA_PUBKEY(rsa_Pkey_File, &rsa_Pkey, NULL, NULL);
    EVP_PKEY_assign_RSA(pkey, rsa_Pkey);
    fclose(rsa_Pkey_File);

    EVP_CIPHER_CTX_init(&ctx);
    ek = (byte*)malloc(EVP_PKEY_size(pkey));
    EVP_SealInit(&ctx, cipher, &ek, &ek_Len, iv, &pkey, 1);

    uint32_t ek_Len_N = htonl(ek_Len);
    uint32_t cipher_Type_N = htonl(cipher_Type);

    ofstream of(out_Path, ios::binary);
    if(!of.is_open())
    {
    	cout << "Can't open output file" << endl;
    	return;
    }

    of.write((char*)&cipher_Type_N, sizeof(cipher_Type_N));
    of.write((char*)&ek_Len_N, sizeof(ek_Len_N));
    of.write((char*)ek, ek_Len);
    of.write((char*)iv, EVP_CIPHER_iv_length(cipher));

    ifstream in(in_Path, ios::binary);
    if(!in.is_open())
    {
    	cout << "Can't open input file" << endl;
    	return;
    }

    in.seekg(0, ios::end);
	streamsize ot_Len = in.tellg();
	in.seekg(0, ios::beg);

	const uint16_t buf_Len = 1024;
	char buf_In[buf_Len];
	int len_Out;
	char buf_Out[buf_Len + EVP_MAX_IV_LENGTH];

	int cnt = (ot_Len + buf_Len - 1) / buf_Len;
	for(int x = 1; x <= cnt; x++)
	{
		int to_Read = (x * buf_Len) <= ot_Len ? buf_Len : ot_Len % buf_Len;
		in.read(buf_In, to_Read);
		EVP_SealUpdate(&ctx, (byte*)buf_Out, &len_Out, (const byte*)buf_In, to_Read);
		of.write((char*)buf_Out, len_Out);
	}
	in.close();
	EVP_SealFinal(&ctx, (byte*)buf_Out, &len_Out);
	of.write((char*)buf_Out, len_Out);
	of.close();

	EVP_PKEY_free(pkey);
	free(ek);
}


void undoSeal(const char rsa_Path[], const char out_Path[], const char in_Path[])
{

    uint32_t cipher_Type;
	RSA* rsa_Pkey 	= NULL;
    EVP_PKEY* pkey 	= EVP_PKEY_new();
    EVP_CIPHER_CTX ctx;
    byte* ek;
    byte iv[EVP_MAX_IV_LENGTH];
    int ek_Len;

    FILE* rsa_Pkey_File;
    rsa_Pkey_File = fopen(rsa_Path, "r");
    if(!rsa_Pkey_File)
    {
    	cout << "Can't open private keyfile" << endl;
    	return;
    }

    PEM_read_RSAPrivateKey(rsa_Pkey_File, &rsa_Pkey, NULL, NULL);
    EVP_PKEY_assign_RSA(pkey, rsa_Pkey);
    fclose(rsa_Pkey_File);

    EVP_CIPHER_CTX_init(&ctx);
    ek = (byte*)malloc(EVP_PKEY_size(pkey));   

    uint32_t cipher_Type_N;
    uint32_t ek_Len_N;

    ifstream in(in_Path, ios::binary);
    if(!in.is_open())
    {
    	cout << "Can't open input file" << endl;
    	return;
    }

    in.seekg(0, ios::end);
	streamsize st_Len = in.tellg();
	in.seekg(0, ios::beg);

    in.read((char*)&cipher_Type_N, sizeof(cipher_Type_N));
    in.read((char*)&ek_Len_N, sizeof(ek_Len_N));

    cipher_Type = ntohl(cipher_Type_N);
    ek_Len = ntohl(ek_Len_N);

    OpenSSL_add_all_ciphers();
    const EVP_CIPHER* cipher = EVP_get_cipherbynid(cipher_Type);

    in.read((char*)ek, ek_Len);
    in.read((char*)iv, EVP_CIPHER_iv_length(cipher));

	EVP_OpenInit(&ctx, cipher, ek, ek_Len, iv, pkey);

	int total_Read = sizeof(cipher_Type_N) + sizeof(ek_Len_N) + ek_Len + EVP_CIPHER_iv_length(cipher);
	in.seekg(total_Read, ios::beg);
	st_Len = st_Len - total_Read;

	const uint16_t buf_Len = 1024;
	char buf_In[buf_Len];
	int len_Out;
	char buf_Out[buf_Len + EVP_MAX_IV_LENGTH];

	int cnt = (st_Len + buf_Len - 1) / buf_Len;
	ofstream of(out_Path, ios::binary);
	if(!in.is_open())
    {
    	cout << "Can't open output file" << endl;
    	return;
    }

	for(int x = 1; x <= cnt; x++)
	{
		int to_Read = (x * buf_Len) <= st_Len ? buf_Len : st_Len % buf_Len;
		in.read(buf_In, to_Read);
		EVP_OpenUpdate(&ctx, (byte*)buf_Out, &len_Out, (const byte*)buf_In, to_Read);
		of.write((char*)buf_Out, len_Out);
	}
	EVP_OpenFinal(&ctx, (byte*)buf_Out, &len_Out);
	of.write((char*)buf_Out, len_Out);
	of.close();

	EVP_PKEY_free(pkey);
	free(ek);
}

void dump(byte* in, int len)
{
	cout << setfill('0') << hex;
	for(int x = 0; x < len; x++)
	{
		cout << setw(2) << (int)in[x] << " ";
	}cout << dec << endl;
}
int main(int argc, char* argv[])
{
	if(argc < 5)
	{
		cout << "usage: " << argv[0] << " [encrypt .. decrypt] path_to_cert input output" << endl; 
		return 0;
	}

	if(strcmp(argv[1], "encrypt") == 0)
	{
		cout << "Encrypting.." << endl;
		OpenSSL_add_all_ciphers();
		const EVP_CIPHER *cipher = EVP_get_cipherbyname("aes-256-ofb");
		byte iv[EVP_MAX_IV_LENGTH] = "init. vektor";
		doSeal(argv[2], argv[4], argv[3], cipher, iv);
		// doSeal("certs/pubkey.pem", "out2.bin", "ot.txt", cipher, iv);
		
	}
	else
	{
		cout << "Decrypting.." << endl;
		undoSeal(argv[2], argv[4], argv[3]);
		// undoSeal("certs/privkey.pem", "ot2.txt", "out2.bin");
	}

	
	return 0;
}