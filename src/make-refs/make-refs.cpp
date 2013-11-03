//----------------------------------------------------------------------
//
//  FILE:   make-refs.cpp
//  AUTHOR: Ian Ross
//  DATE:   04-OCT-2005
//
//----------------------------------------------------------------------
//
//  Bibliography generator.
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
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <set>

using namespace std;


// Local headers.

#include "DocMgr.hh"


//----------------------------------------------------------------------
//
//  LOCAL FUNCTION PROTOTYPES
//
//----------------------------------------------------------------------

string convert_doc_type(string docmgr);
string convert_field_type(string docmgr);
void display_record(ostream &ostr, int number, DocMgr::DocRecord *rec);
string escape_latex_commands(string s);


//----------------------------------------------------------------------
//
//  CLASS DEFINITIONS
//
//----------------------------------------------------------------------

class RefDefinition {
public:

  RefDefinition() { }
  RefDefinition(string ref_def, int ref_number);
  ~RefDefinition() { }

  bool valid(void) const { return _clauses.size() > 0; }

  int nclauses(void) const { return _clauses.size(); }
  string field(int idx) const { return _clauses[idx].field; }
  string value(int idx) const { return _clauses[idx].value; }

  int number(void) const { return _number; }

  DocMgr::Query query(DocMgr::Connection &db) const;

private:

  static bool valid_field(string field);

  static const char *valid_fields[];

  struct RefClause {
    RefClause(string inf, string inv) : field(inf), value(inv) { }
    string field;
    string value;
  };

  vector<RefClause> _clauses;
  int _number;
};

struct DocMgrToBibTeX {
  const char *docmgr;
  const char *bibtex;
};


//----------------------------------------------------------------------
//
//  GLOBAL VARIABLE DEFINITIONS
//
//----------------------------------------------------------------------

DocMgrToBibTeX doc_types[] = {
  { "AT", "Article" },
  { "BK", "Book" },
  { "BL", "Booklet" },
  { "IB", "InBook" },
  { "IC", "InCollection" },
  { "IP", "InProceedings" },
  { "MA", "Manual" },
  { "MT", "MastersThesis" },
  { "MS", "Misc" },
  { "PH", "PhdThesis" },
  { "PR", "Proceedings" },
  { "TR", "TechReport" },
  { "UP", "Unpublished" }
};

DocMgrToBibTeX field_types[] = {
  { "AD", "address" },
  { "BT", "booktitle" },
  { "CA", "chapter" },
  { "EN", "edition" },
  { "ED", "editor" },
  { "HO", "howpublished" },
  { "IN", "institution" },
  { "JN", "journal" },
  { "MO", "month" },
  { "NT", "note" },
  { "NO", "number" },
  { "OG", "organization" },
  { "PU", "publisher" },
  { "SH", "school" },
  { "TI", "title" },
  { "TY", "type" },
  { "UR", "url" },
  { "YR", "year" },
  { "SE", "series" },
  { "VO", "volume" },
  { "AU", "author" },
  { "LA", "language" },
  { "IS", "isbn" },
  { "KW", "keywords" },
  { "EP", "eprint" },
  { "PG", "pages" },
  { "XR", "crossref" }
};


//----------------------------------------------------------------------
//
//  MAIN PROGRAM
//
//----------------------------------------------------------------------

