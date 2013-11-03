//----------------------------------------------------------------------
//
//  FILE:   InteractorList.cpp
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Class for managing keyboard interaction widgets.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <cctype>
using namespace std;


// Unix headers.

#include <curses.h>


// Local headers.

#include "InteractorList.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

void InteractorList::process_key(void)
{
  for (list<Widget *>::iterator it = _interactor_list.begin();
       it != _interactor_list.end(); ++it)
    (*it)->clear_event();
  int ch = get_char();
  for (list<Widget *>::iterator it = _interactor_list.begin();
       it != _interactor_list.end(); ++it)
    if ((*it)->process_key(ch)) { (*it)->set_event();  break; }
}


int InteractorList::get_char(void)
{
  int retval;

  int ch = getch();
  retval = ch;
  if (ch == KEY_RESIZE) throw Resize();
  if (ch == KEY_ESCAPE) {
    halfdelay(1);
    int ch2 = getch();
    if (ch2 == KEY_RESIZE) throw Resize();
    if (ch2 != ERR) {
      if (islower(ch2)) retval = 0x80 | ch2;
      else if (ch2 == 'O') {
        int ch3 = getch();
        if (ch3 == KEY_RESIZE) throw Resize();
        if (ch3 == ERR) { ungetch(ch2); }
        else if (ch3 != '5') { ungetch(ch3); ungetch(ch2); }
        else {
          int ch4 = getch();
          if (ch4 == KEY_RESIZE) throw Resize();
          if (ch4 == ERR) { ungetch(ch3); ungetch(ch2); }
          else if (ch4 == 'A')
            retval = KEY_CTRL_UP;
          else if (ch4 == 'B')
            retval = KEY_CTRL_DOWN;
          else if (ch4 == 'C')
            retval = KEY_CTRL_RIGHT;
          else if (ch4 == 'D')
            retval = KEY_CTRL_LEFT;
          else { ungetch(ch4); ungetch(ch3); ungetch(ch2); }
        }
      } else if (ch2 == '[') {
        int ch3 = getch();
        if (ch3 == KEY_RESIZE) throw Resize();
        if (ch3 == ERR) { ungetch(ch2); }
        else if (ch3 == 'Z') { retval = KEY_SHIFT_TAB; }
        else if (ch3 != '5' && ch3 != '6') {
          ungetch(ch3); ungetch(ch2);
        } else {
          int ch4 = getch();
          if (ch4 == KEY_RESIZE) throw Resize();
          int ch5 = getch();
          if (ch5 == KEY_RESIZE) throw Resize();
          int ch6 = getch();
          if (ch6 == KEY_RESIZE) throw Resize();
          if (ch4 == ERR || ch4 != ';') {
            if (ch4 != ERR) ungetch(ch4);
            ungetch(ch3); ungetch(ch2);
          } else if (ch5 == ERR || ch5 != '5') {
            if (ch5 != ERR) ungetch(ch5);
            ungetch(ch4); ungetch(ch3); ungetch(ch2);
          } else if (ch6 == ERR || ch6 != '~') {
            if (ch6 != ERR) ungetch(ch6);
            ungetch(ch5); ungetch(ch4); ungetch(ch3); ungetch(ch2);
          } else if (ch3 == '5')
            retval = KEY_CTRL_PGUP;
          else if (ch3 == '6')
            retval = KEY_CTRL_PGDN;
        }
      } else { ungetch(ch2); }
    }

    cbreak();
  }

  return retval;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
