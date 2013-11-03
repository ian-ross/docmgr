#include <iostream>

using namespace std;

#include "DocMgr.hh"

using namespace DocMgr;

#include "Filter.hh"

int main(void)
{
  try {
    Connection *conn = new Connection("docmgr");

    Filter filt1(*conn);
    filt1.set_name("Test Filter #1");
    filt1.set_definition("AU=Cox");
    Query q1 = filt1.query();
    string s1(q1);
    cout << s1 << endl;
    vector<DocID> results1;
    q1.run(results1);
    assert(results1.size() > 0);
    for (int idx = 0; idx < results1.size(); ++idx)
      cout << results1[idx] << "  ";
    cout << endl;

    Filter filt2(*conn);
    filt2.set_name("Test Filter #2");
    filt2.set_definition("AU=Cox & (YR=2005 | YR=2004 | YR=2003)");
    Query q2 = filt2.query();
    string s2(q2);
    cout << s2 << endl;
    vector<DocID> results2;
    q2.run(results2);
    assert(results2.size() > 0);
    for (int idx = 0; idx < results2.size(); ++idx)
      cout << results2[idx] << "  ";
    cout << endl;

    Filter filt3(*conn);
    filt3.set_name("Test Filter #3");
    try {
      filt3.set_definition("AU=Cox & &");
    } catch (Filter::Exception &exc) {
      cout << "GOOD CATCH: " << exc.msg() << endl;
    }
  } catch (Exception &exc) {
    cout << "EXCEPTION: " << exc.msg() << endl;
  }
}