int main(int argc, char *argv[])
{
  try {
    if (argc != 2) {
      cout << "Usage: make-refs <in-file>" << endl;
      exit(1);
    }
    string infile = argv[1];

    ifstream *istr = new ifstream(infile.c_str());
    if (!*istr) {
      cout << "Can't open input file '" << infile << "'" << endl;
      exit(1);
    }

    // Process input file - first pass picks out all the @ tags.
    // Second pass will transform @cite\(.*\)(\(.*\))->\cite\1{\2},
    // @bibliography->\bibliography (or table) and chop out reference
    // definitions.
    string bibliography = "";
    bool latex_file = true;
    map<string, vector<int> > cites;
    map<string, RefDefinition> refs;
    map<string, int> ref_lines;
    int line = 0;
    int ref_count = 1;
    bool error = false;
    char buff[1024];
    bool need_read = true;
    while (!istr->eof()) {
      if (need_read) {
        istr->getline(buff, 1024);
        ++line;
      }
      need_read = true;
      char *pt;
      if ((pt = strstr(buff, "@cite"))) {
        int cite_line = line;
        pt += strlen("@cite");
        while (*pt && *pt != '(') ++pt;
        if (*pt++ != '(') {
          cout << infile << ':' << line
               << ": syntax error (missing open parenthesis)" << endl;
          error = true;
        } else {
          char *end_pt = pt;
          while (*end_pt && *end_pt != ')') ++end_pt;
          string cite;
          if (*end_pt == ')')
            cite = string(pt, end_pt - pt);
          else {
            cite = pt;
            bool done = false;
            while (!done) {
              if (istr->eof()) {
                cout << "Missing closing parenthesis at end of file!" << endl;
                exit(1);
              }
              istr->getline(buff, 1024);
              ++line;
              end_pt = buff;
              while (*end_pt && *end_pt != ')') ++end_pt;
              if (*end_pt == ')') {
                cite += string(buff, end_pt - buff);
                memmove(buff, end_pt + 1, strlen(buff) - (end_pt - buff));
                done = true;
                need_read = false;
              } else
                cite += buff;
            }
          }
          while (cite.find(',') != string::npos) {
            string single = cite.substr(0, cite.find(','));
            while (isspace(single[0])) single.erase(0, 1);
            while (isspace(single[single.size() - 1]))
              single.erase(single.size() - 1);
            cites[single].push_back(cite_line);
            cite = cite.substr(cite.find(',') + 1);
          }
          while (isspace(cite[0])) cite.erase(0, 1);
          while (isspace(cite[cite.size() - 1])) cite.erase(cite.size() - 1);
          cites[cite].push_back(cite_line);
        }
      } else if ((pt = strstr(buff, "@bibliography"))) {
        if (bibliography != "") {
          cout << infile << ':' << line
               << ": duplicate bibliography tag" << endl;
          error = true;
        } else {
          pt += strlen("@bibliography");
          if (!*pt) {
            latex_file = false;
            bibliography = "PLAIN-TEXT";
          } else if (*pt++ != '(') {
            cout << infile << ':' << line
                 << ": syntax error (missing open parenthesis)" << endl;
            error = true;
          } else {
            char *end_pt = pt;
            while (*end_pt && *end_pt != ')') ++end_pt;
            if (*end_pt == ')')
              bibliography = string(pt, end_pt - pt);
            else {
              cout << infile << ':' << line << ": syntax error" << endl;
              error = true;
            }
          }
        }
      } else if ((pt = strstr(buff, "@ref"))) {
        pt += strlen("@ref");
        if (*pt++ != '(') {
          cout << infile << ':' << line
               << ": syntax error (missing parenthesis)" << endl;
          error = true;
        } else {
          char *end_pt = pt;
          while (*end_pt && *end_pt != ')') ++end_pt;
          string ref;
          if (*end_pt == ')')
            ref = string(pt, end_pt - pt);
          else {
            ref = pt;
            bool done = false;
            while (!done) {
              if (istr->eof()) {
                cout << "Missing closing parenthesis at end of file!" << endl;
                exit(1);
              }
              istr->getline(buff, 1024);
              ++line;
              end_pt = buff;
              while (*end_pt && *end_pt != ')') ++end_pt;
              if (*end_pt == ')') {
                ref += ' ';
                ref += string(buff, end_pt - buff);
                memmove(buff, end_pt + 1, strlen(buff) - (end_pt - buff));
                done = true;
                need_read = false;
              } else
                ref += buff;
            }
          }
          if (ref.find(' ') == string::npos) {
            cout << infile << ':' << line
                 << ": syntax error in reference definition" << endl;
            error = true;
          } else {
            string ref_name = ref.substr(0, ref.find(' '));
            while (isspace(ref_name[0])) ref_name.erase(0, 1);
            while (isspace(ref_name[ref_name.size() - 1]))
              ref_name.erase(ref_name.size() - 1);
            string ref_rest = ref.substr(ref.find(' ') + 1);
            while (isspace(ref_rest[0])) ref_rest.erase(0, 1);
            while (isspace(ref_rest[ref_rest.size() - 1]))
              ref_rest.erase(ref_rest.size() - 1);
            if (refs.find(ref_name) != refs.end()) {
              cout << infile << ':' << line
                   << ": duplicate definition for reference '" << ref_name
                   << "'" << endl;
              error = true;
            } else {
              RefDefinition def(ref_rest, ref_count++);
              if (def.valid()) {
                refs[ref_name] = def;
                ref_lines[ref_name] = line;
              } else {
                cout << infile << ':' << line
                     << ": syntax error in reference definition" << endl;
                error = true;
              }
            }
          }
        }
      }
    }

    // Check reference definitions for citations.
    for (map<string, vector<int> >::iterator it = cites.begin();
         it != cites.end(); ++it) {
      string cite = it->first;
      vector<int> lines = it->second;
      if (refs.find(cite) == refs.end()) {
        error = true;
        cout << "Undefined reference '" << cite << "' used at line";
        if (lines.size() > 1) cout << 's';
        for (int idx = 0; idx < lines.size(); ++idx) {
          if (idx == 0)
            cout << ' ';
          else if (idx == lines.size() - 1)
            cout << " and ";
          else
            cout << ", ";
          cout << lines[idx];
        }
        cout << endl;
      }
    }
    if (error) {
      cout << "Errors encountered.  Exiting" << endl;
      exit(1);
    }

    // Connect to database.
    string db = getenv("DOCMGR_DB") ? getenv("DOCMGR_DB") : "docmgr";
    string host = getenv("DOCMGR_HOST") ? getenv("DOCMGR_HOST") : "";
    string user = getenv("DOCMGR_USER") ? getenv("DOCMGR_USER") : "";
    string password = getenv("DOCMGR_PASS") ? getenv("DOCMGR_PASS") : "";
    string failure_msg;
    DocMgr::Connection *conn = 0;
    try {
      conn = new DocMgr::Connection(db, host, user, password);
    } catch (DocMgr::Exception &exc) {
      if (exc.type() != DocMgr::Exception::DB_ERROR) throw;
      failure_msg = exc.msg();
    }
    if (!conn) {
      cout << "Database error: " << failure_msg << endl;
      exit(1);
    }

    // Get database IDs for citations.
    map<string, DocMgr::DocRecord *> ref_recs;
    for (map<string, RefDefinition>::const_iterator it = refs.begin();
         it != refs.end(); ++it) {
      DocMgr::Query q(it->second.query(*conn));
      vector<DocMgr::DocID> ids;
      q.run(ids);
      if (ids.size() == 0) {
        cout << infile << ':' << ref_lines[it->first]
             << ": no records for reference '" << it->first << "'" << endl;
        error = true;
      } else if (ids.size() > 1) {
        cout << infile << ':' << ref_lines[it->first]
             << ": non-unique result for reference '"
             << it->first << "'" << endl;
        error = true;
      } else {
        string id = ids[0];
        DocMgr::DocID id_num = ids[0];
        DocMgr::DocRecord *rec = conn->get_doc_by_id(id_num);
        DocMgr::Holding holding = rec->holding();
        delete rec;
        cout << id << ':' << it->first << ':' << string(holding) << endl;
        // int range_base = (id_num / 100) * 100;
        // char fname_buff[80];
        // sprintf(fname_buff, "/papers/%06d-%06d/%s.pdf",
        //         range_base, range_base + 99, id.c_str());
        // ostringstream cmd;
        // cmd << "cp " << fname_buff << " " << it->first << ".pdf";
        // system(cmd.str().c_str());
      }
    }
    if (error) {
      cout << "Errors encountered.  Exiting" << endl;
      exit(1);
    }
  } catch (DocMgr::Exception &exc) {
    cout << "UNCAUGHT DocMgr EXCEPTION: " << exc.msg() << endl;
  }
}


