#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#include "DocMgr.hh"

using namespace DocMgr;


int main(void)
{
  try {
    // Connection tests.

    Connection *conn = new Connection("docmgr_tst");

    DocID max_id = conn->max_doc_id();
    cout << string(max_id) << endl;

    const vector<DocType> &dtypes = conn->doc_types();
    assert(dtypes.size() > 0);
    bool found = false;
    for (int idx = 0; idx < dtypes.size(); ++idx)
      if (string(dtypes[idx]) == "PH") found = true;
    assert(found);
    const vector<FieldType> &ftypes = conn->field_types();
    assert(ftypes.size() > 0);
    found = false;
    for (int idx = 0; idx < ftypes.size(); ++idx)
      if (string(ftypes[idx]) == "PG") found = true;
    assert(found);
    const vector<FieldType> &dfs = conn->doc_fields(DocType(*conn, "AT"));
    assert(dfs.size() > 0);
    found = false;
    for (int idx = 0; idx < dfs.size(); ++idx)
      if (string(dfs[idx]) == "VO") found = true;
    assert(found);

    delete conn;

    cout << "COMPLETED OK" << endl;
  }
  catch (Exception &exc) {
    cout << "UNHANDLED DocMgr EXCEPTION: " << exc.msg() << endl;
  }
}
