//----------------------------------------------------------------------
//
//  FILE:   Filter.hh
//  AUTHOR: Ian Ross
//  DATE:   05-OCT-2005
//
//----------------------------------------------------------------------
//
//  Document filter class.
//
//----------------------------------------------------------------------

#ifndef _H_FILTER_
#define _H_FILTER_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>
#include <vector>

using namespace std;


// Local headers.

#include "DocMgr.hh"


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class Filter {
public:

  class Exception : public DocMgr::Exception {
  public:

    Exception(string msg) :
      DocMgr::Exception(DocMgr::Exception::MISC, msg) { }
    virtual ~Exception() { }
  };

  Filter(DocMgr::Connection &db) :
    _db(db), _name(next_filter_name()), _definition(""),
    _status_set(false), _holding_set(false)
  { _filters.push_back(this); }
  ~Filter();

  string name(void) const { return _name; }
  string definition(void) const { return _definition; }
  string holding(void) const { return _holding_set ? string(_holding) : ""; }
  string status(void) const { return _status_set ? string(_status) : ""; }
  DocMgr::Query query(void) const;

  void set_name(string name) { _name = name; }
  void set_definition(string definition);

  void set_status(DocMgr::Status status);
  void clear_status(void) { _status_set = false; }
  bool has_status(void) const { return _status_set; }
  void set_holding(DocMgr::Holding holding);
  void clear_holding(void) { _holding_set = false; }
  bool has_holding(void) const { return _holding_set; }

  void canonicalise_definition(void);

private:

  DocMgr::Query *parse_definition(string definition) const;
  DocMgr::Query *parse_or_list(string or_list) const;
  DocMgr::Query *parse_and_list(string and_list) const;
  DocMgr::Query *parse_clause(string clause) const;

  static string canonicalise_or_list(string or_list);
  static string canonicalise_and_list(string and_list);
  static string canonicalise_clause(string clause);

  static string next_filter_name(void);

  static vector<Filter *> _filters;
  static int _filter_creation_count;

  DocMgr::Connection &_db;
  string _name;
  string _definition;
  bool _status_set;
  DocMgr::Status _status;
  bool _holding_set;
  DocMgr::Holding _holding;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
