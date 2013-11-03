//----------------------------------------------------------------------
//
//  FILE:   IDList.cpp
//  AUTHOR: Ian Ross
//  DATE:   21-SEP-2005
//
//----------------------------------------------------------------------
//
//  Document ID list widget.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Local headers.

#include "IDList.hh"
#include "ArticleViewForm.hh"
#include "InteractorList.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

// The initial database query is an empty one, to retrieve all the
// available IDs.

IDList::IDList(DocMgr::Connection *db, ArticleViewForm &view_form,
               int x, int y, int w, int h) :
  Widget(x, y, w, h),
  _db(db), _view_form(view_form), _top_vis(0), _curr_loc(0)
{
  _id_query = new DocMgr::Query(*db);
  fill_id_list();
  _db->get_deleted_ids(_deleted_ids);
  _view_form.set_current(current());
}

void IDList::display(void)
{
  clear();
  frame();

  if (_ids.size() > 0) {
    int rows = _ids.size() > (_h - 2) ? (_h - 2) : _ids.size();
    for (int row = 0; row < rows; ++row) {
      DocMgr::DocID id = _ids[_top_vis + row];
      int attr = A_NORMAL;
      if (id_deleted(id)) attr |= COLOR_PAIR(COLOR_RED);
      if (_curr_loc == row) attr |= A_BOLD;
      writestr(1, row + 1, string(_ids[_top_vis + row]), attr);
    }

    scrollable_up(_top_vis != 0);
    scrollable_down(_ids.size() > _h - 2 && _top_vis + _h - 2 < _ids.size());
  }
  wnoutrefresh(_win);
}

void IDList::fill_id_list(void)
{
  _id_query->run(_ids);
  if (_ids.size() == 0)
    _top_vis = _curr_loc = -1;
  else if (_top_vis == -1) _top_vis = _curr_loc = 0;
  _db->get_deleted_ids(_deleted_ids);
}

void IDList::refresh(void)
{
  fill_id_list();
  if (_top_vis + _curr_loc >= _ids.size()) set_current(_ids[_ids.size()-1]);
  if (_ids.size() > _h - 2 && _top_vis + _h - 3 > _ids.size() - 1)
    _top_vis = _ids.size() - 1 -_h + 3;
  display();
}

bool IDList::process_key(int ch)
{
  bool retval = false;
  bool moved = false;
  int scroll = 0;
  int old_curr_loc = _curr_loc, old_top_vis = _top_vis;
  switch (ch) {
  case KEY_UP:
  case CTRL('P'):
    // Up one...
    if (_ids.size() > 0 && _top_vis + _curr_loc > 0) {
      moved = true;
      if (_curr_loc == 0)
        scroll = -1;
      else
        --_curr_loc;
    }
    retval = true;
    break;

  case KEY_DOWN:
  case CTRL('N'):
    // Down one...
    if (_ids.size() > 0 && _top_vis + _curr_loc < _ids.size() - 1) {
      moved = true;
      if (_curr_loc == _h - 3)
        scroll = +1;
      else
        ++_curr_loc;
    }
    retval = true;
    break;

  case KEY_PPAGE:
  case META('v'):
    // Up one page...
    if (_ids.size() > 0 && _top_vis > 0) {
      moved = true;
      scroll = -(_h - 2);
    }
    retval = true;
    break;

  case KEY_NPAGE:
  case CTRL('V'):
    // Down one page...
    if (_ids.size() > 0 && _top_vis < _ids.size() - _h + 2) {
      moved = true;
      scroll = _h - 2;
    }
    retval = true;
    break;

  case KEY_HOME:
    // Go to top...
    set_current(_ids[0]);
    moved = true;
    break;

  case KEY_END:
    // Go to bottom...
    set_current(_ids[_ids.size()-1]);
    moved = true;
    break;

  default: break;
  }
  if (moved) {
    if (scroll != 0) {
      _top_vis += scroll;
      if (_top_vis < 0) _top_vis = 0;
      if (_top_vis > _ids.size() - _h + 2) _top_vis = _ids.size() - _h + 2;
      display();
    } else {
      DocMgr::DocID id = _ids[_top_vis + old_curr_loc];
      int attr = A_NORMAL;
      if (id_deleted(id)) attr |= COLOR_PAIR(COLOR_RED);
      writestr(1, old_curr_loc + 1, string(id), attr);
      id = _ids[_top_vis + _curr_loc];
      attr = A_BOLD;
      if (id_deleted(id)) attr |= COLOR_PAIR(COLOR_RED);
      writestr(1, _curr_loc + 1, string(id), attr);
    }
    scrollable_up(_top_vis != 0);
    scrollable_down(_ids.size() > _h - 2 && _top_vis + _h - 2 < _ids.size());
    _view_form.set_current(current());
    wnoutrefresh(_win);
  }
  return retval;
}


