//----------------------------------------------------------------------
//
//  FILE:   Configuration.cpp
//  AUTHOR: Ian Ross
//  DATE:   02-OCT-2005
//
//----------------------------------------------------------------------
//
//  Configuration file reader/writer.
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

using namespace std;


// Local headers.

#include "DocMgr.hh"
#include "Configuration.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

Configuration::Configuration(string path) :
  _path(path), _show_deleted(false),
 _paper_directory("/papers"), _view_command("acroread %s")
{
  if (_path[0] == '~') {
    string home = getenv("HOME");
    _path = home + _path.substr(1);
  }
  reread();
}


void Configuration::reread(void)
{
  ifstream istr(_path.c_str());
  if (!istr) return;
  char buff_chars[1024];
  _filter_names.clear();
  _filter_defs.clear();
  _filter_holding.clear();
  _filter_status.clear();
  while (!istr.eof()) {
    istr.getline(buff_chars, 1024);
    string buff = buff_chars;
    while (isspace(buff[0])) buff.erase(0, 1);
    while (isspace(buff[buff.size() - 1])) buff.erase(buff.size() - 1);
    if (buff.size() == 0) continue;
    string::size_type colon = buff.find(':');
    if (colon == string::npos)
      throw DocMgr::Exception(DocMgr::Exception::MISC,
                              "Syntax error in config. file");
    string kw = buff.substr(0, colon);
    string val = buff.substr(colon + 1);
    while (isspace(kw[kw.size() - 1])) kw.erase(kw.size() - 1);
    while (isspace(val[0])) val.erase(0, 1);

    if (kw == "SHOW_DELETED") {
      if (val == "true" || val == "TRUE")
        _show_deleted = true;
      else if (val == "false" || val == "FALSE")
        _show_deleted = false;
      else
        throw DocMgr::Exception(DocMgr::Exception::MISC,
                                "Bad boolean value in config. file");
    } else if (kw == "PAPER_DIRECTORY") _paper_directory = val;
    else if (kw == "VIEW_COMMAND") _view_command = val;
    else if (kw.substr(0, 6) == "FILTER" &&
             kw.substr(kw.size() - 5, 5) == "_NAME") {
      string index_str = kw.substr(6, kw.size() - 11);
      bool ok = true;
      for (int idx = 0; idx < index_str.size(); ++idx)
        if (!isdigit(index_str[idx])) ok = false;
      int filtidx = atoi(index_str.c_str());
      if (!ok || filtidx == 0)
        throw DocMgr::Exception(DocMgr::Exception::MISC,
                                "Bad filter specification in config. file");
      for (int idx = _filter_names.size(); idx < filtidx; ++idx)
        _filter_names.push_back("");
      _filter_names[filtidx - 1] = val;
    } else if (kw.substr(0, 6) == "FILTER" &&
             kw.substr(kw.size() - 4, 4) == "_DEF") {
      string index_str = kw.substr(6, kw.size() - 10);
      bool ok = true;
      for (int idx = 0; idx < index_str.size(); ++idx)
        if (!isdigit(index_str[idx])) ok = false;
      int filtidx = atoi(index_str.c_str());
      if (!ok || filtidx == 0)
        throw DocMgr::Exception(DocMgr::Exception::MISC,
                                "Bad filter specification in config. file");
      for (int idx = _filter_defs.size(); idx < filtidx; ++idx)
        _filter_defs.push_back("");
      _filter_defs[filtidx - 1] = val;
    } else if (kw.substr(0, 6) == "FILTER" &&
             kw.substr(kw.size() - 8, 8) == "_HOLDING") {
      string index_str = kw.substr(6, kw.size() - 14);
      bool ok = true;
      for (int idx = 0; idx < index_str.size(); ++idx)
        if (!isdigit(index_str[idx])) ok = false;
      int filtidx = atoi(index_str.c_str());
      if (!ok || filtidx == 0)
        throw DocMgr::Exception(DocMgr::Exception::MISC,
                                "Bad filter specification in config. file");
      for (int idx = _filter_holding.size(); idx < filtidx; ++idx)
        _filter_holding.push_back("");
      _filter_holding[filtidx - 1] = val;
    } else if (kw.substr(0, 6) == "FILTER" &&
             kw.substr(kw.size() - 7, 7) == "_STATUS") {
      string index_str = kw.substr(6, kw.size() - 13);
      bool ok = true;
      for (int idx = 0; idx < index_str.size(); ++idx)
        if (!isdigit(index_str[idx])) ok = false;
      int filtidx = atoi(index_str.c_str());
      if (!ok || filtidx == 0)
        throw DocMgr::Exception(DocMgr::Exception::MISC,
                                "Bad filter specification in config. file");
      for (int idx = _filter_status.size(); idx < filtidx; ++idx)
        _filter_status.push_back("");
      _filter_status[filtidx - 1] = val;
    } else
      throw DocMgr::Exception(DocMgr::Exception::MISC,
                              "Bad keyword in config. file");
  }

  while (_filter_holding.size() < _filter_names.size())
    _filter_holding.push_back("");
  while (_filter_status.size() < _filter_names.size())
    _filter_status.push_back("");
}

