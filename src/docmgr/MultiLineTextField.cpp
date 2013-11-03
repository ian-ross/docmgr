//----------------------------------------------------------------------
//
//  FILE:   MultiLineTextField.cpp
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

#include "MultiLineTextField.hh"
#include "InteractorList.hh"
#include "ArticleEditForm.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

MultiLineTextField::MultiLineTextField(int x, int y, int w, string &result,
                                       string word_separators) :
  EditField(x, y, w), _result(result),
  _curs_pos(0), _word_separators(word_separators)
{
  _nlines = _result.size() / _w;
  if (_result.size() % _w != 0) ++_nlines;
  if (_nlines == 0) _nlines = 1;
  delwin(_win);
  _h = _nlines;
  _win = subwin(stdscr, _h, _w, _y, _x);
}


bool MultiLineTextField::process_key(int ch)
{
  bool retval = false, contents_changed = false;
  switch(ch) {
  case KEY_LEFT:
  case CTRL('B'):
    // Process leftwards motion.
    if (_curs_pos > 0) --_curs_pos;
    retval = true;
    break;

  case KEY_RIGHT:
  case CTRL('F'):
    // Process rightwards motion.
    if (_curs_pos < _result.size()) ++_curs_pos;
    retval = true;
    break;

  case KEY_UP:
  case CTRL('P'):
    if (_curs_pos / _w > 0) {
      _curs_pos -= _w;
      retval = true;
    }
    break;

  case KEY_DOWN:
  case CTRL('N'):
    if (_curs_pos / _w < _h - 1) {
      _curs_pos += _w;
      if (_curs_pos > _result.size()) _curs_pos = _result.size(); 
      retval = true;
    }
    break;

  case KEY_CTRL_LEFT:
  case META('b'):
    // Process leftwards word motion.
    _curs_pos = word_motion(-1);
    retval = true;
    break;

  case KEY_CTRL_RIGHT:
  case META('f'):
    // Process rightwards word motion.
    _curs_pos = word_motion(+1);
    retval = true;
    break;

  case KEY_HOME:
  case CTRL('A'):
    // Process home motion.
    _curs_pos = (_curs_pos / _w) * _w;
    retval = true;
    break;

  case KEY_END:
  case CTRL('E'):
    // Process end motion.
    _curs_pos = (_curs_pos / _w) * _w + _w - 1;
    if (_curs_pos > _result.size()) _curs_pos = _result.size();
    retval = true;
    break;

  case META('a'):
    // Process home motion.
    _curs_pos = 0;
    retval = true;
    break;

  case META('e'):
    // Process home motion.
    _curs_pos = _result.size();
    retval = true;
    break;

  case CTRL('T'):
    // Process character transposition.
    if (_result.size() >= 2 && _curs_pos > 0 &&
        _curs_pos <= _result.size() - 1) {
      int tmp = _result[_curs_pos];
      _result[_curs_pos] = _result[_curs_pos - 1];
      _result[_curs_pos - 1] = tmp;
    }
    retval = true;
    break;

  case KEY_BACKSPACE:
    // Process backspace deletion.
    if (_curs_pos > 0) {
      _result.erase(_curs_pos - 1, 1);
      --_curs_pos;
    }
    contents_changed = true;
    retval = true;
    break;

  case KEY_DC:
  case CTRL('D'):
    // Process under-cursor deletion.
    if (_result.size() > 0 && _curs_pos <= _result.size() - 1)
      _result.erase(_curs_pos, 1);
    contents_changed = true;
    retval = true;
    break;

  case META('d'): {
    // Process under-cursor word deletion.
    if (_result.size() > 0 && _curs_pos <= _result.size() - 1) {
      int end_pos = word_motion(+1);
      _result.erase(_curs_pos, end_pos - _curs_pos);
    }
    contents_changed = true;
    retval = true;
    break;
  }

  case META('u'): {
    // Process upper-case word.
    if (_result.size() > 0 && _curs_pos <= _result.size() - 1) {
      int end_pos = word_motion(+1);
      for (int pos = _curs_pos; pos <= end_pos; ++pos)
        _result[pos] = toupper(_result[pos]);
    }
    process_key(META('f'));
    retval = true;
    break;
  }

  case META('l'): {
    // Process lower-case word.
    if (_result.size() > 0 && _curs_pos <= _result.size() - 1) {
      int end_pos = word_motion(+1);
      for (int pos = _curs_pos; pos <= end_pos; ++pos)
        _result[pos] = tolower(_result[pos]);
    }
    process_key(META('f'));
    retval = true;
    break;
  }

  case META('c'): {
    // Process capitalise word.
    if (_result.size() > 0 && _curs_pos <= _result.size() - 1) {
      int end_pos = word_motion(+1);
      bool capitalised = false;
      for (int pos = _curs_pos; pos <= end_pos; ++pos)
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
    if (_result.size() > 0 && _curs_pos <= _result.size() - 1)
      _result.erase(_curs_pos);
    contents_changed = true;
    retval = true;
    break;

  case CTRL('U'):
    // Process kill all.
    _result = "";
    _curs_pos = 0;
    contents_changed = true;
    retval = true;
    break;

  default:
    if (isprint(ch) && ch < 256) {
      // Insert printing characters.
      if (_y + _result.size() / _w < getmaxy(stdscr) - 2) {
        _result.insert(_curs_pos, 1, ch);
        process_key(KEY_RIGHT);
        contents_changed = true;
      }
      retval = true;
    }
  }

  if (contents_changed) {
    int new_nlines = _result.size() / _w;
    if (_result.size() % _w != 0) ++new_nlines;
    if (new_nlines == 0) new_nlines = 1;
    if (new_nlines > _nlines) {
      _nlines = new_nlines;
      delwin(_win);
      ++_h;
      _win = subwin(stdscr, _h, _w, _y, _x);
    }
  }

  if (retval) display();
  return retval;
}

int MultiLineTextField::word_motion(int dir)
{
  int pos = _curs_pos;
  if (dir < 0) {
    while (pos > 0 && _word_separators.find(_result[pos]) != string::npos)
      --pos;
    while (pos > 0 && _word_separators.find(_result[pos]) == string::npos)
      --pos;
  } else {
    while (pos < _result.size() &&
           _word_separators.find(_result[pos]) != string::npos)
      ++pos;
    while (pos < _result.size() &&
           _word_separators.find(_result[pos]) == string::npos)
      ++pos;
  }
  return pos;
}


void MultiLineTextField::display(int lines)
{
//   char buff[81];
//   sprintf(buff, "W:%d CP:%d H:%d", _w, _curs_pos, _h);
//   char *pt = buff;
//   while (*pt) ++pt;
//   *pt = ' ';
//   buff[80] = '\0';
//   while (*pt) { *pt = ' '; ++pt; }
//   mvaddstr(0, 0, buff);
//   wnoutrefresh(stdscr);

  _displayed_lines = lines;
  for (int line = 0; line < lines; ++line) {
    writestr(0, line, string(_w, '_'));
    if (line * _w < _result.size())
      writestr(0, line, _result.substr(line * _w, _w));
  }
  wnoutrefresh(_win);
  if (_focus) {
    if (_result.size() != 0 && _curs_pos == _result.size() &&
        _curs_pos % _w == 0)
      wmove(stdscr, _y + _curs_pos / _w - 1, _x + _w);
    else
      wmove(stdscr, _y + _curs_pos / _w, _x + _curs_pos % _w);
    refresh();
  }
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
