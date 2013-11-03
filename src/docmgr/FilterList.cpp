//----------------------------------------------------------------------
//
//  FILE:   FilterList.cpp
//  AUTHOR: Ian Ross
//  DATE:   07-OCT-2005
//
//----------------------------------------------------------------------
//
//  Filter list.
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

// Local headers.

#include "FilterList.hh"
#include "InteractorList.hh"
#include "FilterCombinationDialogue.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

// The filter list is fixed in position, and permanently associated
// with a database connection and a configuration file object.

FilterList::FilterList(DocMgr::Connection *db, Configuration *config,
                       int x, int y, int w, int h) :
  Widget(x, y, w, h), _db(*db), _config(config),
  _current_filter_idx(0), _top_row_vis(0), _top_idx_vis(0),
  _editing_menu(0, getmaxy(stdscr) - 1, getmaxx(stdscr)),
  _editing(false)

{
  for (int idx = 0; idx < config->filter_count(); ++idx) {
    _filters.push_back(new Filter(_db));
    _filters[idx]->set_name(config->filter_name(idx));
    _filters[idx]->set_definition(config->filter_definition(idx));
    if (config->filter_holding(idx) != "")
      _filters[idx]->set_holding(config->filter_holding(idx));
    else
      _filters[idx]->clear_holding();
    if (config->filter_status(idx) != "")
      _filters[idx]->set_status(config->filter_status(idx));
    else
      _filters[idx]->clear_status();
  }
  refresh();

  _editing_menu.add_item("&Save",    0);
  _editing_menu.add_item("&Abandon", 1);
}


FilterList::~FilterList()
{
  for (int idx = 0; idx < _filters.size(); ++idx)
    delete _filters[idx];
}


void FilterList::refresh(void)
{
  _article_counts.clear();
  for (int idx = 0; idx < _filters.size(); ++idx) {
    DocMgr::Query q = _filters[idx]->query();
    vector<DocMgr::DocID> ids;
    q.run(ids);
    _article_counts.push_back(ids.size());
  }
}


// Process a single key event.

bool FilterList::process_key(int ch)
{
  bool retval = false;
  if (_editing) {
    switch (ch) {
    case KEY_UP:
    case KEY_SHIFT_TAB:
    case CTRL('P'):
      _curr_field->focus(false);
      if (_curr_field == _name_field)
        _curr_field = _status_field;
      else if (_curr_field == _def_field)
        _curr_field = _name_field;
      else if (_curr_field == _holding_field)
        _curr_field = _def_field;
      else
        _curr_field = _holding_field;
      _curr_field->focus(true);
      retval = true;
      break;

    case KEY_DOWN:
    case KEY_TAB:
    case CTRL('N'):
      _curr_field->focus(false);
      if (_curr_field == _name_field)
        _curr_field = _def_field;
      else if (_curr_field == _def_field)
        _curr_field = _holding_field;
      else if (_curr_field == _holding_field)
        _curr_field = _status_field;
      else
        _curr_field = _name_field;
      _curr_field->focus(true);
      retval = true;
      break;
    }
  } else {
    switch (ch) {
    case KEY_UP:
    case CTRL('P'):
      if (_current_filter_idx > 0) {
        --_current_filter_idx;
        if (_current_filter_idx < _top_idx_vis) {
          --_top_idx_vis;
          _top_row_vis -= _display_rows[_top_idx_vis];
        }
        display();
      }
      retval = true;
      break;

    case KEY_DOWN:
    case CTRL('N'):
      if (_current_filter_idx < _filters.size() - 1) {
        ++_current_filter_idx;
        while (_current_filter_idx > _bot_idx_vis) {
          _top_row_vis += _display_rows[_top_idx_vis];
          ++_top_idx_vis;
          int rows_on_screen = 0;
          for (_bot_idx_vis = _top_idx_vis; _bot_idx_vis < _filters.size();
               ++_bot_idx_vis)
            if (rows_on_screen + _display_rows[_bot_idx_vis] < _h)
              rows_on_screen += _display_rows[_bot_idx_vis];
            else {
              --_bot_idx_vis;
              break;
            }
        }
        display();
      }
      retval = true;
      break;

    case KEY_NPAGE:
    case CTRL('V'):
      for (int idx = 0; idx < 4; ++idx) process_key(KEY_DOWN);
      retval = true;
      break;

    case KEY_PPAGE:
    case META('v'):
      for (int idx = 0; idx < 4; ++idx) process_key(KEY_UP);
      retval = true;
      break;
    }
  }
  return retval;
}


