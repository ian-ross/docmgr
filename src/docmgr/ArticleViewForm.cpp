//----------------------------------------------------------------------
//
//  FILE:   ArticleViewForm.cpp
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

#include "ArticleViewForm.hh"
#include "InteractorList.hh"

//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

// Display the form.

void ArticleViewForm::display(void)
{
  // Clear the display area and draw the frame (including the article
  // ID).
  clear();
  frame();

  // If there's no current document, there's nothing else to do.
  if (!_doc) return;

  int header_width = field_header_width(true);
  int contents_space = _w - header_width - 4;
  int min_break_pos = contents_space * 3 / 4;
  bool displaying_xref = false;
  map<DocMgr::FieldType, string> *fields = &(_doc->fields());
  int row = 0;
  for (int fld = 0; _field_order[fld].id[0]; ++fld) {
    map<DocMgr::FieldType, string>::const_iterator it =
      fields->find(DocMgr::FieldType(*_db, _field_order[fld].id));
    if (it == fields->end() || it->second == "") continue;
    if (string(it->first) == "XR") {
      // Handle cross-reference display: the XR field is always
      // displayed last, so just replace 'fields' with the
      // cross-reference document's fields, put some spiel in between
      // the last main document field and the first cross-reference
      // document field and continuing displaying fields from the
      // beginning of the ordering table...

      if (_xref_doc) {
        displaying_xref = true;
        fields = &(_xref_doc->fields());

        ++row;
        if (row_ok(row)) {
          mvwhline(_win, _y + row + 2, 1, ACS_HLINE, _w - 2);
          mvwaddch(_win, _y + row + 2, 0, ACS_LTEE);
          mvwaddch(_win, _y + row + 2, _w - 1, ACS_RTEE);
          string heading = string(" XREF: ") + string(_xref_doc->id()) + " ";
          mvwaddstr(_win, _y + row + 2, 2, heading.c_str());
        }
        row += 2;
        fld = 0;
        continue;
      }
    }
    if (row_ok(row))
      writestr(2, row + 3, string(_field_order[fld].name) + ":");
    string contents = it->second;
    if (string(it->first) == "XR") {
      // If a cross reference gets to this point, it's missing.
      contents += " (MISSING)";
    } else if (string(it->first) == "AU") {
      // Author output is treated a little bit specially, just to get
      // rid of the "X and Y and Z and ..." format from the database.
      for (;;) {
        string::size_type and_pos = contents.find(" and ");
        if (and_pos == string::npos) break;
        contents.replace(and_pos, 5, ", ");
      }
    }
    while (contents.size() > contents_space) {
      int line_len = contents_space, cut_pos = contents_space;
      if (contents[contents_space] == ' ') {
        line_len = contents_space;
        cut_pos = contents_space + 1;
      } else
        for (int idx = contents_space - 1; idx > min_break_pos; --idx)
          if (contents[idx] == ' ') {
            line_len = idx;
            cut_pos = idx + 1;
            break;
          }

      if (row_ok(row))
        writestr(2 + header_width, row + 3,
                 contents.substr(0, line_len));
      ++row;
      contents = contents.substr(cut_pos);
    }
    if (row_ok(row))
      writestr(2 + header_width, row + 3, contents);
    ++row;
  }
  _total_rows = row;
}


bool ArticleViewForm::row_ok(int row)
{
  return (row >= 0 && row < _h - 4);
}


// Set the current document ID.

void ArticleViewForm::set_current(DocMgr::DocID id)
{
  delete _doc;       _doc = 0;
  delete _xref_doc;  _xref_doc = 0;
  if (!id.valid())
    _doc = 0;
  else {
    _doc = _db->get_doc_by_id(id);
    map<DocMgr::FieldType, string> &fields = _doc->fields();
    map<DocMgr::FieldType, string>::iterator it =
      fields.find(DocMgr::FieldType(*_db, "XR"));
    if (it == fields.end() || !DocMgr::DocID::valid(it->second))
      _xref_doc = 0;
    else {
      try {
        DocMgr::DocID xref_id = it->second;
        _xref_doc = _db->get_doc_by_id(xref_id);
      } catch (DocMgr::Exception &exc) {
        if (exc.type() != DocMgr::Exception::DOCID_NOT_FOUND) throw;
      }
    }
  }

  _total_rows = 0;
  display();
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
