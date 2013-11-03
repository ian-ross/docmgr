//----------------------------------------------------------------------
//
//  FILE:   ArticleForm.cpp
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Abstract base class for both read-only and editable article views.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Curses header.

#include <curses.h>


// Local header.

#include "ArticleForm.hh"


//----------------------------------------------------------------------
//
//  STATIC MEMBER INITIALISERS
//
//----------------------------------------------------------------------

const ArticleForm::FieldDesc ArticleForm::_field_order[] = {
  { "AU", "Author" },
  { "TI", "Title" },

  { "YR", "Year" },
  { "MO", "Month" },

  { "JN", "Journal" },
  { "VO", "Volume" },
  { "NO", "Number" },
  { "PG", "Pages" },

  { "BT", "Book Title" },
  { "ED", "Editor" },
  { "EN", "Edition" },
  { "CA", "Chapter" },
  { "SE", "Series" },

  { "PU", "Publisher" },
  { "OG", "Organization" },
  { "IN", "Institution" },
  { "SH", "School" },
  { "AD", "Address" },

  { "HO", "How Published" },
  { "LA", "Language" },
  { "TY", "Type" },

  { "IS", "ISBN" },
  { "EP", "E-print" },
  { "UR", "URL" },

  { "KW", "Keywords" },
  { "NT", "Note" },
  { "TG", "Tags" },

  { "XR", "Cross Ref." },

  { "", "" }
};


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

// An article form is fixed in position, and permanently associated
// with a database connection and a CDK screen.

ArticleForm::ArticleForm(DocMgr::Connection *db, int x, int y, int w, int h) :
  Widget(x, y, w, h), _db(db), _doc(0)
{ }


void ArticleForm::frame(bool display_holding)
{
  Widget::frame();
  mvwhline(_win, 2, 0, 0, getmaxx(_win));
  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, getmaxx(_win) - 1, ACS_RTEE);
  if (_doc) {
    string id_str = string("ID: ");
    if (_doc->id().valid())
      id_str += string(_doc->id());
    else
      id_str += "(NEW DOC.)";
    id_str += string(" [") + _doc->type().name() + "]";
    writestr(2, 1, id_str);
    if (display_holding) {
      string hold_str = string(_doc->holding()) + "/" + string(_doc->status());
      writestr(_w - hold_str.length() - 2, 1, hold_str);
    } else {
      string hold_str = "  / ";
      writestr(_w - hold_str.length() - 2, 1, hold_str);
    }
  }
  wnoutrefresh(_win);
}


int ArticleForm::field_header_width(bool present_only)
{
  if (!_doc)
    throw DocMgr::Exception(DocMgr::Exception::MISC,
                            "ArticleForm::field_header_width");
  map<DocMgr::FieldType, string> &fields = _doc->fields();
  int retval = 0;
  for (map<DocMgr::FieldType, string>::const_iterator it = fields.begin();
       it != fields.end(); ++it) {
    int order = field_order(it->first);
    string header = _field_order[order].name;
    if (header.size() > retval)
      if (!present_only || it->second.size() > 0)
        retval = header.size();
  }
  return retval + 2;
}


int ArticleForm::field_order(DocMgr::FieldType type)
{
  string type_str = string(type);
  for (int idx = 0; _field_order[idx].id[0]; ++idx)
    if (_field_order[idx].id == type_str)
      return idx;
  throw DocMgr::Exception(DocMgr::Exception::MISC,
                          "ArticleForm::field_order");
}


string ArticleForm::field_name(DocMgr::FieldType type)
{
  string type_str = string(type);
  for (int idx = 0; _field_order[idx].id[0]; ++idx)
    if (_field_order[idx].id == type_str)
      return _field_order[idx].name;
  throw DocMgr::Exception(DocMgr::Exception::MISC,
                          "ArticleForm::field_name");
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
