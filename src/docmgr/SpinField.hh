//----------------------------------------------------------------------
//
//  FILE:   SpinField.hh
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2005
//
//----------------------------------------------------------------------
//
//  Curses spin field.
//
//----------------------------------------------------------------------

#ifndef _H_SPINFIELD_
#define _H_SPINFIELD_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>
#include <vector>

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

class SpinField : public EditField {
public:

  // SpinFields are fixed in position.
  SpinField(int x, int y, int w,
            const vector<string> &options, string initial);
  virtual ~SpinField() { }

  virtual void display(void);
  virtual bool process_key(int ch);
  virtual string result(void) const { return _options[_curr_idx]; }

protected:

  vector<string> _options;
  int _curr_idx;
};

#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