void Configuration::save(void)
{
  ofstream ostr(_path.c_str());
  if (!ostr)
    throw DocMgr::Exception(DocMgr::Exception::MISC,
                            "Couldn't open config. file for writing");

  ostr << "SHOW_DELETED: " << (_show_deleted ? "true" : "false") << endl;
  ostr << "PAPER_DIRECTORY: " << _paper_directory << endl;
  if (_view_command != "") ostr << "VIEW_COMMAND: " << _view_command << endl;

  for (int idx = 0; idx < _filter_names.size(); ++idx) {
    ostr << "FILTER" << (idx + 1) << "_NAME: " << _filter_names[idx] << endl;
    ostr << "FILTER" << (idx + 1) << "_DEF: " << _filter_defs[idx] << endl;
    ostr << "FILTER" << (idx + 1) << "_HOLDING: "
         << _filter_holding[idx] << endl;
    ostr << "FILTER" << (idx + 1) << "_STATUS: "
         << _filter_status[idx] << endl;
  }
}


void Configuration::add_filter(string name, string def,
                               string holding, string status)
{
  _filter_names.push_back(name);
  _filter_defs.push_back(def);
  _filter_holding.push_back(holding);
  _filter_status.push_back(status);
}

void Configuration::delete_filter(int idx)
{
  if (idx >= 0 && idx < _filter_names.size()) {
    _filter_names.erase(_filter_names.begin() + idx);
    _filter_defs.erase(_filter_defs.begin() + idx);
    _filter_holding.erase(_filter_holding.begin() + idx);
    _filter_status.erase(_filter_status.begin() + idx);
  } else
    throw DocMgr::Exception(DocMgr::Exception::MISC,
                            "Invalid index for Configuration::delete_filter!");
}

void Configuration::set_filter_name(int idx, string name)
{
  if (idx >= 0 && idx < _filter_names.size())
    _filter_names[idx] = name;
  else
    throw DocMgr::Exception(DocMgr::Exception::MISC,
                            "Invalid index for "
                            "Configuration::set_filter_name!");
}

void Configuration::set_filter_definition(int idx, string def)
{
  if (idx >= 0 && idx < _filter_names.size())
    _filter_defs[idx] = def;
  else
    throw DocMgr::Exception(DocMgr::Exception::MISC,
                            "Invalid index for "
                            "Configuration::set_filter_definition!");
}

void Configuration::set_filter_holding(int idx, string holding)
{
  if (idx >= 0 && idx < _filter_holding.size())
    _filter_holding[idx] = holding;
  else
    throw DocMgr::Exception(DocMgr::Exception::MISC,
                            "Invalid index for "
                            "Configuration::set_filter_holding!");
}

void Configuration::set_filter_status(int idx, string status)
{
  if (idx >= 0 && idx < _filter_status.size())
    _filter_status[idx] = status;
  else
    throw DocMgr::Exception(DocMgr::Exception::MISC,
                            "Invalid index for "
                            "Configuration::set_filter_status!");
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
