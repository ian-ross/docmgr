//----------------------------------------------------------------------
//
//  FILE:   OptionsDialogue.cpp
//  AUTHOR: Ian Ross
//  DATE:   02-OCT-2005
//
//----------------------------------------------------------------------
//
//  Options dialogue.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Local headers.

#include "OptionsDialogue.hh"
#include "InteractorList.hh"
#include "TextField.hh"


//----------------------------------------------------------------------
//
//  CONSTANT DEFINITIONS
//
//----------------------------------------------------------------------

const int DIALOGUE_WIDTH = 60;
const int DIALOGUE_HEIGHT = 9;


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

OptionsDialogue::OptionsDialogue(Configuration *config) :
  Widget((getmaxx(stdscr) - DIALOGUE_WIDTH) / 2,
         (getmaxy(stdscr) - DIALOGUE_HEIGHT) / 2,
         DIALOGUE_WIDTH, DIALOGUE_HEIGHT),
  _config(config)
{ }


bool OptionsDialogue::process_key(int ch)
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

  case META('r'):
    _completed = true;
    _save = false;
    break;
  }
  return retval;
}


void OptionsDialogue::run(void)
{
  clear();
  frame();
  string title = "Global Options";
  writestr((_w - title.size()) / 2, 0, title);
  draw_hline(_h - 3);
  mvwaddch(_win, _h - 3, 0, ACS_LTEE);
  mvwaddch(_win, _h - 3, _w - 1, ACS_RTEE);
  string menu_str = "Save  Revert";
  writestr((_w - menu_str.size()) / 2, _h - 2, menu_str);
  writestr((_w - menu_str.size()) / 2, _h - 2, "S", A_BOLD);
  writestr((_w - menu_str.size()) / 2 + 6, _h - 2, "R", A_BOLD);

  bool show_deleted = _config->show_deleted();
  string paper_dir = _config->paper_directory();
  string default_import_file = _config->default_import_file();
  string view_command = _config->view_command();

  int row = 2;

  writestr(3, row, "Show deleted entries:");
  vector<string> yes_no;
  yes_no.push_back("No");
  yes_no.push_back("Yes");
  SpinField show_deleted_field(_x + 3 + 22, _y + row++, 3, yes_no,
                               show_deleted ? "Yes" : "No");
  writestr(3, row, "Paper directory:");
  TextField paper_dir_field(_x + 3 + 18, _y + row++,
                            _w - 18 - 6, paper_dir);
  writestr(3, row, "Import file:");
  TextField default_import_file_field(_x + 3 + 18, _y + row++,
                                      _w - 18 - 6, default_import_file);
  writestr(3, row, "PDF view command:");
  TextField view_command_field(_x + 3 + 18, _y + row++,
                               _w - 18 - 6, view_command);

  _field_list.clear();
  _field_list.push_back(&show_deleted_field);
  _field_list.push_back(&paper_dir_field);
  _field_list.push_back(&default_import_file_field);
  _field_list.push_back(&view_command_field);
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
    string tst = show_deleted_field.result();
    _config->set_show_deleted(tst == "Yes");
    _config->set_paper_directory(paper_dir);
    _config->set_default_import_file(default_import_file);
    _config->set_view_command(view_command);
    _config->save();
  }

  _field_list.clear();
  clear();
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
