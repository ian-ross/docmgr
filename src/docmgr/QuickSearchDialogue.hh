//----------------------------------------------------------------------
//
//  FILE:   QuickSearchDialogue.hh
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Search dialogue.
//
//----------------------------------------------------------------------

#ifndef _H_QUICKSEARCHDIALOGUE_
#define _H_QUICKSEARCHDIALOGUE_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>
#include <vector>

using namespace std;


// Local headers.

#include "Widget.hh"
#include "TextField.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class QuickSearchDialogue : public Widget {
public:

  QuickSearchDialogue();
  virtual ~QuickSearchDialogue() { }

  virtual bool process_key(int ch);
  void run(string &result);

private:

  void up_stack(void);
  void down_stack(void);

  bool _completed;

  TextField *_field;
  string _search_str;
  vector<string> _stack;
  int _sp;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
