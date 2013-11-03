//----------------------------------------------------------------------
//
//  FILE:   FilterList.hh
//  AUTHOR: Ian Ross
//  DATE:   07-OCT-2005
//
//----------------------------------------------------------------------
//
//  Filter list.
//
//----------------------------------------------------------------------

#ifndef _H_FILTERLIST_
#define _H_FILTERLIST_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>
#include <vector>

using namespace std;


// Local headers.

#include "Widget.hh"
#include "Filter.hh"
#include "Configuration.hh"
#include "Menu.hh"
#include "TextField.hh"
#include "SpinField.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class FilterList : public Widget {
public:

  // The filter list is fixed in position, and permanently associated
  // with a database connection and a configuration file object.

  FilterList(DocMgr::Connection *db, Configuration *config,
             int x, int y, int w, int h);
  virtual ~FilterList();


  // Process a single key event.
  virtual bool process_key(int ch);

  // Display/hide the form.
  void display(void);

  Filter *current_filter(void) const;

  void add_filter(void);
  void edit_current(void);
  void delete_current(bool delete_from_config = true);
  void combine_filters(void);

  void refresh(void);

private:

  bool run_edit(string &new_name, string &new_def,
                string &new_holding, string &new_status);
  void header_line(string msg = "");

  DocMgr::Connection &_db;
  Configuration *_config;
  vector<Filter *> _filters;
  int _current_filter_idx;
  vector<int> _article_counts;
  vector<int> _display_rows;
  int _top_idx_vis, _bot_idx_vis;
  int _top_row_vis;
  Menu _editing_menu;
  bool _editing;
  TextField *_name_field;
  TextField *_def_field;
  SpinField *_holding_field;
  SpinField *_status_field;
  EditField *_curr_field;
  int _main_x, _main_width, _h_s_x;
};

#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
