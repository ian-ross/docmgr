//----------------------------------------------------------------------
//
//  FILE:   ArticleEditForm.cpp
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Concrete class for read-only article views.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Local headers.

#include "ArticleEditForm.hh"
#include "InteractorList.hh"
#include "Menu.hh"
#include "MultiLineTextField.hh"

//----------------------------------------------------------------------
//
//  CONSTANT DEFINITIONS
//
//----------------------------------------------------------------------

const int SAVE_ACTION = 1;
const int CANCEL_ACTION = 2;


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

DocMgr::DocID ArticleEditForm::add_entry(DocMgr::DocType type)
{
  _doc = new DocMgr::DocRecord(*_db, type);
  _new_entry = true;
  return run();
}


void ArticleEditForm::edit(DocMgr::DocID id)
{
  _doc = _db->get_doc_by_id(id);
  _new_entry = false;
  run();
}


DocMgr::DocID ArticleEditForm::run(void)
{
  clear();
  frame();

  _header_width = field_header_width(false);
  _contents_space = _w - _header_width - 4;

  Menu action_menu(0, getmaxy(stdscr) - 1, getmaxx(stdscr));
  action_menu.add_item("&Save",   SAVE_ACTION);
  action_menu.add_item("&Revert", CANCEL_ACTION);
  action_menu.display();

  map<DocMgr::FieldType, string> &fields = _doc->fields();
  int row = 0;
  for (int fld = 0; _field_order[fld].id[0]; ++fld) {
    map<DocMgr::FieldType, string>::iterator it =
      fields.find(DocMgr::FieldType(*_db, _field_order[fld].id));
    if (it == fields.end()) continue;
    string field_type = it->first;
    int field_width = _contents_space;
    EditField *field;
    bool multi_line = false;

    if (field_type == "AD" || field_type == "BT" || field_type == "ED" ||
        field_type == "NT" || field_type == "TI" || field_type == "AU" ||
        field_type == "KW" || field_type == "SE") {
      // Multi-line fields.

      if (field_type == "AU") {
        // Author output is treated a little bit specially, just to
        // get rid of the "X and Y and Z and ..." format from the
        // database.
        for (;;) {
          string::size_type and_pos = it->second.find(" and ");
          if (and_pos == string::npos) break;
          it->second.replace(and_pos, 5, ", ");
        }
      }
      field =
        new MultiLineTextField(_x + _header_width + 2, _y + 3 + row,
                               field_width, it->second);
      multi_line = true;
      row += field->_h;
    } else {
      // Single-line fields, some scrollable, some fixed.

      if (field_type == "YR")
        field_width = -5;
      else if (field_type == "MO" || field_type == "VO" ||
               field_type == "CA" || field_type == "EN")
        field_width = -10;
      else if (field_type == "NO")
        field_width = 10;
      else if (field_type == "PG")
        field_width = 35;
      else if (field_type == "LA")
        field_width = -20;
      else if (field_type == "IS")
        field_width = -15;
      field = new TextField(_x + _header_width + 2, _y + 3 + row++,
                            field_width, it->second);
    }
    _edit_fields[it->first] = field;
    _multiline_fields[it->first] = multi_line;
    _field_list.push_back(field);
  }
  _holding_field =
    new SpinField(_x + _w - 6, 2, 2,
                  DocMgr::Holding::valid_holdings(), _doc->holding());
  _field_list.push_back(_holding_field);
  _status_field =
    new SpinField(_x + _w - 3, 2, 1,
                  DocMgr::Status::valid_statuses(), _doc->status());
  _field_list.push_back(_status_field);

  DocMgr::DocID retval;
  InteractorList responders;
  Widget tmp_widget(0, 0, 0, 1);
  _curr_field = _field_list.begin();
  curs_set(1);
  (*_curr_field)->focus(true);
  bool completed = false;
  while (!completed) {
    action_menu.clear_event();
    while (!action_menu.activated()) {
      display();
      doupdate();
      responders.clear();
      responders.push_back(**_curr_field);
      responders.push_back(action_menu);
      responders.push_back(*this);

      EditField *before_field = *_curr_field;
      int before_height = (*_curr_field)->_h;
      responders.process_key();
      int after_height = (*_curr_field)->_h;
      if (*_curr_field == before_field && before_height != after_height) {
        int dy = after_height - before_height;
        bool found = false;
        map<DocMgr::FieldType, string> &fields = _doc->fields();
        for (int fld = 0; _field_order[fld].id[0]; ++fld) {
          map<DocMgr::FieldType, string>::const_iterator it =
            fields.find(DocMgr::FieldType(*_db, _field_order[fld].id));
          if (it == fields.end()) continue;
          EditField *field =
            _edit_fields[DocMgr::FieldType(*_db, _field_order[fld].id)];
          if (field == *_curr_field) { found = true;  continue; }
          if (!found) continue;
          field->move(dy);
        }
      }
    }

    if (action_menu.action() == CANCEL_ACTION)
      // Just cancel with no questions.
      completed = true;
    else {
      // Check mandatory fields are OK.
      list<DocMgr::FieldType> bad_field_types;
      if (_doc->mandatory_fields_ok(bad_field_types)) {
        // Get the holding and status values.
        _doc->set_holding(_holding_field->result());
        _doc->set_status(_status_field->result());

        // Fix the author and editors fields.
        if (_doc->fields().find(DocMgr::FieldType(*_db, "AU")) !=
            _doc->fields().end()) {
          string authors = _doc->fields()[DocMgr::FieldType(*_db, "AU")];
          for (;;) {
            string::size_type comma_pos = authors.find(",");
            if (comma_pos == string::npos) break;
            authors.replace(comma_pos, 1, " and ");
          }
          for (;;) {
            string::size_type sspace_pos = authors.find("  ");
            if (sspace_pos == string::npos) break;
            authors.replace(sspace_pos, 2, " ");
          }
          _doc->set_field(DocMgr::FieldType(*_db, "AU"), authors);
        }
        if (_doc->fields().find(DocMgr::FieldType(*_db, "ED")) !=
            _doc->fields().end()) {
          string editors = _doc->fields()[DocMgr::FieldType(*_db, "ED")];
          for (;;) {
            string::size_type comma_pos = editors.find(",");
            if (comma_pos == string::npos) break;
            editors.replace(comma_pos, 1, " and ");
          }
          for (;;) {
            string::size_type sspace_pos = editors.find("  ");
            if (sspace_pos == string::npos) break;
            editors.replace(sspace_pos, 2, " ");
          }
          _doc->set_field(DocMgr::FieldType(*_db, "ED"), editors);
        }

        if (_new_entry)
          // Save new entry.
          _doc->intern();
        else
          _doc->update();
          retval = _doc->id();
          completed = true;
      } else {
        // Display error message.
        string mandatory_field_message;
        if (bad_field_types.size() == 1) {
          string fname = ArticleForm::field_name(bad_field_types.front());
          mandatory_field_message = "FIELD '" + fname + "' MUST BE FILLED IN";
        } else {
          string fnames;
          list<DocMgr::FieldType>::iterator it = bad_field_types.begin();
          while (it != bad_field_types.end()) {
            fnames += "'";
            fnames += ArticleForm::field_name(*it);
            fnames += "'";
            ++it;
            if (it == bad_field_types.end())
              fnames += " OR ";
            else
              fnames += ", ";
            fnames += ArticleForm::field_name(*it);
          }
          mandatory_field_message = "ONE OF FIELDS " + fnames +
            " MUST BE FILLED IN";
        }
        tmp_widget.clear();
        tmp_widget.writestr(0, 0, mandatory_field_message, A_BOLD);
      }
    }
  }
  curs_set(0);

  for (map<DocMgr::FieldType, EditField *>::iterator it = _edit_fields.begin();
       it != _edit_fields.end(); ++it)
    delete it->second;
  _edit_fields.clear();
  _field_list.clear();
  delete _doc;
  _doc = 0;

  return retval;
}




