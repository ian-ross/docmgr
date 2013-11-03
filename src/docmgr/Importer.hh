//----------------------------------------------------------------------
//
//  FILE:   Importer.hh
//  AUTHOR: Ian Ross
//  DATE:   02-OCT-2005
//
//----------------------------------------------------------------------
//
//  ISI format article entry importer.
//
//----------------------------------------------------------------------

#ifndef _H_IMPORTER_
#define _H_IMPORTER_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>

using namespace std;

// Local headers.

#include "DocMgr.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class Importer {
public:

  Importer(DocMgr::Connection &db, string file_name);
  ~Importer();

  bool valid(void) const { return _valid; }
  string reason(void) const { return _failure_reason; }

  vector<DocMgr::DocRecord *> &docs(void) { return _docs; }


private:

  string _file_name;
  bool _valid;
  string _failure_reason;
  vector<DocMgr::DocRecord *> _docs;
};

#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