// Get the currently highlighted document ID.

DocMgr::DocID IDList::current(void) const
{
  DocMgr::DocID retval;
  if (_ids.size() > 0) retval = _ids[_top_vis + _curr_loc];
  return retval;
}


void IDList::set_current(DocMgr::DocID id)
{
  fill_id_list();
  if (_ids.size() > 0) {
    int loc;
    bool found = false;
    for (loc = 0; loc < _ids.size(); ++loc)
      if (_ids[loc] == id) { found = true; break; }
    
    if (!found)
      _top_vis = _curr_loc = 0;
    else {
      if (_ids.size() < _h - 2) {
        _top_vis = 0;
        _curr_loc = loc;
      } else {
        _top_vis = loc - _curr_loc;
        if (_top_vis < 0) { _curr_loc += _top_vis;  _top_vis = 0; }
        if (_top_vis + _h - 2 >= _ids.size()) {
          _curr_loc += _top_vis - (_ids.size() - _h + 2);
          _top_vis = _ids.size() - _h + 2;
        }
      }
    }
  } else
    _top_vis = _curr_loc = -1;

  display();

  _view_form.set_current(current());
}


void IDList::set_query(DocMgr::Query &query)
{
  DocMgr::DocID old_current = current();
  int old_curr_loc = _curr_loc;
  delete _id_query;
  _id_query = new DocMgr::Query(query);
  fill_id_list();

  if (_ids.size() > 0) {
    int loc;
    bool found = false;
    for (loc = 0; loc < _ids.size(); ++loc)
      if (_ids[loc] == old_current) { found = true; break; }
    
    if (!found)
      _top_vis = _curr_loc = 0;
    else {
      if (_ids.size() < _h - 2) {
        _top_vis = 0;
        _curr_loc = loc;
      } else {
        _top_vis = loc - _curr_loc;
        if (_top_vis < 0) { _curr_loc += _top_vis;  _top_vis = 0; }
        if (_top_vis + _h - 2 >= _ids.size()) {
          _curr_loc += _top_vis - (_ids.size() - _h + 2);
          _top_vis = _ids.size() - _h + 2;
        }
      }
    }
  }

  display();
  _view_form.set_current(current());
}


bool IDList::id_deleted(DocMgr::DocID id)
{
  for (int idx = 0; idx < _deleted_ids.size(); ++idx)
    if (id == _deleted_ids[idx]) return true;
  return false;
}


void IDList::toggle_deleted(DocMgr::DocID id)
{
  if (_ids.size() > 0) {
    if (id_deleted(id))
      _db->undelete_doc(id);
    else {
      _db->delete_doc(id);
      if (!_db->view_deleted() &&
          _top_vis == 0 && _curr_loc == _ids.size() - 1) {
        if (_curr_loc > 0) --_curr_loc;
        else if (_top_vis > 0) --_top_vis;
      }
    }
  }
}


bool IDList::id_visible(DocMgr::DocID id)
{
  for (int idx = 0; idx < _ids.size(); ++idx)
    if (_ids[idx] == id) return true;
  return false;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
