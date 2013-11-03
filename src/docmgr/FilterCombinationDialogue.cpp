//----------------------------------------------------------------------
//
//  FILE:   FilterCombinationDialogue.cpp
//  AUTHOR: Ian Ross
//  DATE:   08-OCT-2005
//
//----------------------------------------------------------------------
//
//  Filter combination dialogue.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <cstdlib>


// Local headers.

#include "FilterCombinationDialogue.hh"
#include "InteractorList.hh"
#include "TextField.hh"


//----------------------------------------------------------------------
//
//  CONSTANT DEFINITIONS
//
//----------------------------------------------------------------------

const int DIALOGUE_WIDTH = 24;
const int DIALOGUE_HEIGHT = 10;


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

FilterCombinationDialogue::FilterCombinationDialogue() :
  Widget((getmaxx(stdscr) - DIALOGUE_WIDTH) / 2,
         (getmaxy(stdscr) - DIALOGUE_HEIGHT) / 2,
         DIALOGUE_WIDTH, DIALOGUE_HEIGHT)
{ }


bool FilterCombinationDialogue::process_key(int ch)
{
  bool retval = false;
  switch(ch) {
  case KEY_TAB:
  case KEY_DOWN:
    // Go to next field in form.
    (*_curr_field)->focus(false);
    ++_curr_field;
    if (_curr_field == _field_list.end())
      _curr_field = _field_list.begin();
    (*_curr_field)->focus(true);
    retval = true;
    break;

  case KEY_SHIFT_TAB:
  case KEY_UP:
    // Go to previous field in form.
    (*_curr_field)->focus(false);
    if (_curr_field == _field_list.begin())
      _curr_field = _field_list.end();
    --_curr_field;
    (*_curr_field)->focus(true);
    retval = true;
    break;

  case META('s'):
    _completed = true;
    _save = true;
    break;

  case META('a'):
    _completed = true;
    _save = false;
    break;
  }
  return retval;
}


bool FilterCombinationDialogue::run(int &no1, int &no2, int max_no,
                                    bool &and_mode)
{
  clear();
  frame();
  string title = "Filter Combination";
  writestr((_w - title.size()) / 2, 0, title);
  draw_hline(_h - 3);
  mvwaddch(_win, _h - 3, 0, ACS_LTEE);
  mvwaddch(_win, _h - 3, _w - 1, ACS_RTEE);
  string menu_str = "Save  Abandon";
  writestr((_w - menu_str.size()) / 2, _h - 2, menu_str);
  writestr((_w - menu_str.size()) / 2, _h - 2, "S", A_BOLD);
  writestr((_w - menu_str.size()) / 2 + 6, _h - 2, "A", A_BOLD);

  vector<string> nos;
  char buff[32];
  for (int no = 1; no <= max_no; ++no) {
    sprintf(buff, "%d", no);
    nos.push_back(buff);
  }

  vector<string> modes;
  modes.push_back("AND");
  modes.push_back("OR");

  int row = 2, header_width = 15;
  int field_x = _x + header_width + 3;
  writestr(3, row, "First filter:"); 
  SpinField no1_field(field_x, _y + row++, 3, nos, "1");
  writestr(3, row, "Second filter:");
  SpinField no2_field(field_x, _y + row++, 3, nos, "1");
  ++row;
  writestr(3, row, "Mode:");
  SpinField mode_field(field_x, _y + row++, 3, modes, "AND");

  _field_list.clear();
  _field_list.push_back(&no1_field);
  _field_list.push_back(&no2_field);
  _field_list.push_back(&mode_field);
  _curr_field = _field_list.begin();
  (*_curr_field)->focus(true);

  curs_set(1);
  _completed = _save = false;
  InteractorList responders;
  while (!_completed) {
    for (list<EditField *>::iterator it = _field_list.begin();
         it != _field_list.end(); ++it)
      if (*it != *_curr_field) (*it)->display();
    (*_curr_field)->display();
    doupdate();
    responders.clear();
    responders.push_back(**_curr_field);
    responders.push_back(*this);
    responders.process_key();
  }
  curs_set(0);

  if (_save) {
    no1 = atoi(no1_field.result().c_str());
    no2 = atoi(no2_field.result().c_str());
    and_mode = mode_field.result() == "AND";
  }

  _field_list.clear();
  clear();
  return _save;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
