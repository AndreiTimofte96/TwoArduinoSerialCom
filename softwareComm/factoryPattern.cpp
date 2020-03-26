#include <iostream>
#include "AFactory.hpp"

using namespace std;

int main(void) {
  // Your code here!
  A* a;
  a = AFactory::set(1);
  a->read();
}

using namespace std;

class A {
 public:
  A() {
    cout << "A CT" << endl;
  }

  virtual void read() {
    cout << "A READ" << endl;
  }
};

// #include "A.hpp"

using namespace std;

class B : public A {
 public:
  B() {
    cout << "B CT" << endl;
  }
  void read() {
    cout << "B READ" << endl;
  }
};

// #include "A.hpp"

using namespace std;

class B : public A {
 public:
  B() {
    cout << "B CT" << endl;
  }
  void read() {
    cout << "B READ" << endl;
  }
};

#include "A.hpp"
#include "B.hpp"
#include "C.hpp"

class AFactory {
 public:
  AFactory() {
    cout << "AFactory CT" << endl;
  }

  static A* set(int n) {
    if (n == 1) {
      return new B;
    }
    return new C;
  }
};