void FilterList::header_line(string msg)
{
  // Display top-line info.
  string blank(getmaxx(stdscr), ' ');
  mvaddstr(0, 0, blank.c_str());
  if (msg == "") {
    string title = "FILTER EDITOR";
    mvaddstr(0, (getmaxx(stdscr) - title.size()) / 2, title.c_str());
  } else {
    wattron(stdscr, A_BOLD);
    mvaddstr(0, 0, msg.c_str());
    wattroff(stdscr, A_BOLD);
  }
  wnoutrefresh(stdscr);
}


// Display/hide the form.


void FilterList::display(void)
{
  header_line();
  clear();
  frame();
  int row = 0;
  int num_width = 4;
  int count_width = 6;
  int h_s_width = 5;
  _main_width = getmaxx(stdscr) - num_width - count_width - 10 - h_s_width;
  int num_x = 2;
  _main_x = num_x + num_width + 2;
  int count_x = _w - 1 - count_width;
  _h_s_x = _main_x + _main_width + 2;
  bool can_go_down = false;
  _display_rows.clear();
  for (int idx = 0; idx < _filters.size(); ++idx) {
    string name = _filters[idx]->name();
    string def = _filters[idx]->definition();
    string holding = _filters[idx]->holding();
    string status = _filters[idx]->status();
    int article_count = _article_counts[idx];
    int name_rows = name.size() / _main_width;
    if (name.size() % _main_width != 0) ++name_rows;
    int def_rows = def.size() / _main_width;
    if (def.size() % _main_width != 0 || def_rows == 0) ++def_rows;
    int total_rows = name_rows + def_rows;
    _display_rows.push_back(total_rows + 1);

    if (row - _top_row_vis + total_rows >= _h - 1)
      can_go_down = true;
    else if (row >= _top_row_vis && !can_go_down) {
      // Draw entry frame.
      int top_tee = (idx == _top_idx_vis) ? ACS_TTEE : ACS_PLUS;
      mvwaddch(_win, row - _top_row_vis, _main_x - 2, top_tee);
      mvwvline(_win, row - _top_row_vis + 1, _main_x - 2,
               ACS_VLINE, total_rows);
      mvwaddch(_win, row - _top_row_vis, count_x - 2, top_tee);
      mvwvline(_win, row - _top_row_vis + 1, count_x - 2,
               ACS_VLINE, total_rows);
      mvwaddch(_win, row - _top_row_vis, _h_s_x - 3, top_tee);
      mvwvline(_win, row - _top_row_vis + 1, _h_s_x - 3,
               ACS_VLINE, total_rows);
      mvwhline(_win, row - _top_row_vis + total_rows + 1, 1,
               ACS_HLINE, _w - 2);
      if (row - _top_row_vis + total_rows + 1 < _h - 1) {
        mvwaddch(_win, row - _top_row_vis + total_rows + 1, 0, ACS_LTEE);
        mvwaddch(_win, row - _top_row_vis + total_rows + 1, _w - 1, ACS_RTEE);
      }
      mvwaddch(_win, row - _top_row_vis + total_rows + 1,
               _main_x - 2, ACS_BTEE);
      mvwaddch(_win, row - _top_row_vis + total_rows + 1,
               count_x - 2, ACS_BTEE);
      mvwaddch(_win, row - _top_row_vis + total_rows + 1,
               _h_s_x - 3, ACS_BTEE);

      // Fill in the values.
      int attr = (idx == _current_filter_idx) ? A_BOLD : A_NORMAL;
      char buff[10];
      sprintf(buff, "#%d", idx + 1);
      int num_pos = num_x;
      if (idx < 9) ++num_pos;
      int draw_row = row - _top_row_vis + 1;

      if (holding == "")
        writestr(_h_s_x - 1, draw_row,  "  ", attr);
      else
        writestr(_h_s_x - 1, draw_row, holding, attr);
      writestr(_h_s_x + 1, draw_row, "/", attr);
      if (status == "")
        writestr(_h_s_x + 2, draw_row, " ", attr);
      else
        writestr(_h_s_x + 2, draw_row, status, attr);

      writestr(num_pos, draw_row, buff, attr);
      sprintf(buff, "%4d", article_count);
      writestr(count_x, draw_row, buff, attr);
      while (name.size() > _main_width) {
        writestr(_main_x, draw_row, name.substr(0, _main_width), attr);
        name = name.substr(_main_width + 1);
        ++draw_row;
      }
      writestr(_main_x, draw_row, name, attr);
      ++draw_row;
      while (def.size() > _main_width) {
        writestr(_main_x, draw_row, def.substr(0, _main_width), attr);
        def = def.substr(_main_width + 1);
        ++draw_row;
      }
      writestr(_main_x, draw_row, def, attr);
    }
    row += total_rows + 1;
  }
  int rows_on_screen = 0;
  for (_bot_idx_vis = _top_idx_vis; _bot_idx_vis < _filters.size();
       ++_bot_idx_vis)
    if (rows_on_screen + _display_rows[_bot_idx_vis] < _h)
      rows_on_screen += _display_rows[_bot_idx_vis];
    else {
      --_bot_idx_vis;
      break;
    }
  scrollable_up(_top_idx_vis != 0);
  scrollable_down(can_go_down);
  wnoutrefresh(_win);
}


