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

    DocRecord *doc = conn->get_doc_by_id(DocID("000007"));
    cout << *doc << endl;
    delete doc;

    doc = conn->get_doc_by_id(DocID(161));
    cout << *doc << endl;
    delete doc;

    doc = conn->get_doc_by_id(DocID("000300"));
    cout << *doc << endl;
    delete doc;

    try {
      doc = conn->get_doc_by_id(DocID("554"));
      cout << *doc << endl;
      delete doc;
    } catch (Exception &exc) {
      if (exc.type() != Exception::DOCID_NOT_FOUND) throw;
    }

    delete conn;

    cout << "COMPLETED OK" << endl;
  }
  catch (Exception &exc) {
    cout << "UNHANDLED DocMgr EXCEPTION: " << exc.msg() << endl;
  }
}