// Display the form.

void ArticleEditForm::display(void)
{
  // Clear the display area and draw the frame (including the article
  // ID).
  clear();
  frame(false);

  // If there's no current document, there's nothing else to do.
  if (!_doc) return;

  string header_blanking(_header_width, ' ');
  map<DocMgr::FieldType, string> &fields = _doc->fields();
  int row = 0, curr_field_row;
  for (int fld = 0; _field_order[fld].id[0]; ++fld) {
    string id = _field_order[fld].id;
    map<DocMgr::FieldType, string>::const_iterator it =
      fields.find(DocMgr::FieldType(*_db, _field_order[fld].id));
    bool multiline =
      _multiline_fields[DocMgr::FieldType(*_db, _field_order[fld].id)];
    if (it == fields.end()) continue;
    EditField *field =
      _edit_fields[DocMgr::FieldType(*_db, _field_order[fld].id)];
    if (multiline) {
      MultiLineTextField *mlf = dynamic_cast<MultiLineTextField *>(field);
      int x = fld + 2;
    }
    if (field == *_curr_field) curr_field_row = row;
    if (row >= _h - 4)
      row += field->lines();
    else {
      writestr(2, row + 3, header_blanking);
      writestr(2, row + 3, string(_field_order[fld].name) + ":");
      if (field != *_curr_field) {
        if (row + field->lines() - 1 >= _h - 4)
          field->display(_h - 4 - row);
        else 
          field->display();
      }
      row += field->lines();
    }
  }
  _total_rows = row;
  if (*_curr_field != _holding_field) _holding_field->display();
  if (*_curr_field != _status_field) _status_field->display();
  if (curr_field_row + (*_curr_field)->lines() - 1 >= _h - 4)
    (*_curr_field)->display(_h - 4 - curr_field_row);
  else 
    (*_curr_field)->display();
}


bool ArticleEditForm::row_ok(int row)
{
  return (row >= 0 && row < _h - 4);
}


// Process a single key event.

bool ArticleEditForm::process_key(int ch)
{
  bool retval = false;
  switch(ch) {
  case KEY_TAB:
  case KEY_DOWN:
  case CTRL('N'):
    // Go to next field in form.
    (*_curr_field)->focus(false);
    ++_curr_field;
    if (_curr_field == _field_list.end())
      _curr_field = _field_list.begin();
    while ((*_curr_field) != _holding_field &&
           (*_curr_field) != _status_field &&
           (*_curr_field)->_y + (*_curr_field)->lines() - 1 > _y + _h - 2) {
      ++_curr_field;
      if (_curr_field == _field_list.end())
        _curr_field = _field_list.begin();
    }
    (*_curr_field)->focus(true);
    retval = true;
    break;

  case KEY_SHIFT_TAB:
  case KEY_UP:
  case CTRL('P'):
    // Go to previous field in form.
    (*_curr_field)->focus(false);
    if (_curr_field == _field_list.begin())
      _curr_field = _field_list.end();
    --_curr_field;
    while ((*_curr_field) != _holding_field &&
           (*_curr_field) != _status_field &&
           (*_curr_field)->_y + (*_curr_field)->lines() - 1 > _y + _h - 2) {
      if (_curr_field == _field_list.begin())
        _curr_field = _field_list.end();
      --_curr_field;
    }
    (*_curr_field)->focus(true);
    retval = true;
    break;
  }
  return retval;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
