//----------------------------------------------------------------------
//
//  FILE:   ConfirmDialogue.hh
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Search dialogue.
//
//----------------------------------------------------------------------

#ifndef _H_CONFIRMDIALOGUE_
#define _H_CONFIRMDIALOGUE_

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


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class ConfirmDialogue : public Widget {
public:

  ConfirmDialogue(bool tough = true);
  virtual ~ConfirmDialogue() { }

  virtual bool process_key(int ch);
  bool run(string message);

private:

  bool _tough;
  bool _completed;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
