#include <math.h>
#include <cstring>
#include <iostream>

using namespace std;

char dataSendString[] = "2";

int parityBits = 8;
int const dataSendLength = 16;  // Bytes
int const dataSendBitsLength = parityBits + dataSendLength * 8;

int const dataReceiveBitsLength = dataSendLength * 8;
char dataReceiveString[140];

bool isPowerOfTwo(int n) {
  if (n == 0)
    return false;
  double log2N = log(n) / log(2);
  return (ceil(log2N) == floor(log2N));
}

void initializeArray(int* arr, int length) {
  for (int index = 0; index < length; index++) {
    arr[index] = 0;
  }
}

void baseTwoToChar(int* bits, int length, char* str) {
  int strIndex = 0;
  cout << "base2" << '\n';
  for (int index = 1; index <= length;) {
    int number = 0;
    for (int bitIndex = 7; bitIndex >= 0; bitIndex--) {
      if (bits[index] == 1) {
        cout << index << " " << bitIndex << '\n';
        number += 1 << bitIndex;
      }
      index++;
    }
    cout << "number: ";
    cout << number << '\n';
    str[strIndex++] = number;
  }
  cout << "base2" << '\n';
}

void charToBaseTwo(char* str, int* bits) {
  int bitIndex = 1;
  for (int charIndex = 0; charIndex < strlen(str); charIndex++) {
    char c = str[charIndex];

    //compute current character in base 2 and add it to str
    for (int i = 7; i >= 0; i--, bitIndex++) {
      bits[bitIndex] = (c & (1 << i)) ? 1 : 0;
    }
  }
}

void computeSendData(char* dataSendString, char* dataSendEncodedString) {
  int dataSendBits[140];  // 64 bits  + 7 parityBits

  //fill parity bits position with -1 and the rest with 0
  for (int index = 1; index <= dataSendLength * 8; index++) {
    if (isPowerOfTwo(index)) {
      dataSendBits[index] = -1;
    } else {
      dataSendBits[index] = 0;
    }
  }

  int dataBitsIndex = 1;
  for (int charIndex = 0; charIndex < dataSendLength; charIndex++) {
    char c = dataSendString[charIndex];

    //compute current character in base 2 and add it to dataSendBits
    for (int i = 7; i >= 0; dataBitsIndex++) {
      if (dataSendBits[dataBitsIndex] != -1) {
        dataSendBits[dataBitsIndex] = (c & (1 << i)) ? 1 : 0;
        i--;
      }
    }
  }

  dataSendBits[32] = 0;
  dataSendBits[43] = 0;

  //compute parityBits value by Hamming Distance Code algorithm
  for (int index = 1; index <= dataSendBitsLength; index++) {
    if (dataSendBits[index] == -1) {
      int skipFactor = index - 1;
      int paritySum = 0;
      // cout << index << " ";
      for (int jindex = index + 1; jindex <= dataSendBitsLength;) {
        if (skipFactor) {
          skipFactor--;
          // cout << jindex << " ";
          paritySum += dataSendBits[jindex];
          jindex++;
        } else {
          jindex += index;
          skipFactor = index;
        }
      }
      // cout << "-> " << paritySum << " " << paritySum % 2;
      // cout << '\n';
      dataSendBits[index] = paritySum % 2;
    }
  }

  for (int index = 1; index <= dataSendBitsLength; index++) {
    cout << dataSendBits[index];
  }
  cout << '\n';

  baseTwoToChar(dataSendBits, dataSendBitsLength, dataSendEncodedString);
}

void computeReceiveData(char* dataSendEncodedString, char* dataReceiveString) {
  int dataSendEncodedBits[140];
  int dataReceiveBits[140];

  charToBaseTwo(dataSendEncodedString, dataSendEncodedBits);

  // for (int index = 1; index <= dataSendBitsLength; index++) {
  //   cout << dataSendEncodedBits[index];
  // }
  // cout << '\n';

  // int _wrongBit = 27;
  // dataSendEncodedBits[_wrongBit] = !dataSendEncodedBits[_wrongBit];

  int wrongBit = 0;
  for (int index = 1; index <= dataSendBitsLength; index++) {
    if (isPowerOfTwo(index)) {
      int skipFactor = index - 1;
      int paritySum = 0;
      for (int jindex = index + 1; jindex <= dataSendBitsLength;) {
        if (skipFactor) {
          skipFactor--;
          // cout << jindex << " ";
          paritySum += dataSendEncodedBits[jindex];
          jindex++;
        } else {
          jindex += index;
          skipFactor = index;
        }
      }

      if (paritySum % 2 != dataSendEncodedBits[index]) {
        wrongBit += index;
      }
    }
  }

  if (wrongBit) {
    dataSendEncodedBits[wrongBit] = !dataSendEncodedBits[wrongBit];
  }
  // cout << "WRONG BIT: " << wrongBit << '\n';

  int dataBitsReceiveIndex = 1;
  for (int index = 1; index <= dataSendBitsLength; index++) {
    if (!isPowerOfTwo(index)) {
      dataReceiveBits[dataBitsReceiveIndex++] = dataSendEncodedBits[index];
    }
  }
  baseTwoToChar(dataReceiveBits, dataReceiveBitsLength, dataReceiveString);
}

int main() {
  cout << "COMPUTE SEND DATA: " << '\n';

  char dataSendEncodedString[dataSendBitsLength];
  computeSendData(dataSendString, dataSendEncodedString);
  cout << "ORIGINAL DATA: " << dataSendString << '\n';
  cout << "SENDING DATA: " << dataSendEncodedString << '\n';
  cout << '\n';
  cout << strlen(dataSendEncodedString) << '\n';

  cout << "COMPUTE RECEIVE DATA: " << '\n';
  computeReceiveData(dataSendEncodedString, dataReceiveString);
  cout << "RECEIVED DATA: " << dataReceiveString << '\n';
  cout << strlen(dataReceiveString) << '\n';

  return 0;
}