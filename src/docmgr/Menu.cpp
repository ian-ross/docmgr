//----------------------------------------------------------------------
//
//  FILE:   Menu.cpp
//  AUTHOR: Ian Ross
//  DATE:   21-SEP-2005
//
//----------------------------------------------------------------------
//
//  Menu widget.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------


// Standard headers.

#include <iostream>
#include <cstdlib>
#include <cctype>

using namespace std;

// Local headers.

#include "Menu.hh"
#include "InteractorList.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

// Menu is fixed in position, one row high, and initially empty.

Menu::Menu(int x, int y, int w) :
  Widget(x, y, w, 1), _action(-1)
{ }


// Add a new item to the menu: the label string should contain a
// single ampersand character, which immediately precedes the key to
// be used for the action.

void Menu::add_item(string label, int action)
{
  MenuEntry new_entry;
  string::size_type amp_pos = label.find('&');
  if (amp_pos == string::npos || amp_pos == label.length() - 1) {
    endwin();
    cerr << "Invalid menu label: " << label << endl;
    exit(1);
  }
  new_entry.hilit_pos = amp_pos;
  new_entry.key = tolower(label[amp_pos + 1]);
  new_entry.label = label;
  new_entry.label.erase(amp_pos, 1);
  new_entry.action = action;
  _entries.push_back(new_entry);
}


void Menu::display(void)
{
  clear();
  int total_len = 0;
  for (list<MenuEntry>::iterator it = _entries.begin();
       it != _entries.end(); ++it)
    total_len += it->label.length();
  int entry_spacing = 0;
  if (_entries.size() > 1) {
    int space = _w - total_len;
    entry_spacing = space / (_entries.size() + 1);
    total_len += entry_spacing * (_entries.size() - 1);
  }
  int pos = (_w - total_len) / 2;
  for (list<MenuEntry>::iterator it = _entries.begin();
       it != _entries.end(); ++it) {
    writestr(pos, 0, it->label);
    writestr(pos + it->hilit_pos, 0,
             it->label.substr(it->hilit_pos, 1), A_BOLD);
    pos += it->label.length() + entry_spacing;
  }
  wnoutrefresh(_win);
}


// Run the menu.

bool Menu::process_key(int ch)
{
  bool retval = false;
  for (list<MenuEntry>::iterator it = _entries.begin();
       it != _entries.end(); ++it)
    if (META(it->key) == ch) {
      _action = it->action;
      retval = true;
      break;
    }
  return retval;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
