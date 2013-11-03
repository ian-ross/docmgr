//----------------------------------------------------------------------
//
//  FILE:   ArticleViewForm.hh
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Concrete class for read-only article views.
//
//----------------------------------------------------------------------

#ifndef _H_ARTICLEVIEWFORM_
#define _H_ARTICLEVIEWFORM_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Local headers.

#include "ArticleForm.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class ArticleViewForm : public ArticleForm {
public:

  // An article form is fixed in position, and permanently associated
  // with a database connection.

  ArticleViewForm(DocMgr::Connection *db, int x, int y, int w, int h) :
    ArticleForm(db, x, y, w, h), _xref_doc(0), _total_rows(0) { }
  virtual ~ArticleViewForm() { }


  // Display/hide the form.
  void display(void);

  // Set the current document ID.
  void set_current(DocMgr::DocID id);

  // Refresh the current document information.
  void refresh(void) { if (_doc) set_current(_doc->id()); }

private:

  DocMgr::DocRecord *_xref_doc;
  bool row_ok(int row);
  int _total_rows;
};

#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
