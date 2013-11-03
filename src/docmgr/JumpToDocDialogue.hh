//----------------------------------------------------------------------
//
//  FILE:   JumpToDocDialogue.hh
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Jump to numbered document dialogue.
//
//----------------------------------------------------------------------

#ifndef _H_JUMPTODOCDIALOGUE_
#define _H_JUMPTODOCDIALOGUE_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>

using namespace std;


// Local headers.

#include "Widget.hh"
#include "IDList.hh"
#include "TextField.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class JumpToDocDialogue : public Widget {
public:

  JumpToDocDialogue();
  virtual ~JumpToDocDialogue() { }

  virtual bool process_key(int ch);
  bool run(DocMgr::Connection &db, IDList *id_list);

private:

  void up_stack(void);
  void down_stack(void);

  bool _completed;

  TextField *_field;
  string _id_str;
  vector<string> _stack;
  int _sp;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