//----------------------------------------------------------------------
//
//  LOCAL FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

string convert_doc_type(string docmgr)
{
  for (int idx = 0;
       idx < sizeof(doc_types) / sizeof(DocMgrToBibTeX); ++idx)
    if (docmgr == doc_types[idx].docmgr)
      return doc_types[idx].bibtex;
  return "";
}

string convert_field_type(string docmgr)
{
  for (int idx = 0;
       idx < sizeof(field_types) / sizeof(DocMgrToBibTeX); ++idx)
    if (docmgr == field_types[idx].docmgr)
      return field_types[idx].bibtex;
  return "";
}

string escape_latex_commands(string s)
{
  string retval = s;
  char specials[] = { '"', '\'', '`', '^', '~' };
  for (int spec = 0; spec < sizeof(specials) / sizeof(char); ++spec) {
    string bad = string("\\") + specials[spec] + '{';
    string ok = string("{\\") + specials[spec];
    string::size_type pos = 0;
    while ((pos = retval.find(bad, pos)) != string::npos)
      if (retval[pos + 3] != '}')
        retval.replace(pos, 3, ok);
      else
        ++pos;
  }
  return retval;
}


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

const char *RefDefinition::valid_fields[] =
  { "BT", "CA", "EN", "ED", "IN", "JN", "MO", "NO", "OG",
    "PU", "SH", "TI", "YR", "SE", "VO", "AU", "KW", "PG" };

