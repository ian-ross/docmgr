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

    DocRecord *doc = new DocRecord(*conn, DocType(*conn, "AT"));
    doc->set_field(FieldType(*conn, "AU"), "J\\\"{u}rgen Brinks");
    doc->set_field(FieldType(*conn, "TI"),
                   "How to measure \\mathrm{CH_4}");
    doc->set_field(FieldType(*conn, "JN"), "Kippers Weekly");
    doc->set_field(FieldType(*conn, "VO"), "23");
    doc->set_field(FieldType(*conn, "PG"), "10-20");
    doc->set_field(FieldType(*conn, "YR"), "2005");
    doc->intern();
    cout << "Document interned as " << doc->id() << endl;
    delete doc;

    delete conn;

    cout << "COMPLETED OK" << endl;
  }
  catch (Exception &exc) {
    cout << "UNHANDLED DocMgr EXCEPTION: " << exc.msg() << endl;
  }
}
