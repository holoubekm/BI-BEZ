#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <iomanip>
#include <cstring>

#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include <arpa/inet.h> 

typedef unsigned char byte;

using namespace std;

void die(const char in[])
{
	cout << in << endl;
	exit(1);
}

int main(int argc, char* argv[])
{
	// const char* request = "GET /student/odkazy HTTP/1.1\r\nHost: fit.cvut.cz\r\n\r\n";
	const char* request = "GET /student/odkazy HTTP/1.0\r\nHost: fit.cvut.cz\r\n\r\n";
	const int buf_Len = 512;
	char buf[buf_Len];
	hostent* ent = gethostbyname("fit.cvut.cz");
	if(!ent)
		die("Can't fint hostname");

	sockaddr_in srv_Addr;	
	bzero(&srv_Addr, sizeof(srv_Addr));
	srv_Addr.sin_family = AF_INET;
	srv_Addr.sin_port = htons(443);
	memcpy(&srv_Addr.sin_addr, ent->h_addr_list[0], ent->h_length);

	int srv_Sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(!srv_Sock)
		die("Can't create socket");
	if(connect(srv_Sock, (sockaddr*)&srv_Addr, sizeof(srv_Addr)) < 0)
		die("Can't connect");

	SSL_library_init();
	SSL_CTX* ctx = SSL_CTX_new(SSLv23_client_method());
	SSL* ssl = SSL_new(ctx);
	SSL_set_fd(ssl, srv_Sock);
	int ret = SSL_connect(ssl);
	if(SSL_write(ssl, request, strlen(request)) <= 0)
		die("SSL_write error");

	int read = 0;
	ofstream of("page.html", ios::binary);
	while((ret = SSL_read(ssl, buf, buf_Len)))
	{
		cout << "Read: " << ret << " bytes" << endl;
		if(ret <= 0)
			die("SSL_read error");

		of.write(buf, ret);
		read += ret;
	}
	of.close();

	FILE* cert_File = fopen("srv_cert.pem", "w");
	X509* cert = SSL_get_peer_certificate(ssl);
	PEM_write_X509(cert_File, cert);
	fclose(cert_File);

	SSL_shutdown(ssl);
	close(srv_Sock);
	SSL_free(ssl);
	SSL_CTX_free(ctx);

	cout << "Downloading finished" << endl;
	return 0;
}