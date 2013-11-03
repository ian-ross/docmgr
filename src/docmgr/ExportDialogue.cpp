//----------------------------------------------------------------------
//
//  FILE:   ExportDialogue.cpp
//  AUTHOR: Ian Ross
//  DATE:   25-SEP-2008
//
//----------------------------------------------------------------------
//
//  Export document dialogue.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Local headers.

#include "ExportDialogue.hh"
#include "InteractorList.hh"
#include "TextField.hh"


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

ExportDialogue::ExportDialogue(DocMgr::Connection &db) :
  Widget(0, 0, getmaxx(stdscr), getmaxy(stdscr)), _db(db)
{ }


bool ExportDialogue::process_key(int ch)
{
  _completed = true;
  return true;
}


bool ExportDialogue::run(DocMgr::DocID curr_doc)
{
  bool retval = false;

  clear();

  DocMgr::DocRecord *doc = _db.get_doc_by_id(curr_doc);
  string export_fmt = format_current_doc(doc);
  delete doc;

  int lineidx = 0;
  while (export_fmt.size() > getmaxx(stdscr) - 2) {
    string line = export_fmt.substr(0, getmaxx(stdscr) - 2);
    string::size_type eol = line.find('\n');
    if (eol != string::npos) {
      line = export_fmt.substr(0, eol);
    } else {
      while (line[line.size() - 1] != ' ')
        line.erase(line.size() - 1);
      line.erase(line.size() - 1);
    }
    export_fmt = export_fmt.substr(line.size() + 1);
    writestr(1, lineidx, line);
    ++lineidx;
  }
  writestr(1, lineidx, export_fmt);

  doupdate();

  InteractorList interaction;
  interaction.push_back(*this);

  doupdate();
  _completed = false;
  while (!_completed) { interaction.process_key(); doupdate(); }

  clear();

  return retval;
}


string ExportDialogue::format_current_doc(DocMgr::DocRecord *doc)
{
  string retval;

  retval += string("ID: ") + string(doc->id()) + "  HOLDING: "
    + string(doc->holding())
    + "  STATUS: " + string(doc->status()) + "\n\n";

  // AU (YR). TI. JN VO(IS), BP-EP.
  DocMgr::FieldType au(_db, "AU"), yr(_db, "YR"), ti(_db, "TI");
  DocMgr::FieldType jn(_db, "JN"), vo(_db, "VO"), no(_db, "NO"), pg(_db, "PG");

  map<DocMgr::FieldType, string> flds = doc->fields();
  if (flds[au] != "") {
    retval += format_author(flds[au]);
    if (flds[yr] != "") retval += string(" (") + flds[yr] + ")";
    retval += string(". ");
  }
  retval += flds[ti] + ".";
  if (flds[jn] != "") {
    retval += string(" ") + flds[jn];
    if (flds[vo] != "") {
      retval += string(" ") + flds[vo];
      if (flds[no] != "") retval += string("(") + flds[no] + ")";
      if (flds[pg] != "") retval += string(", ") + flds[pg];
      retval += string(".");
    }
  }

  return retval;
}


string ExportDialogue::format_author(string auth)
{
  vector<string> authors;
  string::size_type sep = auth.find(" and ");
  while (sep != string::npos) {
    authors.push_back(auth.substr(0, sep));
    auth = auth.substr(sep + 5);
    sep = auth.find(" and ");
  }
  authors.push_back(auth);
  string retval;
  switch (authors.size()) {
  case 1:
    retval = authors[0];
    break;
  case 2:
    retval = authors[0] + " & " + authors[1];
    break;
  default:
    retval = authors[0] + " et al.";
    break;
  }
  return retval;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
