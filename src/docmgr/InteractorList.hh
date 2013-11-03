//----------------------------------------------------------------------
//
//  FILE:   InteractorList.hh
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Class for managing keyboard interaction widgets.
//
//----------------------------------------------------------------------

#ifndef _H_INTERACTORLIST_
#define _H_INTERACTORLIST_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <list>

using namespace std;


// Local headers.

#include "Widget.hh"


//----------------------------------------------------------------------
//
//  CONSTANT DEFINITIONS
//
//----------------------------------------------------------------------

// Some extra key codes beyond the ones provided by Curses.

#define KEY_ESCAPE     0033

#define KEY_CTRL_UP    01000
#define KEY_CTRL_DOWN  01001
#define KEY_CTRL_RIGHT 01002
#define KEY_CTRL_LEFT  01003

#define KEY_CTRL_PGUP  01004
#define KEY_CTRL_PGDN  01005

#define KEY_TAB CTRL('I')
#define KEY_SHIFT_TAB  01006


//----------------------------------------------------------------------
//
//  MACRO DEFINITIONS
//
//----------------------------------------------------------------------

// Control key sequences.

#define CTRL(c) ((c) & 0x1F)

// Meta key sequences.

#define META(c) (((c) & 0x7F) | 0x80)


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class InteractorList {
public:

  struct Resize { int x; };

  InteractorList() { }
  ~InteractorList() { }

  void clear(void) { _interactor_list.clear(); }
  void push_back(Widget &inter) { _interactor_list.push_back(&inter); }

  void process_key(void);

  // Do some nasty key-code processing.
  static int get_char(void);

private:

  list<Widget *> _interactor_list;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
