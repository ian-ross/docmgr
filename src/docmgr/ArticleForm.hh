//----------------------------------------------------------------------
//
//  FILE:   ArticleForm.hh
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Abstract base class for both read-only and editable article views.
//
//----------------------------------------------------------------------

#ifndef _H_ARTICLEFORM_
#define _H_ARTICLEFORM_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <vector>

using namespace std;


// Unix headers.

#include <curses.h>


// Local headers.

#include "DocMgr.hh"
#include "Widget.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class ArticleForm : public Widget {
public:

  struct FieldDesc {
    const char *id;
    const char *name;
  };


  // An article form is fixed in position, and permanently associated
  // with a database connection.

  ArticleForm(DocMgr::Connection *db, int x, int y, int w, int h);
  virtual ~ArticleForm() { delete _doc; }


protected:

  virtual void frame(void) { frame(true); }
  virtual void frame(bool display_holding);

  int field_header_width(bool present_only);

  static const FieldDesc _field_order[];
  static int field_order(DocMgr::FieldType type);
  static string field_name(DocMgr::FieldType type);

  DocMgr::Connection *_db;
  DocMgr::DocRecord *_doc;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
