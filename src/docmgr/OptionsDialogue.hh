//----------------------------------------------------------------------
//
//  FILE:   OptionsDialogue.hh
//  AUTHOR: Ian Ross
//  DATE:   02-OCT-2005
//
//----------------------------------------------------------------------
//
//  Options dialogue.
//
//----------------------------------------------------------------------

#ifndef _H_OPTIONSDIALOGUE_
#define _H_OPTIONSDIALOGUE_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>
#include <list>

using namespace std;


// Local headers.

#include "Configuration.hh"
#include "Widget.hh"
#include "TextField.hh"
#include "SpinField.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class OptionsDialogue : public Widget {
public:

  OptionsDialogue(Configuration *config);
  virtual ~OptionsDialogue() { }

  virtual bool process_key(int ch);
  void run(void);

private:

  Configuration *_config;
  bool _completed;
  bool _save;

  list<EditField *> _field_list;
  list<EditField *>::iterator _curr_field;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
