//----------------------------------------------------------------------
//
//  FILE:   Filter.cpp
//  AUTHOR: Ian Ross
//  DATE:   05-OCT-2005
//
//----------------------------------------------------------------------
//
//  Document filter class.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <cctype>
#include <cstdio>
using namespace std;

// Local headers.

#include "Filter.hh"


//----------------------------------------------------------------------
//
//  STATIC MEMBER INITIALISERS
//
//----------------------------------------------------------------------

vector<Filter *> Filter::_filters;

int Filter::_filter_creation_count = 0;


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

Filter::~Filter()
{
  for (vector<Filter *>::iterator it = _filters.begin();
       it != _filters.end(); ++it)
    if (*it == this) {
      _filters.erase(it);
      break;
    }
}


DocMgr::Query Filter::query(void) const
{
  DocMgr::Query *q = parse_definition(_definition);
  DocMgr::Query retval(*q);
  delete q;
  if (_status_set) retval = retval && DocMgr::Query(_db, _status);
  if (_holding_set) retval = retval && DocMgr::Query(_db, _holding);
  return retval;
}


void Filter::set_definition(string definition)
{
  DocMgr::Query *q = parse_definition(definition);
  if (q) {
    delete q;
    _definition = definition;
  } else
    throw Exception("Invalid filter definition!");
  canonicalise_definition();
}


void Filter::set_status(DocMgr::Status status)
{
  _status_set = true;
  _status = status;
}


void Filter::set_holding(DocMgr::Holding holding)
{
  _holding_set = true;
  _holding = holding;
}


string Filter::next_filter_name(void)
{
  string new_name;
  bool ok = false;
  while (!ok) {
    ++_filter_creation_count;
    char buff[80];
    sprintf(buff, "Filter #%d", _filter_creation_count);
    new_name = buff;
    ok = true;
    for (int idx = 0; idx < _filters.size(); ++idx) {
      if (_filters[idx]->name() == new_name) {
        ok = false;
        break;
      }
    }
  }
  return new_name;
}

// Syntax of filter definitions:
//
//  filter ::= <empty> | or_list
//  or_list ::= <and_list> | <and_list> "|" <and_list>
//  and_list ::= <clause> | <clause> "&" <clause>
//  clause ::= "(" <or_list> ")" | <key> "=" <value>
//
// e.g. AU=Cox & (YR=2005 | YR=2004 | YR=2003)

DocMgr::Query *Filter::parse_definition(string def) const
{
  DocMgr::Query *retval = 0;
  while (isspace(def[0])) def.erase(0, 1);
  while (isspace(def[def.size() - 1])) def.erase(def.size() - 1);

  if (def == "")
    retval = new DocMgr::Query(_db);
  else
    retval = parse_or_list(def);
  return retval;
}


DocMgr::Query *Filter::parse_or_list(string or_list) const
{
  DocMgr::Query *retval = 0;
  vector<string> and_lists;
  int last_split_idx = 0;
  for (int idx = 0; idx < or_list.size(); ++idx) {
    if (or_list[idx] == '(') {
      ++idx;
      while (idx < or_list.size() && or_list[idx] != ')') ++idx;
      if (or_list[idx] != ')') return 0;
    } else if (or_list[idx] == '|') {
      and_lists.push_back(or_list.substr(last_split_idx,
                                         idx - last_split_idx));
      last_split_idx = idx + 1;
    }
  }
  and_lists.push_back(or_list.substr(last_split_idx));

  for (int idx = 0; idx < and_lists.size(); ++idx) {
    string and_list = and_lists[idx];
    while (isspace(and_list[0])) and_list.erase(0, 1);
    while (isspace(and_list[and_list.size() - 1]))
      and_list.erase(and_list.size() - 1);
    DocMgr::Query *and_query = parse_and_list(and_list);
    if (and_query == 0) { delete retval;  return 0; }
    if (idx == 0)
      retval = and_query;
    else {
      DocMgr::Query *tmp = new DocMgr::Query(*retval || *and_query);
      delete and_query;
      delete retval;
      retval = tmp;
    }
  }
  return retval;
}


DocMgr::Query *Filter::parse_and_list(string and_list) const
{
  DocMgr::Query *retval = 0;
  vector<string> clauses;
  int last_split_idx = 0;
  for (int idx = 0; idx < and_list.size(); ++idx) {
    if (and_list[idx] == '(') {
      ++idx;
      while (idx < and_list.size() && and_list[idx] != ')') ++idx;
      if (and_list[idx] != ')') return 0;
    } else if (and_list[idx] == '&') {
      clauses.push_back(and_list.substr(last_split_idx,
                                        idx - last_split_idx));
      last_split_idx = idx + 1;
    }
  }
  clauses.push_back(and_list.substr(last_split_idx));

  for (int idx = 0; idx < clauses.size(); ++idx) {
    string clause = clauses[idx];
    while (isspace(clause[0])) clause.erase(0, 1);
    while (isspace(clause[clause.size() - 1])) clause.erase(clause.size() - 1);
    DocMgr::Query *clause_query = parse_clause(clause);
    if (clause_query == 0) { delete retval;  return 0; }
    if (idx == 0)
      retval = clause_query;
    else {
      DocMgr::Query *tmp = new DocMgr::Query(*retval && *clause_query);
      delete clause_query;
      delete retval;
      retval = tmp;
    }
  }
  return retval;
}


