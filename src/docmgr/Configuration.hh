//----------------------------------------------------------------------
//
//  FILE:   Configuration.hh
//  AUTHOR: Ian Ross
//  DATE:   02-OCT-2005
//
//----------------------------------------------------------------------
//
//  Configuration file reader/writer.
//
//----------------------------------------------------------------------

#ifndef _H_CONFIGURATION_
#define _H_CONFIGURATION_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>
#include <vector>

using namespace std;


//----------------------------------------------------------------------
//
//  CLASS DEFINITION
//
//----------------------------------------------------------------------

class Configuration {
public:

  Configuration(string path);

  void reread(void);
  void save(void);

  bool show_deleted(void) const { return _show_deleted; }
  string paper_directory(void) const { return _paper_directory; }
  string default_import_file(void) const { return _default_import_file; }
  string view_command(void) const { return _view_command; }

  int filter_count(void) const { return _filter_names.size(); }
  string filter_name(int idx) const { return _filter_names[idx]; }
  string filter_definition(int idx) const { return _filter_defs[idx]; }
  string filter_holding(int idx) const { return _filter_holding[idx]; }
  string filter_status(int idx) const { return _filter_status[idx]; }

  void set_show_deleted(bool val) { _show_deleted = val; }
  void set_paper_directory(string val) { _paper_directory = val; }
  void set_default_import_file(string val) { _default_import_file = val; }
  void set_view_command(string val) { _view_command = val; }

  void add_filter(string name, string def,
                  string holding = "", string status = "");
  void delete_filter(int idx);
  void set_filter_name(int idx, string name);
  void set_filter_definition(int idx, string def);
  void set_filter_holding(int idx, string holding);
  void set_filter_status(int idx, string status);

private:

  string _path;

  bool _show_deleted;
  string _paper_directory;
  string _default_import_file;
  string _view_command;

  vector<string> _filter_names;
  vector<string> _filter_defs;
  vector<string> _filter_holding;
  vector<string> _filter_status;
};


#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
