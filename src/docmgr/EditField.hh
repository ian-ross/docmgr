//----------------------------------------------------------------------
//
//  FILE:   EditField.hh
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2005
//
//----------------------------------------------------------------------
//
//  Editable Curses field.
//
//----------------------------------------------------------------------

#ifndef _H_EDITFIELD_
#define _H_EDITFIELD_

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

class EditField : public Widget {
public:

  // EditFields are fixed in position.
  EditField(int x, int y, int w);
  virtual ~EditField() { delwin(_win); }

  void move(int dy);
  void focus(bool foc) { _focus = foc; }
  virtual void display(void) = 0;
  virtual void display(int lines) { display(); }
  virtual bool process_key(int ch) = 0;
  virtual string result(void) const = 0;

  virtual int lines(void) const { return 1; }

protected:

  bool _focus;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
