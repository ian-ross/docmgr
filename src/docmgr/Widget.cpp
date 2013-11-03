//----------------------------------------------------------------------
//
//  FILE:   Widget.cpp
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Curses widget class.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Curses header.

#include <curses.h>


// Local headers.

#include "Widget.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

Widget::Widget(int x, int y, int w, int h) :
  _x(x), _y(y), _w(w), _h(h), _activated(false)
{
  if (_h <= 0) _h = getmaxy(stdscr) + _h;
  if (_w <= 0) _w = getmaxx(stdscr) + _w;
  _win = subwin(stdscr, _h, _w, _y, _x);
}

void Widget::clear(void)
{
  werase(_win);
  wnoutrefresh(_win);
}

void Widget::frame(void)
{
  wborder(_win, 0, 0, 0, 0, 0, 0, 0, 0);
  wnoutrefresh(_win);
}

void Widget::writestr(int x, int y, string str)
{
  mvwaddstr(_win, y, x, str.c_str());
  wnoutrefresh(_win);
}

void Widget::writestr(int x, int y, string str, int attr)
{
  wattron(_win, attr);
  mvwaddstr(_win, y, x, str.c_str());
  wattroff(_win, attr);
  wnoutrefresh(_win);
}

void Widget::scrollable_up(bool is_it)
{
  int ch = is_it ? '^' : ACS_HLINE;
  mvwaddch(_win, 0, _w - 2, ch);
  wnoutrefresh(_win);
}

void Widget::scrollable_down(bool is_it)
{
  int ch = is_it ? 'v' : ACS_HLINE;
  mvwaddch(_win, _h - 1, _w - 2, ch);
  wnoutrefresh(_win);
}

void Widget::draw_hline(int y)
{
  for (int x = 1; x < _w - 1; ++x)
    mvwaddch(_win, y, x, ACS_HLINE);
  wnoutrefresh(_win);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
