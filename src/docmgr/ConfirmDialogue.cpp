//----------------------------------------------------------------------
//
//  FILE:   ConfirmDialogue.cpp
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Confirmation dialogue.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <cctype>
#include <vector>

using namespace std;

// Local headers.

#include "ConfirmDialogue.hh"
#include "InteractorList.hh"
#include "TextField.hh"
#include "SpinField.hh"


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

ConfirmDialogue::ConfirmDialogue(bool tough) :
  Widget((getmaxx(stdscr) - DIALOGUE_WIDTH) / 2, 1, DIALOGUE_WIDTH, 10),
  _tough(tough)
{ }


bool ConfirmDialogue::process_key(int ch)
{
  bool retval = false;
  switch(ch) {
  case CTRL('J'):
  case CTRL('M'):
  case KEY_ENTER:
    _completed = true;
    retval = true;
    break;
  }
  return retval;
}


bool ConfirmDialogue::run(string message)
{
  int line_count = 1;
  string::size_type pos = 0;
  while (message.find("\n", pos) != string::npos) {
    ++line_count;
    pos = message.find("\n", pos) + 1;
  }
  _h = line_count + 5;
  _y = (getmaxy(stdscr) - _h) / 2,
  delwin(_win);
  _win = subwin(stdscr, _h, _w, _y, _x);

  clear();
  frame();
  string title = "Confirmation";
  writestr((_w - title.size()) / 2, 0, title);

  vector<string> lines;
  while (message.find('\n') != string::npos) {
    lines.push_back(message.substr(0, message.find('\n')));
    message = message.substr(message.find('\n') + 1);
  }
  lines.push_back(message);

  int row = 2;
  for (int idx = 0; idx < lines.size(); ++idx) {
    int attr = (lines[idx][0] == 'B') ? A_BOLD : A_NORMAL;
    string msg_line = lines[idx].substr(1);
    writestr((_w - msg_line.size()) / 2, row, msg_line, attr);
    ++row;
  }

  curs_set(1);
  string result;
  EditField *field;
  if (_tough)
    field = new TextField(_x + (_w - 4) / 2, _y + _h - 2, -4, result);
  else {
    vector<string> options;
    options.push_back("Yes");
    options.push_back("No");
    field = new SpinField(_x + (_w - 4) / 2, _y + _h - 2, 4, options, "Yes");
  }
  field->focus(true);
  field->display();
  doupdate();

  InteractorList interaction;
  interaction.push_back(*field);
  interaction.push_back(*this);

  _completed = false;
  while (!_completed) { interaction.process_key(); doupdate(); }

  curs_set(0);
  clear();

  if (!_tough) result = field->result();
  delete field;
  return (result.size() == 3 && toupper(result[0]) == 'Y' &&
          toupper(result[1]) == 'E' && toupper(result[2]) == 'S');
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
