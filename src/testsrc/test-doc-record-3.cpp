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

    Connection *conn = new Connection("docmgr_tst");

    DocRecord *doc = conn->get_doc_by_id(DocID(4));
    doc->set_field(FieldType(*conn, "AU"), "Billy Bragg");
    doc->set_field(FieldType(*conn, "MO"), "December");
    doc->set_field(FieldType(*conn, "PG"), "");
    doc->update();
    delete doc;

    delete conn;

    cout << "COMPLETED OK" << endl;
  }
  catch (Exception &exc) {
    cout << "UNHANDLED DocMgr EXCEPTION: " << exc.msg() << endl;
  }
}
