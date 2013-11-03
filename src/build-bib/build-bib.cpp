//----------------------------------------------------------------------
//
//  FILE:   build-bib.cpp
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
    if (argc != 3 && argc != 4) {
      cout << "Usage: build-bib <dtx-file> <tex-file> [<bib-file>]" << endl;
      exit(1);
    }
    string infile = argv[1];
    string texfile = argv[2];
    string bibfile = "";
    if (argc == 4) bibfile = argv[3];

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
    bool write_bib_command = true;
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
      if ((pt = strstr(buff, "@suppress-bib-command"))) {
        write_bib_command = false;
        pt += strlen("@suppress-bib-file");
      } else if ((pt = strstr(buff, "@cite"))) {
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
    if (write_bib_command && bibliography == "") {
      cout << "No bibliography definition" << endl;
      error = true;
    }
    if (error) {
      cout << "Errors encountered.  Exiting" << endl;
      exit(1);
    }

    // Connect to database.
    string db;
    if (getenv("DOCMGR_DB"))
      db = getenv("DOCMGR_DB");
    else {
      string home = getenv("HOME");
      db = home + "/.docmgr2/docmgr.db";
    }
    string failure_msg;
    DocMgr::Connection *conn = 0;
    try {
      conn = new DocMgr::Connection(db);
    } catch (DocMgr::Exception &exc) {
      if (exc.type() != DocMgr::Exception::DB_ERROR) throw;
      failure_msg = exc.msg();
    }
    if (!conn) {
      cout << "Database error: " << failure_msg << endl;
      exit(1);
    }

    // Get database records.
    map<string, DocMgr::DocRecord *> ref_recs;
    for (map<string, RefDefinition>::const_iterator it = refs.begin();
         it != refs.end(); ++it) {
      const RefDefinition &rrr = it->second;
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
      } else
        ref_recs[it->first] = conn->get_doc_by_id(ids[0]);
    }
    if (error) {
      cout << "Errors encountered.  Exiting" << endl;
      exit(1);
    }

    // Get cross-reference database records.
    map<DocMgr::DocID, DocMgr::DocRecord *> xref_recs;
    for (map<string, DocMgr::DocRecord *>::iterator it = ref_recs.begin();
         it != ref_recs.end(); ++it) {
      DocMgr::DocRecord *rec = it->second;
      map<DocMgr::FieldType, string> &fields = rec->fields();
      map<DocMgr::FieldType, string>::iterator xrf =
        fields.find(DocMgr::FieldType(*conn, "XR"));
      if (xrf != fields.end()) {
        DocMgr::DocID xref_id = xrf->second;
        xref_recs[xref_id] = conn->get_doc_by_id(xref_id);
      }
    }

    // Rewind for second pass.
    delete istr;
    istr = new ifstream(infile.c_str());

    ofstream ostr(texfile.c_str());
    if (!ostr) {
      cout << "Can't open output file '" << texfile << "'" << endl;
      exit(1);
    }

    if (latex_file) {
      // Change @ tags to LaTeX tags or elide as required.
      need_read = true;
      while (!istr->eof()) {
        if (need_read) istr->getline(buff, 1024);
        need_read = true;
        int line_len = strlen(buff);
        char *pt;
        if ((pt = strstr(buff, "@suppress-bib-command"))) {
          pt += strlen("@suppress-bib-file");
        } else if ((pt = strstr(buff, "@cite"))) {
          *pt = '\0';
          ostr << buff;
          pt += strlen("@cite");
          ostr << "\\cite";
          while (*pt && *pt != '(') { ostr << *pt; ++pt; }
          ++pt;
          ostr << '{';
          char *end_pt = pt;
          while (*end_pt && *end_pt != ')') ++end_pt;
          string cite;
          if (*end_pt == ')') {
            ostr << string(pt, end_pt - pt) << '}';
            memmove(buff, end_pt + 1, line_len - (end_pt - buff));
            need_read = false;
          } else {
            ostr << pt;
            bool done = false;
            while (!done) {
              istr->getline(buff, 1024);
              line_len = strlen(buff);
              end_pt = buff;
              while (*end_pt && *end_pt != ')') ++end_pt;
              if (*end_pt == ')') {
                ostr << string(buff, end_pt - buff) << '}';
                memmove(buff, end_pt + 1, line_len - (end_pt - buff));
                done = true;
                need_read = false;
              } else
                ostr << buff << endl;
            }
          }
        } else if ((pt = strstr(buff, "@bibliography"))) {
          *pt = '\0';
          ostr << buff;
          pt += strlen("@bibliography(");
          if (write_bib_command) ostr << "\\bibliography{";
          char *end_pt = pt;
          while (*end_pt != ')') ++end_pt;
          if (write_bib_command) ostr << string(pt, end_pt - pt) << '}';
          memmove(buff, end_pt + 1, line_len - (end_pt - buff));
          need_read = false;
        } else if ((pt = strstr(buff, "@ref"))) {
          *pt = '\0';
          ostr << buff;
          pt += strlen("@ref(");
          char *end_pt = pt;
          while (*end_pt && *end_pt != ')') ++end_pt;
          if (*end_pt == ')') {
            if (end_pt[1] != '\0') {
              memmove(buff, end_pt + 1, line_len - (end_pt - buff));
              need_read = false;
            }
          } else {
            bool done = false;
            while (!done) {
              istr->getline(buff, 1024);
              line_len = strlen(buff);
              end_pt = buff;
              while (*end_pt && *end_pt != ')') ++end_pt;
              if (*end_pt == ')') {
                if (end_pt[1] != '\0') {
                  memmove(buff, end_pt + 1, line_len - (end_pt - buff));
                  need_read = false;
                }
                done = true;
              }
            }
          }
        } else
          // Just copy the line.
          ostr << buff << endl;
      }


      // Write the BibTeX file.
      if (bibfile == "") bibfile = bibliography + ".bib";
      ofstream bstr(bibfile.c_str());
      if (!bstr) {
        cout << "Can't open bibliography file '" << bibfile << "'" << endl;
        exit(1);
      }

      for (map<string, DocMgr::DocRecord *>::iterator it = ref_recs.begin();
           it != ref_recs.end(); ++it) {
        string name = it->first;
        DocMgr::DocRecord *rec = it->second;
        
        string entry_type = convert_doc_type(rec->type());
        if (entry_type == "") {
          cout << "INTERNAL ERROR: unknown entry type '"
               << string(rec->type()) << "'" << endl;
          exit(1);
        }

        bstr << '@' << entry_type << '{' << name << ',' << endl;
        map<DocMgr::FieldType, string> &fields = rec->fields();
        bool first = true;
        for (map<DocMgr::FieldType, string>::iterator it = fields.begin();
             it != fields.end(); ++it) {
          if (it->second == "") continue;
          string field_name = convert_field_type(it->first);
          if (field_name != "") {
            if (!first) bstr << "," << endl;
            first = false;
            if (field_name != "crossref")
              bstr << "  " << field_name << "=\""
                   << escape_latex_commands(it->second) << "\"";
            else
              bstr << "  " << field_name << "=\"xref-" << it->second << "\"";
          }
        }
        bstr << endl << '}' << endl;
      }

      // Write the cross-reference records.
      for (map<DocMgr::DocID, DocMgr::DocRecord *>::iterator it =
             xref_recs.begin(); it != xref_recs.end(); ++it) {
        DocMgr::DocID id = it->first;
        DocMgr::DocRecord *rec = it->second;

        string entry_type = convert_doc_type(rec->type());
        if (entry_type == "") {
          cout << "INTERNAL ERROR: unknown entry type '"
               << string(rec->type()) << "'" << endl;
          exit(1);
        }

        string name = string("xref-") + string(id);
        bstr << '@' << entry_type << '{' << name << ',' << endl;
        map<DocMgr::FieldType, string> &fields = rec->fields();
        bool first = true;
        for (map<DocMgr::FieldType, string>::iterator it = fields.begin();
             it != fields.end(); ++it) {
          if (it->second == "") continue;
          string field_name = convert_field_type(it->first);
          if (!first) bstr << "," << endl;
          first = false;
          if (field_name == "title") field_name = "booktitle";
          if (field_name != "crossref")
            bstr << "  " << field_name << "=\""
                 << escape_latex_commands(it->second) << "\"";
          else
            bstr << "  " << field_name << "=\"xref-" << it->second << "\"";
        }
        bstr << endl << '}' << endl;
      }
    } else {
      // Non-LaTeX.

      // Change @cite tags to [<number>], elide @bibliography tag and
      // replace @ref tags with bibliography definitions.
      need_read = true;
      while (!istr->eof()) {
        if (need_read) istr->getline(buff, 1024);
        need_read = true;
        int line_len = strlen(buff);
        char *pt;
        if ((pt = strstr(buff, "@cite"))) {
          *pt = '\0';
          ostr << buff;
          pt += strlen("@cite(");
          char *end_pt = pt;
          while (*end_pt && *end_pt != ')') ++end_pt;
          string cite;
          if (*end_pt == ')') {
            cite = string(pt, end_pt - pt);
            memmove(buff, end_pt + 1, line_len - (end_pt - buff));
            need_read = false;
          } else {
            cite = pt;
            bool done = false;
            while (!done) {
              istr->getline(buff, 1024);
              line_len = strlen(buff);
              end_pt = buff;
              while (*end_pt && *end_pt != ')') ++end_pt;
              if (*end_pt == ')') {
                cite += string(buff, end_pt - buff);
                memmove(buff, end_pt + 1, line_len - (end_pt - buff));
                done = true;
                need_read = false;
              } else
                cite += buff;
            }
          }
          ostr << "[";
          bool first = true;
          while (cite.find(',') != string::npos) {
            string single = cite.substr(0, cite.find(','));
            while (isspace(single[0])) single.erase(0, 1);
            while (isspace(single[single.size() - 1]))
              single.erase(single.size() - 1);
            if (!first) ostr << ',';
            ostr << refs[single].number();
            cite = cite.substr(cite.find(',') + 1);
            first = false;
          }
          while (isspace(cite[0])) cite.erase(0, 1);
          while (isspace(cite[cite.size() - 1])) cite.erase(cite.size() - 1);
          if (!first) ostr << ',';
          ostr << refs[cite].number() << ']';
        } else if ((pt = strstr(buff, "@bibliography"))) {
          *pt = '\0';
          ostr << buff;
          pt += strlen("@bibliography");
          need_read = false;
        } else if ((pt = strstr(buff, "@ref("))) {
          pt += strlen("@ref(");
          char *end_pt = pt;
          while (*end_pt && *end_pt != ')') ++end_pt;
          string ref;
          if (*end_pt == ')')
            ref = string(pt, end_pt - pt);
          else {
            ref = pt;
            bool done = false;
            while (!done) {
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
          string ref_name = ref.substr(0, ref.find(' '));
          while (isspace(ref_name[0])) ref_name.erase(0, 1);
          while (isspace(ref_name[ref_name.size() - 1]))
            ref_name.erase(ref_name.size() - 1);
          display_record(ostr, refs[ref_name].number(), ref_recs[ref_name]);
        } else
          // Just copy the line.
          ostr << buff << endl;
      }
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
    "PU", "SH", "TI", "YR", "SE", "VO", "AU", "KW", "PG", "TG"  };

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
