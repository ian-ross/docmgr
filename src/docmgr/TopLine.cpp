//----------------------------------------------------------------------
//
//  FILE:   TopLine.cpp
//  AUTHOR: Ian Ross
//  DATE:   21-SEP-2005
//
//----------------------------------------------------------------------
//
//  Document manager status line.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Local headers.

#include "TopLine.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

// TopLine is fixed in position, one row high.

TopLine::TopLine() :
  Widget(0, 0, 0, 1), _filter_name("n/a"), _entry_count(0)
{ }


void TopLine::display(void)
{
  clear();
  writestr(1, 0, string("Document filter: ") + _filter_name);
  char buff[32];
  sprintf(buff, "%d", _entry_count);
  string entries = string(buff) + " entries";
  writestr(_w - entries.length() - 1, 0, entries);
  wnoutrefresh(_win);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------