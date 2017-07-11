#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#include "DocMgr.hh"

using namespace DocMgr;


int main(void)
{
  try {
    // DocID tests.

    assert(DocID::valid("1"));
    assert(DocID::valid("001"));
    assert(DocID::valid("000001"));
    assert(!DocID::valid("0000001"));
    assert(DocID::valid("25"));
    assert(DocID::valid("0025"));
    assert(DocID::valid("000025"));
    assert(!DocID::valid("0000025"));
    assert(DocID::valid("123456"));
    assert(!DocID::valid("XXX"));
    assert(DocID::valid(1));
    assert(DocID::valid(157));
    assert(DocID::valid(999999));
    assert(!DocID::valid(1000000));
    DocID i1;
    DocID i2(1);
    DocID i2p("000001");
    DocID i2pp(i2);
    assert(!i1.valid());
    assert(i2.valid());
    assert(i2p.valid());
    assert(i2pp.valid());
    assert(i2 == i2p);
    assert(i2 == i2pp);
    DocID i3;
    i3 = i2;
    assert(i2 == i3);
    DocID i4;
    i4 = 123;
    assert(i4 > i1);
    assert(i4 != i1);
    assert(i4 >= i1);
    assert(!(i4 < i1));


    // FieldType tests.



    // Holding tests.

    const char *holdings[] = { "E", "P", "EP", "PE", "B", "L", "-" };
    for (int idx = 0; idx < sizeof(holdings) / sizeof(const char *); ++idx)
      assert(Holding::valid(holdings[idx]));
    assert(!Holding::valid("XX"));
    Holding h1;
    Holding h1p(h1);
    assert(!h1.valid());
    assert(!h1p.valid());
    Holding h2("EP");
    Holding h2p(h2);
    assert(h2.valid());
    assert(h2p.valid());
    try {
      Holding h3("XX");
      assert(false);
    } catch (Exception &exc) {
      assert(exc.type() == Exception::INVALID_HOLDING);
    }


    // Status tests.

    const char *statuses[] = { "R", "W", "N", "!", "+", "-" };
    for (int idx = 0; idx < sizeof(statuses) / sizeof(const char *); ++idx)
      assert(Status::valid(statuses[idx]));
    assert(!Status::valid("XX"));
    Status s1;
    Status s1p(s1);
    assert(!s1.valid());
    assert(!s1p.valid());
    Status s2("R");
    Status s2p(s2);
    assert(s2.valid());
    assert(s2p.valid());
    try {
      Status s3("XX");
      assert(false);
    } catch (Exception &exc) {
      assert(exc.type() == Exception::INVALID_STATUS);
    }

    cout << "COMPLETED OK" << endl;
  }
  catch (Exception &exc) {
    cout << "UNHANDLED DocMgr EXCEPTION: " << exc.msg() << endl;
  }
}
