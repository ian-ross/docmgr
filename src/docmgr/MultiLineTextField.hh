//----------------------------------------------------------------------
//
//  FILE:   MultiLineTextField.hh
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2005
//
//----------------------------------------------------------------------
//
//  Editable Curses text field.
//
//----------------------------------------------------------------------

#ifndef _H_MULTILINETEXTFIELD_
#define _H_MULTILINETEXTFIELD_

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

class ArticleEditForm;

class MultiLineTextField : public EditField {
public:

  // MultiLineTextFields are fixed in position.
  MultiLineTextField(int x, int y, int w, string &result,
                     string word_separators = " ,.;:\"'-!&()");
  virtual ~MultiLineTextField() { }

  virtual void display(void) { display(_nlines); }
  virtual void display(int lines);
  virtual bool process_key(int ch);
  virtual string result(void) const { return _result; }

  virtual int lines(void) const { return _nlines; }

protected:

  string &_result;
  int _curs_pos;

private:

  string _word_separators;
  int word_motion(int dir);

  int _nlines, _displayed_lines;
};

#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
