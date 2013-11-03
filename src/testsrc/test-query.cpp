#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#include "DocMgr.hh"

using namespace DocMgr;


int main(void)
{
  try {
    // Query tests.

    Connection *conn = new Connection("docmgr");

    Query q1(*conn, FieldType(*conn, "AU"), "Cox");
    string s1(q1);
    cout << s1 << endl;
    vector<DocID> results1;
    q1.run(results1);
    assert(results1.size() > 0);
    for (int idx = 0; idx < results1.size(); ++idx)
      cout << results1[idx] << "  ";
    cout << endl;

    Query q2s(*conn, FieldType(*conn, "YR"), "2003");
    Query q2 = q1 && q2s;
    string s2(q2);
    cout << s2 << endl;
    vector<DocID> results2;
    q2.run(results2);
    assert(results2.size() > 0);
    for (int idx = 0; idx < results2.size(); ++idx)
      cout << results2[idx] << "  ";
    cout << endl;

    Query q3(*conn);
    string s3(q3);
    cout << s3 << endl;
    vector<DocID> results3;
    q3.run(results3);
    assert(results3.size() > 0);
    for (int idx = 0; idx < results3.size(); ++idx)
      cout << results3[idx] << "  ";
    cout << endl;

    Query q4(*conn, "Cox carbon");
    string s4(q4);
    cout << s4 << endl;
    vector<DocID> results4;
    q4.run(results4);
    assert(results4.size() > 0);
    for (int idx = 0; idx < results4.size(); ++idx)
      cout << results4[idx] << "  ";
    cout << endl;

    delete conn;

    cout << "COMPLETED OK" << endl;
  }
  catch (Exception &exc) {
    cout << "UNHANDLED DocMgr EXCEPTION: " << exc.msg() << endl;
  }
}
