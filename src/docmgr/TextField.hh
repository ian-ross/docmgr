//----------------------------------------------------------------------
//
//  FILE:   TextField.hh
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2005
//
//----------------------------------------------------------------------
//
//  Editable Curses text field.
//
//----------------------------------------------------------------------

#ifndef _H_TEXTFIELD_
#define _H_TEXTFIELD_

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

#include "EditField.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class TextField : public EditField {
public:

  // TextFields are fixed in position.
  TextField(int x, int y, int w, string &result,
            string word_separators = " ,.;:\"'-!&()");
  virtual ~TextField() { }

  virtual void display(void);
  virtual bool process_key(int ch);
  virtual string result(void) const { return _result; }

  int cursor_pos(void) const { return _first_displayed + _curs_pos; }

protected:

  string &_result;
  int _curs_pos;
  int _first_displayed;
  bool _scrollable;

private:

  string _word_separators;
  int word_motion(int dir);
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
