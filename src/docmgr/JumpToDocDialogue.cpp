//----------------------------------------------------------------------
//
//  FILE:   JumpToDocDialogue.cpp
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Jump to numbered document dialogue.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Local headers.

#include "JumpToDocDialogue.hh"
#include "InteractorList.hh"
#include "TextField.hh"


//----------------------------------------------------------------------
//
//  CONSTANT DEFINITIONS
//
//----------------------------------------------------------------------

const int DIALOGUE_WIDTH = 20;
const int DIALOGUE_HEIGHT = 5;


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

JumpToDocDialogue::JumpToDocDialogue() :
  Widget((getmaxx(stdscr) - DIALOGUE_WIDTH) / 2,
         (getmaxy(stdscr) - DIALOGUE_HEIGHT) / 2,
         DIALOGUE_WIDTH, DIALOGUE_HEIGHT), _sp(0)
{ }


bool JumpToDocDialogue::process_key(int ch)
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


bool JumpToDocDialogue::run(DocMgr::Connection &db, IDList *id_list)
{
  bool retval = false;

  clear();
  frame();
  writestr(2, 0, "Jump to Document");

  curs_set(1);
  _stack.push_back("");
  if (_stack.size() > 10) _stack.erase(_stack.begin());
  _sp = _stack.size() - 1;
  _id_str = "";
  _field = new TextField(_x + 2, _y + 2, _w - 4, _id_str);
  _field->focus(true);
  _field->display();
  doupdate();

  InteractorList interaction;
  interaction.push_back(*_field);
  interaction.push_back(*this);

  bool ok = false;
  string err_msg;
  while (!ok) {
    doupdate();
    _completed = false;
    while (!_completed) { interaction.process_key(); doupdate(); }

    ok = true;
    if (_id_str != "") {
      ok = true;
      if (!DocMgr::DocID::valid(_id_str)) {
        ok = false;
        err_msg = "INVALID DOCUMENT ID!";
      } else {
        DocMgr::DocID id(_id_str);
        if (!id_list->id_visible(id)) {
          DocMgr::Query tmp_q(db);
          vector<DocMgr::DocID> ids;
          tmp_q.run(ids);
          bool found = false;
          for (int idx = 0; idx < ids.size(); ++idx)
            if (ids[idx] == id) { found = true;  break; }
          if (!found) {
            ok = false;
            err_msg = "DOCUMENT ID NOT FOUND IN DATABASE!";
          } else {
            retval = true;
            id_list->clear_query();
            id_list->set_current(id);
            _stack[_sp] = _id_str;
          }
        } else {
          _stack[_sp] = _id_str;
          id_list->set_current(id);
          if (_sp != _stack.size() - 1) _stack.erase(_stack.end());
        }
      }
    } else {
      _stack.erase(_stack.end());
      --_sp;
    }

    if (!ok) {
      string blank(getmaxx(stdscr), ' ');
      mvaddstr(0, 0, blank.c_str());
      wattron(stdscr, A_BOLD);
      mvaddstr(0, 0, err_msg.c_str());
      wattroff(stdscr, A_BOLD);
      wnoutrefresh(stdscr);
      _field->display();
    }
  }

  curs_set(0);
  clear();

  delete _field;
  return retval;
}


void JumpToDocDialogue::up_stack(void)
{
  if (_sp > 0) {
    _stack[_sp] = _id_str;
    --_sp;
    _id_str = _stack[_sp];
    _field->display();
  }
}


void JumpToDocDialogue::down_stack(void)
{
  if (_sp < _stack.size() - 1) {
    _stack[_sp] = _id_str;
    ++_sp;
    _id_str = _stack[_sp];
    _field->display();
  }
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
