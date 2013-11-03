//----------------------------------------------------------------------
//
//  FILE:   TextField.cpp
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2005
//
//----------------------------------------------------------------------
//
//  Editable Curses text field.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <cctype>
#include <string>

using namespace std;


// Local headers.

#include "TextField.hh"
#include "InteractorList.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

TextField::TextField(int x, int y, int w, string &result,
                     string word_separators) :
  EditField(x, y, w < 0 ? -w : w), _result(result),
  _curs_pos(result.size() <= _w ? result.size() : 0),
  _first_displayed(0), _scrollable(w > 0),
  _word_separators(word_separators)
{ }


bool TextField::process_key(int ch)
{
  bool retval = false;
  switch(ch) {
  case KEY_LEFT:
  case CTRL('B'):
    // Process leftwards motion.
    if (_curs_pos > 0)
      --_curs_pos;
    else if (_scrollable && _first_displayed > 0)
      --_first_displayed;
    retval = true;
    break;

  case KEY_RIGHT:
  case CTRL('F'):
    // Process rightwards motion.
    if (_curs_pos < _w - 1) {
      if (_first_displayed + _curs_pos < _result.size())
        ++_curs_pos;
    } else if (_scrollable && _result.size() - _first_displayed >= _w)
      ++_first_displayed;
    retval = true;
    break;

  case KEY_CTRL_LEFT:
  case META('b'): {
    // Process leftwards word motion.
    int new_pos = word_motion(-1);
    if (new_pos - _first_displayed >= 0)
      _curs_pos = new_pos - _first_displayed;
    else if (_scrollable) {
      _first_displayed = new_pos;
      _curs_pos = 0;
    }
    retval = true;
    break;
  }

  case KEY_CTRL_RIGHT:
  case META('f'): {
    // Process rightwards word motion.
    int new_pos = word_motion(+1);
    if (new_pos - _first_displayed <= _w - 1)
      _curs_pos = new_pos - _first_displayed;
    else if (_scrollable) {
      _first_displayed = new_pos - _w + 1;
      _curs_pos = _w - 1;
    }
    retval = true;
    break;
  }

  case KEY_HOME:
  case CTRL('A'):
    // Process home motion.
    _first_displayed = 0;
    _curs_pos = 0;
    retval = true;
    break;

  case KEY_END:
  case CTRL('E'):
    // Process end motion.
    if (_result.size() < _w) {
      _first_displayed = 0;
      _curs_pos = _result.size();
    } else if (_scrollable) {
      _first_displayed = _result.size() - _w + 1;
      _curs_pos = _w - 1;
    }
    retval = true;
    break;

  case CTRL('T'):
    // Process character transposition.
    if (_result.size() >= 2 && _curs_pos > 0 &&
        _first_displayed + _curs_pos > 0 &&
        _first_displayed + _curs_pos <= _result.size() - 1) {
      int tmp = _result[_first_displayed + _curs_pos];
      _result[_first_displayed + _curs_pos] =
        _result[_first_displayed + _curs_pos - 1];
      _result[_first_displayed + _curs_pos - 1] = tmp;
    }
    retval = true;
    break;

  case KEY_BACKSPACE:
    // Process backspace deletion.
    if (_curs_pos > 0) {
      _result.erase(_first_displayed + _curs_pos - 1, 1);
      --_curs_pos;
    } else if (_first_displayed > 0) {
      _result.erase(_first_displayed - 1, 1);
      --_first_displayed;
    }
    retval = true;
    break;

  case KEY_DC:
  case CTRL('D'):
    // Process under-cursor deletion.
    if (_result.size() > 0 &&
        _first_displayed + _curs_pos <= _result.size() - 1)
      _result.erase(_first_displayed + _curs_pos, 1);
    retval = true;
    break;

  case META('d'): {
    // Process under-cursor word deletion.
    if (_result.size() > 0 &&
        _first_displayed + _curs_pos <= _result.size() - 1) {
      int end_pos = word_motion(+1);
      _result.erase(_first_displayed + _curs_pos,
                    end_pos - (_first_displayed + _curs_pos));
    }
    retval = true;
    break;
  }

  case META('u'): {
    // Process upper-case word.
    if (_result.size() > 0 &&
        _first_displayed + _curs_pos <= _result.size() - 1) {
      int end_pos = word_motion(+1);
      for (int pos = _first_displayed + _curs_pos; pos <= end_pos; ++pos)
        _result[pos] = toupper(_result[pos]);
    }
    process_key(META('f'));
    retval = true;
    break;
  }

  case META('l'): {
    // Process lower-case word.
    if (_result.size() > 0 &&
        _first_displayed + _curs_pos <= _result.size() - 1) {
      int end_pos = word_motion(+1);
      for (int pos = _first_displayed + _curs_pos; pos <= end_pos; ++pos)
        _result[pos] = tolower(_result[pos]);
    }
    process_key(META('f'));
    retval = true;
    break;
  }

  case META('c'): {
    // Process capitalise word.
    if (_result.size() > 0 &&
        _first_displayed + _curs_pos <= _result.size() - 1) {
      int end_pos = word_motion(+1);
      bool capitalised = false;
      for (int pos = _first_displayed + _curs_pos; pos <= end_pos; ++pos)
        if (_word_separators.find(_result[pos]) == string::npos) {
          if (!capitalised)
            _result[pos] = toupper(_result[pos]);
          else
            _result[pos] = tolower(_result[pos]);
          capitalised = true;
        }
    }
    process_key(META('f'));
    retval = true;
    break;
  }

  case CTRL('K'):
    // Process kill to end of line.
    if (_result.size() > 0 &&
        _first_displayed + _curs_pos <= _result.size() - 1)
      _result.erase(_first_displayed + _curs_pos);
    retval = true;
    break;

  case CTRL('U'):
    // Process kill all.
    _result = "";
    _curs_pos = 0;
    _first_displayed = 0;
    retval = true;
    break;

  default:
    if (isprint(ch) && ch < 256) {
      // Insert printing characters.
      if (_scrollable) {
        _result.insert(_first_displayed + _curs_pos, 1, ch);
        if (_result.size() - _first_displayed < _w)
          ++_curs_pos;
        else
          ++_first_displayed;
      } else {
        if (_result.size() < _w - 1) {
          _result.insert(_curs_pos, 1, ch);
          ++_curs_pos;
        }
      }
      retval = true;
    }
  }

  if (retval) display();
  return retval;
}


int TextField::word_motion(int dir)
{
  int pos = _first_displayed + _curs_pos;

  if ((dir < 0 && pos > 0) || (dir > 0 && pos < _result.size()) &&
      _word_separators.find(_result[pos]) != string::npos)
    pos += dir;

  while ((dir < 0 && pos > 0) || (dir > 0 && pos < _result.size())) {
    if (_word_separators.find(_result[pos]) != string::npos)
      return pos;
    pos += dir;
  }

  return pos;
}


void TextField::display(void)
{
//   char buff[81];
//   sprintf(buff, "W:%d CP:%d FD:%d RS:%d",
//           _w, _curs_pos, _first_displayed, _result.size());
//   char *pt = buff;
//   while (*pt) ++pt;
//   *pt = ' ';
//   buff[80] = '\0';
//   while (*pt) { *pt = ' '; ++pt; }
//   mvaddstr(0, 0, buff);
//   wnoutrefresh(stdscr);
  writestr(0, 0, string(_w, '_'));
  writestr(0, 0, _result.substr(_first_displayed, _w));
  wnoutrefresh(_win);
  if (_focus) {
    wmove(stdscr, _y, _x + _curs_pos);
    refresh();
  }
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
