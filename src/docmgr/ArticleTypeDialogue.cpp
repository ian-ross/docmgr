//----------------------------------------------------------------------
//
//  FILE:   ArticleTypeDialogue.cpp
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2005
//
//----------------------------------------------------------------------
//
//  Article type selection dialogue.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Local headers.

#include "ArticleTypeDialogue.hh"
#include "InteractorList.hh"


//----------------------------------------------------------------------
//
//  CONSTANT DEFINITIONS
//
//----------------------------------------------------------------------

const int DIALOGUE_WIDTH = 25;


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

ArticleTypeDialogue::ArticleTypeDialogue(DocMgr::Connection &db) :
  Widget((getmaxx(stdscr) - DIALOGUE_WIDTH) / 2,
         (getmaxy(stdscr) - (db.doc_types().size() + 4)) / 2,
         DIALOGUE_WIDTH, db.doc_types().size() + 4),
  _db(db)
{ }


bool ArticleTypeDialogue::process_key(int ch)
{
  bool retval = false;
  switch(ch) {
  case KEY_UP:
  case CTRL('P'):
    if (_curr_row == 0)
      _curr_row = _db.doc_types().size() - 1;
    else
      --_curr_row;
    retval = true;
    break;

  case KEY_DOWN:
  case CTRL('N'):
    if (_curr_row == _db.doc_types().size() - 1)
      _curr_row = 0;
    else
      ++_curr_row;
    retval = true;
    break;

  case CTRL('J'):
  case CTRL('M'):
  case KEY_ENTER:
  case META('s'):
    _completed = true;
    _save = true;
    retval = true;
    break;

  case META('a'):
    _completed = true;
    _save = false;
    retval = true;
    break;
  }
  return retval;
}


void ArticleTypeDialogue::display(void)
{
  clear();
  frame();
  string title = "New-Document-Type";
  writestr((_w - title.size()) / 2, 0, title);
  draw_hline(_h - 3);
  mvwaddch(_win, _h - 3, 0, ACS_LTEE);
  mvwaddch(_win, _h - 3, _w - 1, ACS_RTEE);
  string menu_str = "Select   Abandon";
  writestr((_w - menu_str.size()) / 2, _h - 2, menu_str);
  writestr((_w - menu_str.size()) / 2, _h - 2, "S", A_BOLD);
  writestr((_w - menu_str.size()) / 2 + 9, _h - 2, "A", A_BOLD);

  int row = 0;
  for (vector<DocMgr::DocType>::const_iterator it = _db.doc_types().begin();
       it != _db.doc_types().end(); ++it) {
    string name = it->name();
    if (row == _curr_row)
      writestr((_w - name.size()) / 2, row + 1, name, A_BOLD);
    else
      writestr((_w - name.size()) / 2, row + 1, name);
    ++row;
  }
  wnoutrefresh(_win);
}
  

bool ArticleTypeDialogue::run(DocMgr::DocType &result)
{
  _curr_row = 0;
  _completed = _save = false;
  display();
  doupdate();

  InteractorList interaction;
  interaction.push_back(*this);

  while (!_completed) { interaction.process_key(); display(); doupdate(); }

  clear();

  if (_save) result = _db.doc_types()[_curr_row];
  return _save;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
