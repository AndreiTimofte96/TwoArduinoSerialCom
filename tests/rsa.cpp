#include <math.h>
#include <string.h>

#include <iostream>

using namespace std;

int prime1, prime2, primeProd, totem;
int publicKey, privateKey, temp[100];
char text[] = "Ana are mere si!", encrypted[100], decrypted[100];

bool isPrime(int number) {
  int d = 2;
  while (d * d <= number) {
    if (number % d == 0) return 0;
    d++;
  }
  return 1;
}

int computePrivateKey(int publicKey) {
  int k = 1;
  while (1) {
    int s = 1 + k * totem;
    if (s % publicKey == 0) return s / publicKey;
    k++;
  }
}

void computeRSAKeys() {
  for (int index = 2; index < totem; index++) {
    if (totem % index != 0) {
      bool prime = isPrime(index);
      if (prime && index != prime1 && index != prime2) {
        publicKey = index;
        privateKey = computePrivateKey(publicKey);
        if (privateKey > 0) break;
      }
    }
  }
}

void encrypt_decrypt(char* text, char* result, int key) {
  int offset = 0;
  int index;
  for (index = 0; index < strlen(text); index++) {
    int chr = text[index] - offset;
    int pow = 1;
    for (int powIndex = 0; powIndex < key; powIndex++) {
      pow *= chr;
      pow %= primeProd;
    }
    temp[index] = pow;
    result[index] = pow + offset;
  }
  result[index] = -1;
}

int main() {
  prime1 = 11;
  prime2 = 13;
  primeProd = prime1 * prime2;
  totem = (prime1 - 1) * (prime2 - 1);

  computeRSAKeys();
  cout << "\nPublic Key and Private Key\n";
  cout << publicKey << " " << privateKey << '\n';

  encrypt_decrypt(text, encrypted, publicKey);

  cout << "\nTHE ENCRYPTED MESSAGE IS\n";
  for (int index = 0; index < strlen(text); index++) {
    cout << encrypted[index];
  }
  cout << '\n';

  encrypt_decrypt(encrypted, decrypted, privateKey);
  cout << "\nTHE DECRYPTED MESSAGE IS\n";
  for (int index = 0; index < strlen(text); index++) {
    cout << decrypted[index];
  }
  cout << '\n';

  return 0;
}