DocMgr::Query *Filter::parse_clause(string clause) const
{
  DocMgr::Query *retval = 0;
  if (clause[0] == '(') {
    if (clause[clause.size() - 1] != ')')
      retval = 0;
    else
      retval = parse_or_list(clause.substr(1, clause.size() - 2));
  } else {
    string::size_type equals = clause.find('=');
    if (equals == string::npos)
      retval = 0;
    else {
      string key = clause.substr(0, equals);
      while (isspace(key[0])) key.erase(0, 1);
      while (isspace(key[key.size() - 1])) key.erase(key.size() - 1);
      for (int idx = 0; idx < key.size(); ++idx) key[idx] = toupper(key[idx]);
      string value = clause.substr(equals + 1);
      while (isspace(value[0])) value.erase(0, 1);
      while (isspace(value[value.size() - 1])) value.erase(value.size() - 1);
      if (!DocMgr::FieldType::valid(_db, key))
        retval = 0;
      else
        retval = new DocMgr::Query(_db, DocMgr::FieldType(_db, key), value);
    }
  }
  return retval;
}



void Filter::canonicalise_definition(void)
{
  while (isspace(_definition[0])) _definition.erase(0, 1);
  while (isspace(_definition[_definition.size() - 1]))
    _definition.erase(_definition.size() - 1);
  _definition = canonicalise_or_list(_definition);
  if (_definition[0] == '(')
    _definition = _definition.substr(1, _definition.size() - 2);
}


string Filter::canonicalise_or_list(string or_list)
{
  string retval = "";
  vector<string> and_lists;
  int last_split_idx = 0;
  for (int idx = 0; idx < or_list.size(); ++idx) {
    if (or_list[idx] == '(') {
      ++idx;
      while (idx < or_list.size() && or_list[idx] != ')') ++idx;
      if (or_list[idx] != ')') return 0;
    } else if (or_list[idx] == '|') {
      and_lists.push_back(or_list.substr(last_split_idx,
                                         idx - last_split_idx));
      last_split_idx = idx + 1;
    }
  }
  and_lists.push_back(or_list.substr(last_split_idx));

  for (int idx = 0; idx < and_lists.size(); ++idx) {
    string and_list = and_lists[idx];
    while (isspace(and_list[0])) and_list.erase(0, 1);
    while (isspace(and_list[and_list.size() - 1]))
      and_list.erase(and_list.size() - 1);
    string and_str = canonicalise_and_list(and_list);
    if (and_str == "") { return ""; }
    if (idx == 0)
      retval = and_str;
    else
      retval += string(" | ") + and_str;
  }
  if (and_lists.size() > 1) retval = string("(") + retval + ")";
  return retval;
}


string Filter::canonicalise_and_list(string and_list)
{
  string retval = "";
  vector<string> clauses;
  int last_split_idx = 0;
  for (int idx = 0; idx < and_list.size(); ++idx) {
    if (and_list[idx] == '(') {
      ++idx;
      while (idx < and_list.size() && and_list[idx] != ')') ++idx;
      if (and_list[idx] != ')') return 0;
    } else if (and_list[idx] == '&') {
      clauses.push_back(and_list.substr(last_split_idx,
                                        idx - last_split_idx));
      last_split_idx = idx + 1;
    }
  }
  clauses.push_back(and_list.substr(last_split_idx));

  for (int idx = 0; idx < clauses.size(); ++idx) {
    string clause = clauses[idx];
    while (isspace(clause[0])) clause.erase(0, 1);
    while (isspace(clause[clause.size() - 1])) clause.erase(clause.size() - 1);
    string clause_str = canonicalise_clause(clause);
    if (clause_str == "") return "";
    if (idx == 0)
      retval = clause_str;
    else
      retval += string(" & ") + clause_str;
  }
  if (clauses.size() > 1) retval = string("(") + retval + ")";
  return retval;
}


string Filter::canonicalise_clause(string clause)
{
  string retval = "";
  if (clause[0] == '(') {
    if (clause[clause.size() - 1] != ')')
      retval = "";
    else
      retval = canonicalise_or_list(clause.substr(1, clause.size() - 2));
  } else {
    string::size_type equals = clause.find('=');
    if (equals == string::npos)
      retval = "";
    else {
      string key = clause.substr(0, equals);
      while (isspace(key[0])) key.erase(0, 1);
      while (isspace(key[key.size() - 1])) key.erase(key.size() - 1);
      for (int idx = 0; idx < key.size(); ++idx) key[idx] = toupper(key[idx]);
      string value = clause.substr(equals + 1);
      while (isspace(value[0])) value.erase(0, 1);
      while (isspace(value[value.size() - 1])) value.erase(value.size() - 1);
      retval = key + "=" + value;
    }
  }
  return retval;
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
