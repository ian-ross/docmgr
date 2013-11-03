//----------------------------------------------------------------------
//
//  FILE:   QuickSearchDialogue.cpp
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Search dialogue.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Local headers.

#include "QuickSearchDialogue.hh"
#include "InteractorList.hh"
#include "TextField.hh"


//----------------------------------------------------------------------
//
//  CONSTANT DEFINITIONS
//
//----------------------------------------------------------------------

const int DIALOGUE_WIDTH = 50;


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

QuickSearchDialogue::QuickSearchDialogue() :
  Widget((getmaxx(stdscr) - DIALOGUE_WIDTH) / 2,
         (getmaxy(stdscr) - 5) / 2,
         DIALOGUE_WIDTH, 5), _sp(0)
{ }


bool QuickSearchDialogue::process_key(int ch)
{
  bool retval = false;
  switch(ch) {
  case CTRL('P'):
  case KEY_UP:
    up_stack();
    retval = true;
    break;

  case CTRL('N'):
  case KEY_DOWN:
    down_stack();
    retval = true;
    break;

  case CTRL('J'):
  case CTRL('M'):
  case KEY_ENTER:
    _completed = true;
    retval = true;
    break;
  }
  return retval;
}


void QuickSearchDialogue::run(string &result)
{
  clear();
  frame();
  writestr(2, 0, "Quick-Search");

  curs_set(1);
  _stack.push_back("");
  if (_stack.size() > 10) _stack.erase(_stack.begin());
  _sp = _stack.size() - 1;
  _search_str = "";
  _field = new TextField(_x + 2, _y + 2, _w - 4, _search_str);
  _field->focus(true);
  _field->display();
  doupdate();

  InteractorList interaction;
  interaction.push_back(*_field);
  interaction.push_back(*this);

  _completed = false;
  while (!_completed) { interaction.process_key(); doupdate(); }

  result = _search_str;
  if (_search_str != "") {
    _stack[_sp] = _search_str;
    if (_sp != _stack.size() - 1) _stack.erase(_stack.end());
  } else {
    _stack.erase(_stack.end());
    --_sp;
  }

  curs_set(0);
  clear();
}


void QuickSearchDialogue::up_stack(void)
{
  if (_sp > 0) {
    _stack[_sp] = _search_str;
    --_sp;
    _search_str = _stack[_sp];
    _field->display();
  }
}


void QuickSearchDialogue::down_stack(void)
{
  if (_sp < _stack.size() - 1) {
    _stack[_sp] = _search_str;
    ++_sp;
    _search_str = _stack[_sp];
    _field->display();
  }
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
