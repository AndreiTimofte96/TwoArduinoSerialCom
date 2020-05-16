#include <string.h>

#include <iostream>
using namespace std;

struct A {
  int x;
  char d[100];
  int y;
};
char buffer[100];

template <typename T>
void write(T Struct, int structSz) {
  unsigned char* ptr = (unsigned char*)&Struct;

  cout << 00 << " " << sizeof(Struct) << '\n';

  for (int index = 0; index < structSz; index++) {
    buffer[index] = *ptr++;
  }

  cout << 11 << " " << strlen(buffer) << " " << sizeof(buffer) << '\n';
}

template <typename T>
void read(T& Struct, int structSz) {
  cout << buffer << '\n';
  unsigned char* ptr = (unsigned char*)&Struct;

  for (int index = 0; index < structSz; index++) {
    *ptr++ = buffer[index];
  }
}

int main(void) {
  A s1, s2;
  s1.x = 2345;
  s1.y = 7891;
  strcpy(s1.d, "ana are mere! 123456");

  cout << sizeof(s1) << '\n';
  write(s1, sizeof(s1));
  read(s2, sizeof(s2));

  cout << s2.x << " " << s2.y << " " << s2.d << '\n';
  return 0;
}
