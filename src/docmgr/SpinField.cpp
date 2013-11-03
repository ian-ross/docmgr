//----------------------------------------------------------------------
//
//  FILE:   SpinField.cpp
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2005
//
//----------------------------------------------------------------------
//
//  Curses spin field.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>

using namespace std;


// Local headers.

#include "SpinField.hh"
#include "InteractorList.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

SpinField::SpinField(int x, int y, int w,
                     const vector<string> &options, string initial) :
  EditField(x, y, w), _options(options), _curr_idx(0)
{
  for (int idx = 0; idx < _options.size(); ++idx)
    if (_options[idx] == initial) { _curr_idx = idx;  break; }
}


bool SpinField::process_key(int ch)
{
  bool retval = false;
  switch(ch) {
  case KEY_LEFT:
  case CTRL('B'):
    // Process leftwards motion.
    _curr_idx = (_curr_idx - 1 + _options.size()) % _options.size();
    retval = true;
    break;

    case KEY_RIGHT:
    case CTRL('F'):
      // Process rightwards motion.
      _curr_idx = (_curr_idx + 1) % _options.size();
      retval = true;
      break;
  }

  if (retval) display();
  return retval;
}


void SpinField::display(void)
{
  clear();
  writestr(0, 0, _options[_curr_idx]);
  if (_focus) wmove(_win, 0, 0);
  wnoutrefresh(_win);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
