# BI-BEZ Course v2014
## Bezpečnost
### CTU / ČVUT FIT

### Bonuses
#### Bonus 1
The first bonus task was to show how the birthday paradox works. You can select number of iterations the program will use.

```bash
cd bonus_1
make && ./paradox 10000
```

#### Bonus 2
The second bonus was about attacking on a stream cipher. It simply takes two encrypted strings moves one of them along and tries to stop when readable word is detected.

```bash
make && ./stream
```

### Labs
#### Lab 2
The first part located in `ukol1.cpp` tries to find SHA256 collision in the first 2 bytes. The input clear text is systematically generated and output is matched against two hardcoded values. 

The second task shown, that stream cipher applied twice with the same config has no security at all. It was easy to `xor` those two outputs and effiently get unencrypted output.

Basic idea:

```
in_1 = "......."
in_2 = "......."
cipher_stream = RC4_GetStream();
#The same stream used twice
out_1 = bytewise_xor(in_1, cipher_stream)
out_2 = bytewise_xor(in_2, cipher_stream)
#We know this, because `xor` will remove the cipher_stream
#Now there is not secrecy
bytewise_xor(in_1, in_2) == bytewise_xor(out_1, out_2)

```

#### Lab 3
This lab show different properties of block ciphers in different set-ups. The `bmp` image of Homer Simpson was disected and split into image header and data. Image data was encrypted and the results merged. Now we have different images encrypted with different setups. Given this we can easily (**okometricky**) compare the quality of encryption.

Encryption combinations
```
CBC-ENCRYPTION
CBC-DECRYPTION
ECB-ENCRYPTION
ECB-DECRYPTION
```

#### Lab 4
Is about assymetric cryptography. You can encrypt and decrypt file using `private` and `public` keys.

#### Lab 5
Program opens SSL connection to a hardcoded webserver and downloads it's certificate to the local `pem` file.

#### Lab 6
Program opens SSL connection to a hardcoded webserver and lists server capabilities concerning security suits and configs.