bool RefDefinition::valid_field(string field)
{
  for (int idx = 0; idx < sizeof(valid_fields) / sizeof(const char *); ++idx)
    if (field == valid_fields[idx])
      return true;
  return false;
}


RefDefinition::RefDefinition(string ref_def, int ref_number)
{
  _number = ref_number;
  vector<string> clause_strs;
  while (ref_def.find(',') != string::npos) {
    string single = ref_def.substr(0, ref_def.find(','));
    while (isspace(single[0])) single.erase(0, 1);
    while (isspace(single[single.size() - 1]))
      single.erase(single.size() - 1);
    clause_strs.push_back(single);
    ref_def = ref_def.substr(ref_def.find(',') + 1);
  }
  while (isspace(ref_def[0])) ref_def.erase(0, 1);
  while (isspace(ref_def[ref_def.size() - 1]))
    ref_def.erase(ref_def.size() - 1);
  clause_strs.push_back(ref_def);

  bool ok = true;
  for (int idx = 0; idx < clause_strs.size(); ++idx) {
    if (clause_strs[idx].size() < 4) { ok = false; break; }
    string field_name = clause_strs[idx].substr(0, 2);
    field_name[0] = toupper(field_name[0]);
    field_name[1] = toupper(field_name[1]);
    if (!valid_field(field_name)) { ok = false; break; }
    string rest = clause_strs[idx].substr(2);
    while (isspace(rest[0])) rest.erase(0, 1);
    if (rest.size() < 2 || rest[0] != '=') { ok = false; break; }
    rest = rest.substr(1);
    while (isspace(rest[0])) rest.erase(0, 1);
    _clauses.push_back(RefClause(field_name, rest));
  }
  if (!ok) _clauses.clear();
}


DocMgr::Query RefDefinition::query(DocMgr::Connection &db) const
{
  DocMgr::Query retval(db);
  for (int idx = 0; idx < _clauses.size(); ++idx) {
    string field = _clauses[idx].field;
    string value = _clauses[idx].value;

    DocMgr::Query clause_query(db);
    while (value.find(' ') != string::npos) {
      string single = value.substr(0, value.find(' '));
      while (isspace(single[0])) single.erase(0, 1);
      while (isspace(single[single.size() - 1]))
        single.erase(single.size() - 1);
      clause_query = clause_query &&
        DocMgr::Query(db, DocMgr::FieldType(db, field), single);
      value = value.substr(value.find(' ') + 1);
    }
    while (isspace(value[0])) value.erase(0, 1);
    while (isspace(value[value.size() - 1])) value.erase(value.size() - 1);
    clause_query = clause_query &&
      DocMgr::Query(db, DocMgr::FieldType(db, field), value);

    retval = retval && clause_query;
  }
  return retval;
}


void display_record(ostream &ostr, int number, DocMgr::DocRecord *rec)
{
  ostr << '[' << number << "] ";
  rec->display(ostr);
}




//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