Filter *FilterList::current_filter(void) const
{
  if (_filters.size() > 0)
    return _filters[_current_filter_idx];
  else
    return 0;
}

void FilterList::add_filter(void)
{
  _filters.push_back(new Filter(_db));
  _article_counts.push_back(0);
  _display_rows.push_back(3);
  _current_filter_idx = _filters.size() - 1;

  if (_filters.size() == 1)
    _top_idx_vis = _bot_idx_vis = 0;
  else {
    _bot_idx_vis = _current_filter_idx;
    int row = _h - 1;
    _top_idx_vis = _bot_idx_vis;
    do {
      row -= _display_rows[_top_idx_vis];
      --_top_idx_vis;
    } while (row - _display_rows[_top_idx_vis] > 0 && _top_idx_vis > 0);
  }

  _top_row_vis = 0;
  for (int idx = 0; idx < _top_idx_vis; ++idx)
    _top_row_vis += _display_rows[idx];

  display();

  string new_name, new_def, new_holding, new_status;
  if (run_edit(new_name, new_def, new_holding, new_status)) {
    _filters[_current_filter_idx]->set_name(new_name);
    _filters[_current_filter_idx]->set_definition(new_def);
    if (new_holding != "")
      _filters[_current_filter_idx]->set_holding(new_holding);
    else
      _filters[_current_filter_idx]->clear_holding();
    if (new_status != "")
      _filters[_current_filter_idx]->set_status(new_status);
    else
      _filters[_current_filter_idx]->clear_status();
    DocMgr::Query q = _filters[_current_filter_idx]->query();
    vector<DocMgr::DocID> ids;
    q.run(ids);
    _article_counts[_current_filter_idx] = ids.size();
    _config->add_filter(_filters[_current_filter_idx]->name(),
                        _filters[_current_filter_idx]->definition(),
                        _filters[_current_filter_idx]->has_holding() ?
                        _filters[_current_filter_idx]->holding() : "",
                        _filters[_current_filter_idx]->has_status() ?
                        _filters[_current_filter_idx]->status() : "");
    _config->save();
  } else
    delete_current(false);

  display();
}

