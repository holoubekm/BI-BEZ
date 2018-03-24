
all: lab06.cpp
	g++ -g -Wall -pedantic lab06.cpp -lssl -lcrypto -o lab06

run: all
	./lab06

cert: 
	openssl s_client -host fit.cvut.cz -port 443 -showcerts
	
cert2:
	openssl x509 -in srv_cert.pem -text