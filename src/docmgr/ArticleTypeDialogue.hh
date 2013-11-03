//----------------------------------------------------------------------
//
//  FILE:   ArticleTypeDialogue.hh
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Article type selection dialogue.
//
//----------------------------------------------------------------------

#ifndef _H_ARTICLETYPEDIALOGUE_
#define _H_ARTICLETYPEDIALOGUE_

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
#include "Widget.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class ArticleTypeDialogue : public Widget {
public:

  ArticleTypeDialogue(DocMgr::Connection &db);
  virtual ~ArticleTypeDialogue() { }

  virtual void display(void);

  virtual bool process_key(int ch);
  bool run(DocMgr::DocType &result);

private:

  DocMgr::Connection &_db;
  int _curr_row;
  bool _completed;
  bool _save;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
