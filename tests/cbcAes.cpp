#include <openssl/conf.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *aes_key = (unsigned char *)"01234567890123456789012345678901";

char *file_data;
char *encrypted;
char *decrypted;

void AES() {
}

void encryptAES_CBC(char *data, char *result) {
  char *toEncrypt, *cypherText, *_iv;

  toEncrypt = (char *)malloc(sizeof(char) * 17);
  memset(toEncrypt, '\0', sizeof(char) * 17);
  cypherText = (char *)malloc(sizeof(char) * 17);
  memset(cypherText, '\0', sizeof(char) * 17);

  _iv = (char *)malloc(sizeof(char) * 17);
  strcpy(_iv, "0123456789012345");

  printf("%d\n", strlen(data));
  int numberOfBlocks = strlen(data) / 16;

  if (strlen(data) % 16 != 0) {
    numberOfBlocks++;
  }

  for (int index = 0; index < numberOfBlocks; index++) {
    memset(toEncrypt, '\0', sizeof(char) * 17);
    int k = 0;
    for (int encryptIndex = index * 16; encryptIndex < index * 16 + 16; encryptIndex++) {
      toEncrypt[k++] = data[encryptIndex];
    }  // formam blocurile de cate 16

    for (int i = 0; i < 16; i++) {
      cypherText[i] = (_iv[i] ^ toEncrypt[i]) % 127;
    }

    printf("cyptherText: %s\n", cypherText);
    strcat(result, cypherText);
    strcpy(_iv, cypherText);
  }
}

void decryptAES_CBC(char *data, char *result) {
  char *toEncrypt, *cypherText, *_iv;

  toEncrypt = (char *)malloc(sizeof(char) * 17);
  memset(toEncrypt, '\0', sizeof(char) * 17);
  cypherText = (char *)malloc(sizeof(char) * 17);
  memset(cypherText, '\0', sizeof(char) * 17);

  _iv = (char *)malloc(sizeof(char) * 17);
  strcpy(_iv, "0123456789012345");

  int numberOfBlocks = strlen(data) / 16;
  if (strlen(data) % 16 != 0) {
    numberOfBlocks++;
  }

  for (int index = 0; index < numberOfBlocks; index++) {
    memset(toEncrypt, '\0', sizeof(char) * 17);
    int k = 0;
    for (int encryptIndex = index * 16; encryptIndex < index * 16 + 16; encryptIndex++) {
      toEncrypt[k++] = data[encryptIndex];
    }  // formam blocurile de cate 16

    for (int i = 0; i < 16; i++) {
      cypherText[i] = (_iv[i] ^ toEncrypt[i]) % 127;
    }
    strcat(result, cypherText);
    strcpy(_iv, toEncrypt);
  }
  result[strlen(file_data)] = '\0';
}

int main() {
  int length = 50;
  file_data = (char *)malloc(sizeof(char) * length);
  memset(file_data, '\0', sizeof(char) * length);
  encrypted = (char *)malloc(sizeof(char) * length);
  memset(encrypted, '\0', sizeof(char) * length);
  decrypted = (char *)malloc(sizeof(char) * length);
  memset(decrypted, '\0', sizeof(char) * length);

  strcpy(file_data, "abcdefghiklmnopr");
  printf("\nFILE_DATA: %s %d\n", file_data, strlen(file_data));

  encryptAES_CBC(file_data, encrypted);
  printf("\nENCRYPTED: %s %d\n", encrypted, strlen(encrypted));

  decryptAES_CBC(encrypted, decrypted);
  printf("\nDECRYPTED: %s\n", decrypted);
  return 0;
}