void FilterList::edit_current(void)
{
  if (_filters.size() == 0) return;

  string new_name = _filters[_current_filter_idx]->name();
  string new_def = _filters[_current_filter_idx]->definition();
  string new_holding = _filters[_current_filter_idx]->holding();
  string new_status = _filters[_current_filter_idx]->status();
  if (run_edit(new_name, new_def, new_holding, new_status)) {
    _filters[_current_filter_idx]->set_name(new_name);
    _filters[_current_filter_idx]->set_definition(new_def);
    if (new_holding != "")
      _filters[_current_filter_idx]->set_holding(new_holding);
    else
      _filters[_current_filter_idx]->clear_holding();
    if (new_status != "")
      _filters[_current_filter_idx]->set_status(new_status);
    else
      _filters[_current_filter_idx]->clear_status();
    DocMgr::Query q = _filters[_current_filter_idx]->query();
    vector<DocMgr::DocID> ids;
    q.run(ids);
    _article_counts[_current_filter_idx] = ids.size();
    _config->set_filter_name
      (_current_filter_idx, _filters[_current_filter_idx]->name());
    _config->set_filter_definition
      (_current_filter_idx, _filters[_current_filter_idx]->definition());
    _config->set_filter_holding
      (_current_filter_idx,
       _filters[_current_filter_idx]->has_holding() ?
       _filters[_current_filter_idx]->holding() : "");
    _config->set_filter_status
      (_current_filter_idx,
       _filters[_current_filter_idx]->has_status() ?
       _filters[_current_filter_idx]->status() : "");
    _config->save();
  }
  display();
}

void FilterList::delete_current(bool delete_from_config)
{
  if (_filters.size() > 0) {
    if (delete_from_config) {
      _config->delete_filter(_current_filter_idx);
      _config->save();
    }
    delete _filters[_current_filter_idx];
    _filters.erase(_filters.begin() + _current_filter_idx);
    _article_counts.erase(_article_counts.begin() + _current_filter_idx);
    _display_rows.erase(_display_rows.begin() + _current_filter_idx);
    if (_current_filter_idx != 0 && _current_filter_idx >= _filters.size())
      --_current_filter_idx;
    if (_current_filter_idx < _top_idx_vis) {
      --_top_idx_vis;
      _top_row_vis = 0;
      for (int idx = 0; idx < _top_idx_vis; ++idx)
        _top_row_vis += _display_rows[idx];
    }
  }
  display();
}

void FilterList::combine_filters(void)
{
  if (_filters.size() == 0) return;

  FilterCombinationDialogue combine_dialogue;
  int filter_no1, filter_no2;
  bool and_combine;
  if (combine_dialogue.run(filter_no1, filter_no2, _filters.size(),
                           and_combine)) {
    _filters.push_back(new Filter(_db));
    _article_counts.push_back(0);
    _display_rows.push_back(3);
    _current_filter_idx = _filters.size() - 1;

    string def1 = string("(") + _filters[filter_no1 - 1]->definition() + ")";
    string def2 = string("(") + _filters[filter_no2 - 1]->definition() + ")";
    string combination = and_combine ? " & " : " | ";
    string new_def = def1 + combination + def2;

    _filters[_current_filter_idx]->set_name("COMBINED FILTER");
    _filters[_current_filter_idx]->set_definition(new_def);
    if (_filters[filter_no1 - 1]->has_holding())
      _filters[_current_filter_idx]->set_holding
        (_filters[filter_no1 - 1]->holding());
    else
      _filters[_current_filter_idx]->clear_holding();
    if (_filters[filter_no1 - 1]->has_status())
      _filters[_current_filter_idx]->set_status
        (_filters[filter_no1 - 1]->status());
    else
      _filters[_current_filter_idx]->clear_status();
    DocMgr::Query q = _filters[_current_filter_idx]->query();
    vector<DocMgr::DocID> ids;
    q.run(ids);
    _article_counts[_current_filter_idx] = ids.size();
    _config->add_filter(_filters[_current_filter_idx]->name(),
                        _filters[_current_filter_idx]->definition(),
                        _filters[_current_filter_idx]->holding(),
                        _filters[_current_filter_idx]->status());
    _config->save();

    string name = _filters[_current_filter_idx]->name();
    string def = _filters[_current_filter_idx]->definition();
    string holding = _filters[_current_filter_idx]->holding();
    string status = _filters[_current_filter_idx]->status();
    int name_rows = name.size() / _main_width;
    if (name.size() % _main_width != 0) ++name_rows;
    int def_rows = def.size() / _main_width;
    if (def.size() % _main_width != 0 || def_rows == 0) ++def_rows;
    _display_rows[_current_filter_idx] = name_rows + def_rows + 1;

    _bot_idx_vis = _current_filter_idx;
    int row = _h - 1;
    _top_idx_vis = _bot_idx_vis;
    do {
      row -= _display_rows[_top_idx_vis];
      --_top_idx_vis;
    } while (row - _display_rows[_top_idx_vis] > 0 && _top_idx_vis > 0);

    _top_row_vis = 0;
    for (int idx = 0; idx < _top_idx_vis; ++idx)
      _top_row_vis += _display_rows[idx];
  }

  display();
}


