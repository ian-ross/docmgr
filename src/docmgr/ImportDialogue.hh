//----------------------------------------------------------------------
//
//  FILE:   ImportDialogue.hh
//  AUTHOR: Ian Ross
//  DATE:   02-OCT-2005
//
//----------------------------------------------------------------------
//
//  Import dialogue.
//
//----------------------------------------------------------------------

#ifndef _H_IMPORTDIALOGUE_
#define _H_IMPORTDIALOGUE_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>
#include <list>

using namespace std;


// Local headers.

#include "DocMgr.hh"
#include "Widget.hh"
#include "TextField.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class ImportDialogue : public Widget {
public:

  ImportDialogue(DocMgr::Connection &db);
  virtual ~ImportDialogue() { }

  virtual bool process_key(int ch);
  void run(string default_import_file);

private:

  void complete_file(string &head);

  DocMgr::Connection &_db;
  bool _completed;
  bool _import;
  string _file_name;
  TextField *_field;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
