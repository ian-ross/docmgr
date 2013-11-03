//----------------------------------------------------------------------
//
//  FILE:   IDList.hh
//  AUTHOR: Ian Ross
//  DATE:   21-SEP-2005
//
//----------------------------------------------------------------------
//
//  Header file for document ID list widget derived from CDK scrolling
//  list widget.
//
//----------------------------------------------------------------------

#ifndef _H_IDLIST_
#define _H_IDLIST_

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


// DocMgr library header.

#include "DocMgr.hh"


// Local headers.

#include "Widget.hh"
class ArticleViewForm;


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class IDList : public Widget {
public:

  // ID list is fixed in position, and permanently associated with a
  // database connection and an article view form.

  IDList(DocMgr::Connection *db, ArticleViewForm &view_form,
         int x, int y, int w, int h);
  virtual ~IDList() { }


  // Process a single key event.
  virtual bool process_key(int ch);

  // Display the ID list.
  virtual void display(void);

  // Reload the ID list from the database and redisplay.
  void refresh(void);

  // Get the currently highlighted document ID.
  DocMgr::DocID current(void) const;

  // Set the currently highlighted document ID.
  void set_current(DocMgr::DocID id);

  // Return entry count.
  int entries(void) const { return _ids.size(); }

  // Query management.
  const DocMgr::Query *query(void) const { return _id_query; }
  void set_query(DocMgr::Query &query);
  void clear_query(void) { DocMgr::Query tmp(*_db);  set_query(tmp); }

  // Deletion management.
  void toggle_deleted(DocMgr::DocID id);

  // Utility to determine whether or not a given ID is currently
  // visible.
  bool id_visible(DocMgr::DocID id);

private:

  void fill_id_list(void);
  bool id_deleted(DocMgr::DocID id);

  DocMgr::Connection *_db;
  DocMgr::Query *_id_query;
  ArticleViewForm &_view_form;
  vector<DocMgr::DocID> _ids;
  vector<DocMgr::DocID> _deleted_ids;
  int _top_vis;
  int _curr_loc;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
