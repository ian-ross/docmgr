//----------------------------------------------------------------------
//
//  FILE:   TopLine.hh
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Header file for document manager status line.
//
//----------------------------------------------------------------------

#ifndef _H_TOPLINE_
#define _H_TOPLINE_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>

using namespace std;


// Unix headers.

#include <curses.h>


// Local headers.

#include "Widget.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class TopLine : public Widget {
public:

  // Top line is fixed in position.

  TopLine();
  ~TopLine() { }


  // Display/hide the top line.
  void display(void);

  // Set the filter name.
  void set_filter_name(string filter_name)
  { _filter_name = filter_name;  display(); }

  // Set the number of entries.
  void set_entry_count(int entry_count)
  { _entry_count = entry_count;  display(); }


private:

  string _filter_name;
  int _entry_count;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
