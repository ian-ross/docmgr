//----------------------------------------------------------------------
//
//  FILE:   FilterCombinationDialogue.hh
//  AUTHOR: Ian Ross
//  DATE:   08-OCT-2005
//
//----------------------------------------------------------------------
//
//  Filter combination dialogue.
//
//----------------------------------------------------------------------

#ifndef _H_FILTERCOMBINATIONDIALOGUE_
#define _H_FILTERCOMBINATIONDIALOGUE_

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

#include "Widget.hh"
#include "TextField.hh"
#include "SpinField.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class FilterCombinationDialogue : public Widget {
public:

  FilterCombinationDialogue();
  virtual ~FilterCombinationDialogue() { }

  virtual bool process_key(int ch);
  bool run(int &no1, int &no2, int max_no, bool &and_mode);

private:

  bool _completed;
  bool _save;

  list<EditField *> _field_list;
  list<EditField *>::iterator _curr_field;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
