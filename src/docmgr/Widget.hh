//----------------------------------------------------------------------
//
//  FILE:   Widget.hh
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2005
//
//----------------------------------------------------------------------
//
//  Header file for Curses widget base class.
//
//----------------------------------------------------------------------

#ifndef _H_WIDGET_
#define _H_WIDGET_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>

using namespace std;


// Unix headers.

#include <curses.h>


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class Widget {
public:

  // Widgets are fixed in position.
  Widget(int x, int y, int w, int h);
  ~Widget() { delwin(_win); }


  // Process a single key event.
  virtual bool process_key(int ch) { return false; }

  // Display/hide the widget.
  virtual void display(void) { }
  virtual void hide(void) { clear(); }

  void clear_event(void) { _activated = false; }
  void set_event(void) { _activated = true; }
  bool activated(void) const { return _activated; }

  // Drawing functions.
  void clear(void);
  virtual void frame(void);
  void writestr(int x, int y, string str);
  void writestr(int x, int y, string str, int attr);
  virtual void scrollable_up(bool is_it);
  virtual void scrollable_down(bool is_it);
  void draw_hline(int y);

  int _x, _y, _w, _h;
  WINDOW *_win;

private:

  bool _activated;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