bool FilterList::run_edit(string &new_name, string &new_def,
                          string &new_holding, string &new_status)
{
  string old_name = new_name, old_def = new_def;
  string old_holding = new_holding, old_status = new_status;
  _editing_menu.display();
  curs_set(1);
  int field_row = 0;
  for (int idx = _top_idx_vis; idx < _current_filter_idx; ++idx)
    field_row += _display_rows[idx];
  field_row += 2;
  _name_field = new TextField(_main_x + 1, field_row,
                              -(_main_width - 4), new_name);
  _def_field = new TextField(_main_x + 1, field_row + 1,
                             _main_width - 4, new_def);
  vector<string> valid_holdings = DocMgr::Holding::valid_holdings();
  valid_holdings.push_back("");
  vector<string> valid_statuses = DocMgr::Status::valid_statuses();
  valid_statuses.push_back("");
  _holding_field =
    new SpinField(_h_s_x, field_row, 2, valid_holdings, new_holding);
  _status_field =
    new SpinField(_h_s_x + 3, field_row, 2, valid_statuses, new_status);
  if (new_name == "")
    _curr_field = _name_field;
  else
    _curr_field = _def_field;
  _curr_field->focus(true);
  _editing = true;
  bool saved = false;
  bool done = false;
  InteractorList responders;
  while (!done) {
    _editing_menu.clear_event();
    while (!_editing_menu.activated()) {
      if (_curr_field != _name_field) _name_field->display();
      if (_curr_field != _def_field) _def_field->display();
      if (_curr_field != _holding_field) _holding_field->display();
      if (_curr_field != _status_field) _status_field->display();
      _curr_field->display();
      doupdate();
      responders.clear();
      responders.push_back(*_curr_field);
      responders.push_back(_editing_menu);
      responders.push_back(*this);
      responders.process_key();
    }

    if (_editing_menu.action() == 1)
      // Just cancel with no questions.
      done = true;
    else {
      bool bad_edit = false;
      while (isspace(new_def[0])) new_def.erase(0, 1);
      while (isspace(new_def[new_def.size() - 1]))
        new_def.erase(new_def.size() - 1);
      new_holding = _holding_field->result();
      new_status = _status_field->result();
      if (new_def == "" && new_holding == "" && new_status == "")
        bad_edit = true;
      else {
        Filter *tmp_filter = new Filter(_db);
        try {
          tmp_filter->set_definition(new_def);
        } catch (Filter::Exception &exc) {
          bad_edit = true;
        }
        delete tmp_filter;
      }

      if (bad_edit)
        header_line("SYNTAX ERROR IN FILTER DEFINITION!");
      else {
        saved = true;
        done = true;
      }
    }

  }

  delete _name_field;
  delete _def_field;
  delete _holding_field;
  delete _status_field;
  curs_set(0);
  if (!saved) {
    new_name = old_name;
    new_def = old_def;
    new_holding = old_holding;
    new_status = old_status;
  }
  _editing = false;
  return saved;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
