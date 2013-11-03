//----------------------------------------------------------------------
//
//  FILE:   ExportDialogue.hh
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2008
//
//----------------------------------------------------------------------
//
//  Export document dialogue.
//
//----------------------------------------------------------------------

#ifndef _H_EXPORTDIALOGUE_
#define _H_EXPORTDIALOGUE_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>

using namespace std;


// Local headers.

#include "Widget.hh"
#include "IDList.hh"
#include "TextField.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class ExportDialogue : public Widget {
public:

  ExportDialogue(DocMgr::Connection &db);
  virtual ~ExportDialogue() { }

  virtual bool process_key(int ch);
  bool run(DocMgr::DocID curr_doc);

private:

  string format_current_doc(DocMgr::DocRecord *doc);
  string format_author(string auth);

  bool _completed;

  DocMgr::Connection &_db;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
