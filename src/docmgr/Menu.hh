//----------------------------------------------------------------------
//
//  FILE:   Menu.hh
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Header file for simple Curses menu class.
//
//----------------------------------------------------------------------

#ifndef _H_MENU_
#define _H_MENU_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <list>
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

class Menu : public Widget {
public:

  // Menu is fixed in position.

  Menu(int x, int y, int w);
  ~Menu() { }


  // Process a single key event.
  virtual bool process_key(int ch);

  // Display/hide the menu.
  void display(void);

  // Add a new item to the menu.
  void add_item(string label, int action);

  // Get last menu action.
  int action(void) const { return _action; }

private:

  struct MenuEntry {
    string label;
    int hilit_pos;
    int key;
    int action;
  };

  list<MenuEntry> _entries;
  int _action;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
