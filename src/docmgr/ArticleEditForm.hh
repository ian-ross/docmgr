//----------------------------------------------------------------------
//
//  FILE:   ArticleEditForm.hh
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2005
//
//----------------------------------------------------------------------
//
//  Concrete class for editable article views.
//
//----------------------------------------------------------------------

#ifndef _H_ARTICLEEDITFORM_
#define _H_ARTICLEEDITFORM_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <list>

using namespace std;

// Local headers.

#include "ArticleForm.hh"
#include "TextField.hh"
#include "SpinField.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class ArticleEditForm : public ArticleForm {
public:

  // An article form is fixed in position, and permanently associated
  // with a database connection.

  ArticleEditForm(DocMgr::Connection *db, int x, int y, int w, int h) :
    ArticleForm(db, x, y, w, h), _total_rows(0) { }
  virtual ~ArticleEditForm() { }


  // Run form to add a new database entry, returning the new document
  // ID.
  DocMgr::DocID add_entry(DocMgr::DocType type);

  // Run the form to edit an existing document.
  void edit(DocMgr::DocID doc_id);


  // Process a single key event.
  virtual bool process_key(int ch);

  // Display/hide the form.
  void display(void);

private:

  DocMgr::DocID run(void);

  bool row_ok(int row);
  int _total_rows;
  int _header_width;
  int _contents_space;
  bool _new_entry;

  list<EditField *> _field_list;
  list<EditField *>::iterator _curr_field;

  SpinField *_holding_field;
  SpinField *_status_field;
  map<DocMgr::FieldType, EditField *> _edit_fields;
  map<DocMgr::FieldType, bool> _multiline_fields;
};

#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
