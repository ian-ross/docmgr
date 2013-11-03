//----------------------------------------------------------------------
//
//  FILE:   Importer.cpp
//  AUTHOR: Ian Ross
//  DATE:   02-OCT-2005
//
//----------------------------------------------------------------------
//
//  ISI format article entry importer.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>

using namespace std;

// Local headers.

#include "Importer.hh"


//----------------------------------------------------------------------
//
//  LOCAL FUNCTION PROTOTYPES
//
//----------------------------------------------------------------------

static string trim_string(string s);


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

Importer::Importer(DocMgr::Connection &db, string file_name) :
  _file_name(file_name), _valid(true)
{
  string in_file = _file_name;
  if (in_file[0] == '~') {
    string home = getenv("HOME");
    in_file = home + in_file.substr(1);
  }
  ifstream istr(in_file.c_str());
  if (!istr) {
    _valid = false;
    _failure_reason = string("Couldn't open import file ") + _file_name;
    return;
  }
  char buff_chars[1024];
  istr.getline(buff_chars, 1024);
  string header1 = buff_chars;
  while (!isprint(header1[0])) header1 = header1.substr(1);
  istr.getline(buff_chars, 1024);
  string header2 = buff_chars;
  if (header1.substr(0, 3) != "FN " || header2 != "VR 1.0") {
    _valid = false;
    _failure_reason = "Import file isn't in ISI V1.0 format";
    return;
  }

  map<string, string> fields;
  string last_tag;
  while (!istr.eof()) {
    istr.getline(buff_chars, 1024);
    string buff = buff_chars;
    if (buff.size() == 0 || buff == "EF") continue;
    string tag = buff.substr(0, 2);
    string val = buff.size() > 3 ? buff.substr(3) : "";
    if (tag == "ER") {
      DocMgr::DocRecord *rec = DocMgr::DocRecord::isi_import(db, fields);
      if (!rec) {
        _valid = false;
        _failure_reason = "Syntax error in import file";
        return;
      }
      _docs.push_back(rec);
      last_tag = "";
    } else {
      if (tag == "  ") {
        if (last_tag == "AU")
          fields[last_tag] += ';';
        else
          fields[last_tag] += ' ';
        fields[last_tag] += trim_string(val);
      } else {
        fields[tag] = val;
        last_tag = tag;
      }
    }
  }
}


Importer::~Importer()
{
  for (vector<DocMgr::DocRecord *>::iterator it = _docs.begin();
       it != _docs.end(); ++it)
    delete *it;
  _docs.clear();
}


//----------------------------------------------------------------------
//
//  LOCAL FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

static string trim_string(string s)
{
  string::size_type start = s.find_first_not_of(" \t\n");
  if (start == string::npos) return "";
  string::size_type end = s.find_last_not_of(" \t\n");
  return s.substr(start, end - start + 1);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
