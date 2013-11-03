//----------------------------------------------------------------------
//
//  FILE:   DocMgr.cpp
//  AUTHOR: Ian Ross
//  DATE:   12-SEP-2004
//
//  Main source file for document manager library.
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
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <list>

using namespace std;


// Sqlite3 API header.

#include <sqlite3.h>


// Local header.

#include "DocMgr.hh"

using namespace DocMgr;


//ofstream qstr("query.sql");

//----------------------------------------------------------------------
//
//  LOCAL TYPE DEFINITIONS
//
//----------------------------------------------------------------------

struct DocMgr::ConnectionPriv {
  sqlite3 *dbconn;
};


//----------------------------------------------------------------------
//
//  LOCAL FUNCTION PROTOTYPES
//
//----------------------------------------------------------------------

static string escape_string(string in_str);


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS: DocID
//
//----------------------------------------------------------------------

DocID::DocID(int poss_id)
{
  if (poss_id <= 0 || poss_id > 999999) {
    char buff[32];
    sprintf(buff, "%d", poss_id);
    throw Exception(Exception::INVALID_DOCID,
                    string("Invalid document ID: '") + buff + "'");
  }
  _id = poss_id;
}

DocID::DocID(const char *poss_id)
{
  if (!valid(poss_id))
    throw Exception(Exception::INVALID_DOCID,
                    string("Invalid document ID: '") + poss_id + "'");
  _id = atoi(poss_id);
}

DocID::DocID(string poss_id)
{
  if (!valid(poss_id))
    throw Exception(Exception::INVALID_DOCID,
                    string("Invalid document ID: '") + poss_id + "'");
  _id = atoi(poss_id.c_str());
}

bool DocID::valid(string poss_id)
{
  if (poss_id.length() < 1 || poss_id.length() > 6) return false;
  for (int idx = 0; idx < poss_id.length(); ++idx)
    if (!isdigit(poss_id[idx])) return false;
  return true;
}

bool DocID::valid(int poss_id)
{
  return (poss_id > 0 && poss_id <= 999999);
}


bool DocID::valid_range(string poss_range)
{
  string::size_type dash = poss_range.find('-');
  if (dash == string::npos) return false;
  string i1 = poss_range.substr(0, dash);
  string i2 = poss_range.substr(dash + 1);
  while (i1[0] == ' ') i1.erase(0, 1);
  while (i1[i1.size() - 1] == ' ') i1.erase(i1.size() - 1);
  while (i2[0] == ' ') i2.erase(0, 1);
  while (i2[i2.size() - 1] == ' ') i2.erase(i2.size() - 1);
  return valid(i1) && valid(i2);
}

DocID DocID::range_first(string range)
{
  if (!valid_range(range))
    throw Exception(Exception::INVALID_DOCID,
                    string("Invalid ID range: '") + range + "'");
  string::size_type dash = range.find('-');
  string id = range.substr(0, dash);
  while (id[0] == ' ') id.erase(0, 1);
  while (id[id.size() - 1] == ' ') id.erase(id.size() - 1);
  return DocID(id);
}

DocID DocID::range_second(string range)
{
  if (!valid_range(range))
    throw Exception(Exception::INVALID_DOCID,
                    string("Invalid ID range: '") + range + "'");
  string::size_type dash = range.find('-');
  string id = range.substr(dash + 1);
  while (id[0] == ' ') id.erase(0, 1);
  while (id[id.size() - 1] == ' ') id.erase(id.size() - 1);
  return DocID(id);
}

DocID::operator string(void) const
{
  char buff[7];
  sprintf(buff, "%06d", _id);
  return buff;
}


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS: FieldType
//
//----------------------------------------------------------------------

FieldType::FieldType(Connection &db, string poss_id)
{
  const vector<FieldType> &ftypes = db.field_types();
  vector<FieldType>::const_iterator it;
  for (it = ftypes.begin(); it != ftypes.end(); ++it)
    if (poss_id == it->id()) { *this = *it;  break; }
  if (it == ftypes.end())
    throw Exception(Exception::INVALID_FIELDTYPE,
                    string("Invalid field type: '") + poss_id + "'");
}

bool FieldType::valid(Connection &db, string poss_id)
{
  const vector<FieldType> &ftypes = db.field_types();
  for (vector<FieldType>::const_iterator it = ftypes.begin();
       it != ftypes.end(); ++it)
    if (poss_id == it->id()) return true;
  return false;
}

string FieldType::id(void) const
{
  if (!valid())
    throw Exception(Exception::INVALID_FIELDTYPE, "Uninitialised field type");
  return _id;
}

string FieldType::name(void) const
{
  if (!valid())
    throw Exception(Exception::INVALID_FIELDTYPE, "Uninitialised field type");
  return _name;
}

string FieldType::condition(void) const
{
  if (!valid())
    throw Exception(Exception::INVALID_FIELDTYPE, "Uninitialised field type");
  return _condition;
}


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS: DocType
//
//----------------------------------------------------------------------

DocType::DocType(Connection &db, string poss_id)
{
  const vector<DocType> &dtypes = db.doc_types();
  vector<DocType>::const_iterator it;
  for (it = dtypes.begin(); it != dtypes.end(); ++it)
    if (poss_id == it->id()) { *this = *it;  break; }
  if (it == dtypes.end())
    throw Exception(Exception::INVALID_DOCTYPE,
                    string("Invalid document type: '") + poss_id + "'");
}

bool DocType::valid(Connection &db, string poss_id)
{
  const vector<DocType> &dtypes = db.doc_types();
  for (vector<DocType>::const_iterator it = dtypes.begin();
       it != dtypes.end(); ++it)
    if (poss_id == it->id()) return true;
  return false;
}

string DocType::id(void) const
{
  if (!valid())
    throw Exception(Exception::INVALID_DOCTYPE, "Uninitialised document type");
  return _id;
}

string DocType::name(void) const
{
  if (!valid())
    throw Exception(Exception::INVALID_DOCTYPE, "Uninitialised document type");
  return _name;
}

string DocType::mandatory(void) const
{
  if (!valid())
    throw Exception(Exception::INVALID_DOCTYPE, "Uninitialised document type");
  return _mandatory;
}


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS: Holding
//
//----------------------------------------------------------------------

Holding::Holding(string poss_holding)
{
  if (!valid(poss_holding))
    throw Exception(Exception::INVALID_HOLDING,
                    string("Invalid holding value: '") + poss_holding + "'");
  _holding = poss_holding;
}

const char *Holding::_valid_holdings[] =
  { "-", "E", "P", "EP", "PE", "B", "L" };

bool Holding::valid(string poss_holding)
{
  bool found = false;
  for (int idx = 0;
       idx < sizeof(_valid_holdings) / sizeof(const char *); ++idx)
    if (poss_holding == _valid_holdings[idx]) { found = true; break; }
  return found;
}

vector<string> Holding::valid_holdings(void)
{
  vector<string> retval;
  for (int idx = 0;
       idx < sizeof(_valid_holdings) / sizeof(const char *); ++idx)
    retval.push_back(_valid_holdings[idx]);
  return retval;
}


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS: Status
//
//----------------------------------------------------------------------

Status::Status(string poss_status)
{
  if (!valid(poss_status))
    throw Exception(Exception::INVALID_STATUS,
                    string("Invalid status value: '") + poss_status + "'");
  _status = poss_status;
}

const char *Status::_valid_statuses[] =
  { "-", "R", "W", "N", "!", "+" };

bool Status::valid(string poss_status)
{
  bool found = false;
  for (int idx = 0;
       idx < sizeof(_valid_statuses) / sizeof(const char *); ++idx)
    if (poss_status == _valid_statuses[idx]) { found = true; break; }
  return found;
}

vector<string> Status::valid_statuses(void)
{
  vector<string> retval;
  for (int idx = 0;
       idx < sizeof(_valid_statuses) / sizeof(const char *); ++idx)
    retval.push_back(_valid_statuses[idx]);
  return retval;
}


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS: DocRecord
//
//----------------------------------------------------------------------

DocRecord::DocRecord(Connection &db, DocType type) :
  _db(db), _type(type), _holding("-"), _status("-")
{
  const vector<FieldType> &doc_fields = _db.doc_fields(type);
  for (vector<FieldType>::const_iterator it = doc_fields.begin();
       it != doc_fields.end(); ++it)
    _fields[*it] = "";
  _modified = false;
}

DocRecord::DocRecord(Connection &db, DocType type, DocID id) :
  _db(db), _type(type), _id(id)
{
  const vector<FieldType> &doc_fields = _db.doc_fields(type);
  for (vector<FieldType>::const_iterator it = doc_fields.begin();
       it != doc_fields.end(); ++it)
    _fields[*it] = "";
  _modified = false;
}

void DocRecord::set_holding(Holding holding)
{
  if (holding != _holding) {
    _holding = holding;
    _modified = true;
  }
}

void DocRecord::set_status(Status status)
{
  if (status != _status) {
    _status = status;
    _modified = true;
  }
}

void DocRecord::set_field(FieldType field_id, string value)
{
  map<FieldType, string>::iterator it = _fields.find(field_id);
  if (it == _fields.end())
    throw Exception(Exception::INVALID_FIELDTYPE,
                    "Invalid field type for document");
  it->second = value;
  _modified = true;
}

void DocRecord::intern(void)
{
  if (interned())
    throw Exception(Exception::SEQUENCE,
                    string("Document '") + string(_id) +
                    "' is already interned");

  // Get new record ID.

  char **rows;
  int nrow, ncol;
  char *errmsg;
  int res = sqlite3_get_table(_db.priv()->dbconn,
                              "SELECT next_value FROM doc_id;",
                              &rows, &nrow, &ncol, &errmsg);
  if (res != SQLITE_OK) {
    string excmsg = string("Query failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }
  if (nrow < 1 || ncol != 1)
    throw Exception(Exception::DB_ERROR,
                    "Internal DB error: bad result size!");
  DocID new_id(rows[1]);
  sqlite3_free_table(rows);
  DocID next_id(static_cast<int>(new_id) + 1);
  string cmd = "UPDATE doc_id SET next_value='" + string(next_id) + "';";
  res = sqlite3_exec(_db.priv()->dbconn, cmd.c_str(), 0, 0, &errmsg);
  if (res != SQLITE_OK) {
    string excmsg = string("Command failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }


  // Insert document entry.

  cmd = "INSERT INTO documents VALUES ('";
  cmd += string(new_id) + "', '";
  cmd += string(_type) + "', '";
  cmd += string(_holding) + "', '";
  cmd += string(_status) + "');";
  res = sqlite3_exec(_db.priv()->dbconn, cmd.c_str(), 0, 0, &errmsg);
  if (res != SQLITE_OK) {
    string excmsg = string("Command failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }


  // Insert document fields.

  for (map<FieldType, string>::const_iterator it = _fields.begin();
       it != _fields.end(); ++it) {
    if (it->second == "") continue;
    string cmd = "INSERT INTO doc_data VALUES ('";
    cmd += string(new_id) + "', '";
    cmd += string(it->first) + "', '";
    cmd += escape_string(it->second) + "');";
    int res = sqlite3_exec(_db.priv()->dbconn, cmd.c_str(), 0, 0, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Command failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }
  }

  _id = new_id;
}

void DocRecord::update(void)
{
  DocRecord *existing = _db.get_doc_by_id(_id);
  char *errmsg;
  if (existing->holding() != _holding) {
    string cmd = "UPDATE documents SET holding='";
    cmd += string(_holding) + "' WHERE id='" + string(_id) + "';";
    int res = sqlite3_exec(_db.priv()->dbconn, cmd.c_str(), 0, 0, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Command failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }
  }
  if (existing->status() != _status) {
    string cmd = "UPDATE documents SET status='";
    cmd += string(_status) + "' WHERE id='" + string(_id) + "';";
    int res = sqlite3_exec(_db.priv()->dbconn, cmd.c_str(), 0, 0, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Command failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }
  }


  // Deal with document fields one at a time - may need to insert,
  // update or delete....

  for (map<FieldType, string>::const_iterator it = _fields.begin();
       it != _fields.end(); ++it) {
    string existing_field = existing->fields()[it->first];
    string new_field = it->second;
    if (existing_field == "" && new_field == "")
      continue;
    else if (existing_field == "" && new_field != "") {
      // Insert a new one.

      string cmd = "INSERT INTO doc_data VALUES ('";
      cmd += string(_id) + "', '";
      cmd += string(it->first) + "', '";
      cmd += escape_string(new_field) + "');";
      int res = sqlite3_exec(_db.priv()->dbconn, cmd.c_str(), 0, 0, &errmsg);
      if (res != SQLITE_OK) {
        string excmsg = string("Command failed: ") + errmsg;
        sqlite3_free(errmsg);
        throw Exception(Exception::DB_ERROR, excmsg);
      }
    } else if (existing_field != "" && new_field == "") {
      // Delete one.

      string cmd = "DELETE FROM doc_data WHERE doc_id='";
      cmd += string(_id) + "' AND field_id='";
      cmd += string(it->first) + "';";
      int res = sqlite3_exec(_db.priv()->dbconn, cmd.c_str(), 0, 0, &errmsg);
      if (res != SQLITE_OK) {
        string excmsg = string("Command failed: ") + errmsg;
        sqlite3_free(errmsg);
        throw Exception(Exception::DB_ERROR, excmsg);
      }
    } else if (existing_field != new_field) {
      // Update one.

      string cmd = "UPDATE doc_data SET data='";
      cmd += escape_string(new_field) + "' WHERE doc_id='";
      cmd += string(_id) + "' AND field_id='";
      cmd += string(it->first) + "';";
      int res = sqlite3_exec(_db.priv()->dbconn, cmd.c_str(), 0, 0, &errmsg);
      if (res != SQLITE_OK) {
        string excmsg = string("Command failed: ") + errmsg;
        sqlite3_free(errmsg);
        throw Exception(Exception::DB_ERROR, excmsg);
      }
    }
  }
}


void DocRecord::display(ostream &ostr) const
{
  if (valid())
    ostr << "ID: " << string(_id);
  else
    ostr << "NOT INTERNED";
  ostr << "   HOLDING: " << string(_holding)
       << "   STATUS: " << string(_status) << endl;
  const vector<FieldType> &field_ids = _db.doc_fields(_type);
  DocRecord &doc = const_cast<DocRecord &>(*this);
  for (int idx = 0; idx < field_ids.size(); ++idx)
    ostr << "  " << string(field_ids[idx])
         << ": " << doc._fields[field_ids[idx]] << endl;
  ostr << endl;
}


bool DocRecord::mandatory_fields_ok(list<FieldType> &bad_field_types)
{
  return mandatory_check(_type.mandatory(), bad_field_types);
}


bool DocRecord::mandatory_check(string mandatory,
                                list<FieldType> &bad_field_types)
{
  bad_field_types.clear();
  if (mandatory == "")
    return true;
  else if (mandatory.find('(') == string::npos) {
    FieldType ftype(_db, mandatory);
    map<FieldType, string>::iterator loc = _fields.find(ftype);
    if (loc != _fields.end() && loc->second.size() > 0)
      return true;
    else {
      bad_field_types.push_back(ftype);
      return false;
    }
  } else {
    bool and_clause;
    int clause_start_len;
    if (mandatory.substr(0, 4) == "(AND") {
      and_clause = true;
      clause_start_len = 5;
    } else {
      and_clause = false;
      clause_start_len = 4;
    }

    string field_name_str =
      mandatory.substr(clause_start_len,
                       mandatory.size() - clause_start_len - 1);
    list<string> field_names;
    while (field_name_str.find(' ') != string::npos ||
           field_name_str[0] == '(') {
      if (field_name_str[0] == '(') {
        field_names.push_back(field_name_str.substr
                              (0, field_name_str.find(')') + 1));
        field_name_str = field_name_str.substr(field_name_str.find(')') + 2);
      } else {
        field_names.push_back(field_name_str.substr
                              (0, field_name_str.find(' ')));
        field_name_str = field_name_str.substr(field_name_str.find(' ') + 1);
      }
    }
    field_names.push_back(field_name_str);

    if (and_clause) {
      for (list<string>::iterator it = field_names.begin();
           it != field_names.end(); ++it)
        if (!mandatory_check(*it, bad_field_types)) return false;
      return true;
    } else {
      for (list<string>::iterator it = field_names.begin();
           it != field_names.end(); ++it)
        if (mandatory_check(*it, bad_field_types)) return true;
      bad_field_types.clear();
      for (list<string>::iterator it = field_names.begin();
           it != field_names.end(); ++it) {
        FieldType ftype(_db, mandatory);
        bad_field_types.push_back(ftype);
      }
      return false;
    }
  }
}


DocRecord *DocRecord::isi_import(Connection &db, map<string, string> &fields)
{
  string isi_article_type = fields["PT"];
  if (isi_article_type != "J" && isi_article_type != "S")
    throw Exception(Exception::MISC,
                    string("Don't know how to import ISI article type '") +
                    isi_article_type + "': talk to Ian!");

  // We only deal with importing normal articles at the moment.
  DocRecord *retval = new DocRecord(db, DocType(db, "AT"));

  string begin_page = "", end_page = "", pages = "";
  for (map<string, string>::iterator it = fields.begin();
       it != fields.end(); ++it) {
    string fn = it->first;
    string val = it->second;

    // Deal with the easy ones first...

    if (fn == "TI")
      retval->set_field(FieldType(db, "TI"), val);
    else if (fn == "LA")
      retval->set_field(FieldType(db, "LA"), val);
    else if (fn == "PY")
      retval->set_field(FieldType(db, "YR"), val);
    else if (fn == "VL")
      retval->set_field(FieldType(db, "VO"), val);
    else if (fn == "IS")
      retval->set_field(FieldType(db, "NO"), val);
    else if (fn == "DI")
      retval->set_field(FieldType(db, "NT"), string("doi:") + val);
    else if (fn == "AU") {
      list<string> isi_authors;
      while (val.find(';') != string::npos) {
        isi_authors.push_back(val.substr(0, val.find(';')));
        val = val.substr(val.find(';') + 1);
      }
      isi_authors.push_back(val);

      list<string> docmgr_authors;
      for (list<string>::iterator it = isi_authors.begin();
           it != isi_authors.end(); ++it) {
        string isi_author = *it;
        if (isi_author.find(',') == string::npos)
          docmgr_authors.push_back(isi_author);
        else {
          string surname = isi_author.substr(0, isi_author.find(','));
          string inits = isi_author.substr(isi_author.find(',') + 2);
          bool all_inits = true;
          for (int idx = 0; idx < inits.size(); ++idx)
            if (!isupper(inits[idx])) { all_inits = false;  break; }
          if (all_inits) {
            int init_count = inits.size();
            for (int idx = 0; idx < init_count; ++idx)
              inits.insert(idx * 3 + 1, ". ");
          } else
            inits += ' ';
          docmgr_authors.push_back(inits + surname);
        }
      }

      string author_field;
      for (list<string>::iterator it = docmgr_authors.begin();
           it != docmgr_authors.end(); ++it) {
        if (it != docmgr_authors.begin()) author_field += " and ";
        author_field += *it;
      }

      retval->set_field(FieldType(db, "AU"), author_field);
    } else if (fn == "SO" || fn == "SE") {
      // Need to translate journal names here!
      if (fn == "SE" ||
          (fn == "SO" && retval->fields()[FieldType(db, "JN")] == ""))
        retval->set_field(FieldType(db, "JN"), db.journal_abbrev(val));
    } else if (fn == "PD") {
      string month = val.substr(0, 3);
      month[0] = toupper(month[0]);
      month[1] = tolower(month[1]);
      month[2] = tolower(month[2]);
      retval->set_field(FieldType(db, "MO"), month);
    } else if (fn == "BP" || fn == "EP" || fn == "AR") {
      if (fn == "AR")
        pages = string("art. no. ") + val;
      else if (fn == "BP")
        begin_page = val;
      else if (fn == "EP")
        end_page = val;

      if (begin_page != "" && end_page != "")
        pages = begin_page + '-' + end_page;

      if (pages != "")
        retval->set_field(FieldType(db, "PG"), pages);
    }
  }

  return retval;
}


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS: Query
//
//----------------------------------------------------------------------

Query::Query(Connection &db, FieldType field, string val) :
  _db(&db)
{
  _tree = new QTreeNode;
  _tree->type = SIMPLE;
  _tree->clause = new QClause;
  _tree->clause->first = field;
  _tree->clause->second = val;
}

Query::Query(Connection &db, string quick) : _db(&db)
{
  _tree = new QTreeNode;
  _tree->type = QUICK;
  _tree->quick = new string(quick);
}

Query::Query(Connection &db, Status status) : _db(&db)
{
  _tree = new QTreeNode;
  _tree->type = STATUS;
  _tree->status = new Status(status);
}

Query::Query(Connection &db, Holding holding) : _db(&db)
{
  _tree = new QTreeNode;
  _tree->type = HOLDING;
  _tree->holding = new Holding(holding);
}

Query::Query(const Query &other) :
  _db(other._db), _tree(copy_tree(other._tree))
{ }

Query &Query::operator=(const Query &other)
{
  if (this != &other) {
    delete_tree(_tree);
    _db = other._db;
    _tree = copy_tree(other._tree);
  }
  return *this;
}

Query Query::operator&&(const Query &other)
{
  if (_db != other._db)
    throw Exception(Exception::INVALID_QUERY,
                    "Attempt to combine queries to two different databases");

  if (_tree == 0)
    return other;
  else if (other._tree == 0)
    return *this;

  Query retval(*_db);
  retval._tree = new QTreeNode;
  retval._tree->type = AND;
  retval._tree->node1 = copy_tree(_tree);
  retval._tree->node2 = copy_tree(other._tree);
  return retval;
}

Query Query::operator||(const Query &other)
{
  if (_db != other._db)
    throw Exception(Exception::INVALID_QUERY,
                    "Attempt to combine queries to two different databases");

  if (_tree == 0)
    return other;
  else if (other._tree == 0)
    return *this;

  Query retval(*_db);
  retval._tree = new QTreeNode;
  retval._tree->type = OR;
  retval._tree->node1 = copy_tree(_tree);
  retval._tree->node2 = copy_tree(other._tree);
  return retval;
}

Query::operator string(void) const
{
  string result = process_tree(_tree);
  result += ";";
  return result;
}


void Query::run(vector<DocID> &ids)
{
  _db->get_ids((string)(*this), ids);
}


void Query::delete_tree(Query::QTreeNode *orig)
{
  if (!orig) return;
  switch (orig->type) {
  case EMPTY:
    break;

  case QUICK:
    delete orig->quick;
    break;

  case STATUS:
    delete orig->status;
    break;

  case HOLDING:
    delete orig->holding;
    break;

  case SIMPLE:
    delete orig->clause;
    break;

  case AND:
  case OR:
    delete_tree(orig->node1);  delete orig->node1;
    delete_tree(orig->node2);  delete orig->node2;
    break;
  }
}


Query::QTreeNode *Query::copy_tree(Query::QTreeNode *orig)
{
  QTreeNode *retval = 0;
  if (orig) {
    switch (orig->type) {
    case EMPTY:
      break;

    case QUICK:
      retval = new QTreeNode;
      retval->type = QUICK;
      retval->quick = new string(*orig->quick);
      break;

    case STATUS:
      retval = new QTreeNode;
      retval->type = STATUS;
      retval->status = new Status(*orig->status);
      break;

    case HOLDING:
      retval = new QTreeNode;
      retval->type = HOLDING;
      retval->holding = new Holding(*orig->holding);
      break;

    case SIMPLE:
      retval = new QTreeNode;
      retval->type = SIMPLE;
      retval->clause = new QClause;
      retval->clause->first = orig->clause->first;
      retval->clause->second = orig->clause->second;
      break;

    case AND:
    case OR:
      retval = new QTreeNode;
      retval->type = orig->type;
      retval->node1 = copy_tree(orig->node1);
      retval->node2 = copy_tree(orig->node2);
      break;
    }
  }
  return retval;
}


string Query::process_tree(Query::QTreeNode *node)
{
  string retval = "";
  if (!node)
    retval = "SELECT DISTINCT doc_id FROM doc_data";
  else {
    switch (node->type) {
    case EMPTY:
      break;

    case QUICK: {
      string quick = *node->quick;
      list<string> words;
      while (quick.find(' ') != string::npos) {
        words.push_back(quick.substr(0, quick.find(' ')));
        quick.erase(0, quick.find(' ') + 1);
      }
      words.push_back(quick);

      retval = "SELECT DISTINCT doc_id FROM doc_data WHERE ";
      for (list<string>::iterator it = words.begin();
           it != words.end(); ++it) {
        if (it != words.begin())
          retval += " OR ";
        string word = *it;
        retval += "data LIKE '%";
        retval += escape_string(word);
        retval += "%' OR data LIKE '%";
        retval += escape_string(word.substr(0, 1));
        retval += '}';
        retval += escape_string(word.substr(1));
        retval += "%'";
      }
      break;
    }

    case STATUS: {
      retval = "SELECT DISTINCT id FROM documents WHERE status='";
      retval += string(*node->status) + "'";
      break;
    }

    case HOLDING: {
      retval = "SELECT DISTINCT id FROM documents WHERE holding='";
      retval += string(*node->holding) + "'";
      break;
    }

    case SIMPLE: {
      string val = node->clause->second;
      retval = "SELECT DISTINCT doc_id FROM doc_data WHERE field_id='";
      retval += node->clause->first.id();
      retval += "' AND ";
      if (node->clause->first.condition() == "=") {
        retval += "data = '";
        retval += escape_string(val);
        retval += "'";
      } else if (node->clause->first.condition() == "~*") {
        retval += "(data LIKE '%";
        retval += escape_string(val);
        retval += "%' OR data LIKE '%";
        retval += escape_string(node->clause->second.substr(0, 1));
        retval += '}';
        retval += escape_string(node->clause->second.substr(1));
        retval += "%')";
      }
      break;
    }

    case AND:
      retval = process_tree(node->node1);
      retval += " INTERSECT ";
      retval += process_tree(node->node2);
      break;

    case OR:
      retval = process_tree(node->node1);
      retval += " UNION ";
      retval += process_tree(node->node2);
      break;
    }
  }
  return retval;
}


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS: Connection
//
//----------------------------------------------------------------------

Connection::Connection(string dbfile) : _priv(new ConnectionPriv())
{
  // Connect to database.

  string dbf = dbfile;
  if (dbf[0] == '~') {
    string home = getenv("HOME");
    dbf = home + dbf.substr(1);
  }
  int res = sqlite3_open(dbf.c_str(), &_priv->dbconn);
  if (res != SQLITE_OK)
    throw Exception(Exception::DB_ERROR,
                    string("Failed to connect to database: ") +
                    sqlite3_errmsg(_priv->dbconn));


  // Retrieve document type data.

  char **rows;
  int nrow, ncol;
  char *errmsg;
  res = sqlite3_get_table(_priv->dbconn,
                          "SELECT id, doctype, mandatory FROM doc_types;",
                          &rows, &nrow, &ncol, &errmsg);
  if (res != SQLITE_OK) {
    string excmsg = string("Query failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }

  if (nrow < 1 || ncol != 3)
    throw Exception(Exception::DB_ERROR,
                    "Internal DB error: bad result size!");

  for (int dtype = 0; dtype < nrow; ++dtype) {
    char *id = rows[ncol * (dtype + 1) + 0];
    char *doctype = rows[ncol * (dtype + 1) + 1];
    char *mandatory = rows[ncol * (dtype + 1) + 2];
    _doc_types.push_back(DocType(id, doctype, mandatory));
  }

  sqlite3_free_table(rows);


  // Retrieve field type data.

  res = sqlite3_get_table(_priv->dbconn,
                          "SELECT id, field, condition FROM field_types;",
                          &rows, &nrow, &ncol, &errmsg);
  if (res != SQLITE_OK) {
    string excmsg = string("Query failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }

  if (nrow < 1 || ncol != 3)
    throw Exception(Exception::DB_ERROR,
                    "Internal DB error: bad result size!");

  for (int ftype = 0; ftype < nrow; ++ftype) {
    char *id = rows[ncol * (ftype + 1) + 0];
    char *field = rows[ncol * (ftype + 1) + 1];
    char *condition = rows[ncol * (ftype + 1) + 2];
    _field_types.push_back(FieldType(id, field, condition));
  }

  sqlite3_free_table(rows);


  // Retrieve document field data.

  for (vector<DocType>::const_iterator it = _doc_types.begin();
       it != _doc_types.end(); ++it) {
    string id = it->id();
    string query = string("SELECT field_id FROM doc_fields ") +
      string ("WHERE doctype_id ='") + id + "';";

    res = sqlite3_get_table(_priv->dbconn, query.c_str(),
                            &rows, &nrow, &ncol, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Query failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }

    if (nrow < 1 || ncol != 1)
      throw Exception(Exception::DB_ERROR,
                      "Internal DB error: bad result size!");

    vector<FieldType> fields;
    for (int field = 0; field < nrow; ++field)
      fields.push_back(FieldType(*this, rows[field + 1]));
    _doc_fields[*it] = fields;

    sqlite3_free_table(rows);
  }
}


Connection::~Connection()
{
  sqlite3_close(_priv->dbconn);
  delete _priv;
}


DocID Connection::max_doc_id(void)
{
  char **rows;
  int nrow, ncol;
  char *errmsg;
  int res = sqlite3_get_table(_priv->dbconn,
                              "SELECT MAX(id) FROM documents;",
                              &rows, &nrow, &ncol, &errmsg);
  if (res != SQLITE_OK) {
    string excmsg = string("Query failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }

  if (nrow != 1 || ncol != 1)
    throw Exception(Exception::DB_ERROR,
                    "Internal DB error: bad result size!");

  char *id_str = rows[1];
  if (id_str[0] == '\0')
    throw Exception(Exception::DB_ERROR, "Database is currently empty");
  DocID id(id_str);
  sqlite3_free_table(rows);
  return id;
}


void Connection::get_deleted_ids(vector<DocID> &ids)
{
  char **rows;
  int nrow, ncol;
  char *errmsg;
  int res = sqlite3_get_table(_priv->dbconn,
                              "SELECT id FROM deleted_ids;",
                              &rows, &nrow, &ncol, &errmsg);
  if (res != SQLITE_OK) {
    string excmsg = string("Query failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }

  if (nrow > 0 && ncol != 1)
    throw Exception(Exception::DB_ERROR,
                    "Internal DB error: bad result size!");

  ids.clear();
  for (int idx = 0; idx < nrow; ++idx) {
    char *id = rows[idx + 1];
    ids.push_back(DocID(id));
  }

  sqlite3_free_table(rows);
}


void Connection::get_ids(string query, vector<DocID> &ids)
{
  vector<DocID> deleted_ids;
  if (!_view_deleted) get_deleted_ids(deleted_ids);
  int del_size = deleted_ids.size();

  ids.clear();

  char **rows;
  int nrow, ncol;
  char *errmsg;
  //  qstr << query << endl;
  int res = sqlite3_get_table(_priv->dbconn, query.c_str(),
                              &rows, &nrow, &ncol, &errmsg);

  if (res != SQLITE_OK) {
    string excmsg = string("Query failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }

  if (nrow > 0 && ncol != 1)
    throw Exception(Exception::DB_ERROR,
                    "Internal DB error: bad result size!");

  for (int idx = 0; idx < nrow; ++idx) {
    DocID id = rows[idx + 1];
    bool deleted = false;
    for (int check = 0; check < deleted_ids.size(); ++check)
      if (deleted_ids[check] == id) {
        deleted = true;
        break;
      }
    if (!deleted)
      ids.push_back(id);
  }

  sqlite3_free_table(rows);
}


DocRecord *Connection::get_doc_by_id(DocID id)
{
  DocRecord *doc = 0;
  try {
    // Retrieve document metadata.

    string query1 =
      string("SELECT doc_type, holding, status "
             "FROM documents WHERE id='") +
      string(id) + "';";

    char **rows;
    int nrow, ncol;
    char *errmsg;
    int res = sqlite3_get_table(_priv->dbconn, query1.c_str(),
                                &rows, &nrow, &ncol, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Query failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }

    if (nrow != 1)
      throw Exception(Exception::DOCID_NOT_FOUND,
                      string("Document ID '") + string(id) + "' not found");

    if (ncol != 3)
      throw Exception(Exception::DB_ERROR,
                      "Internal DB error: bad result size!");

    DocType doc_type(*this, rows[3]);
    Holding holding(rows[4]);
    Status status(rows[5]);

    doc = new DocRecord(*this, doc_type, id);
    doc->set_holding(holding);
    doc->set_status(status);

    sqlite3_free_table(rows);


    string query2 =
      string("SELECT field_id, data FROM doc_data WHERE doc_id='") +
      string(id) + "';";
    res = sqlite3_get_table(_priv->dbconn, query2.c_str(),
                            &rows, &nrow, &ncol, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Query failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }

    if (nrow < 1 || ncol != 2)
      throw Exception(Exception::DB_ERROR,
                      "Internal DB error: bad result size!");

    for (int field = 0; field < nrow; ++field) {
      FieldType ftype(*this, rows[2 * (field + 1) + 0]);
      doc->set_field(ftype, rows[2 * (field + 1) + 1]);
    }

    sqlite3_free_table(rows);
  } catch (Exception &exc) {
    delete doc;
    if (exc.type() == Exception::INVALID_DOCTYPE)
      throw Exception(Exception::DB_ERROR,
                      "Unrecognised document type in database!");
    if (exc.type() == Exception::INVALID_FIELDTYPE)
      throw Exception(Exception::DB_ERROR,
                      "Unrecognised field type in database!");
    else if (exc.type() == Exception::INVALID_HOLDING)
      throw Exception(Exception::DB_ERROR,
                      "Invalid holding value in database!");
    else if (exc.type() == Exception::INVALID_STATUS)
      throw Exception(Exception::DB_ERROR,
                      "Invalid status value in database!");
    else if (exc.type() == Exception::INVALID_DATE)
      throw Exception(Exception::DB_ERROR,
                      "Invalid creation date value in database!");
    else
      throw;
  }

  doc->clear_modified();
  return doc;
}


void Connection::delete_doc(DocID id)
{
  vector<DocID> deleted_ids;
  get_deleted_ids(deleted_ids);
  bool found = false;
  for (int idx = 0; idx < deleted_ids.size(); ++idx)
    if (deleted_ids[idx] == id) { found = true; break; }
  if (!found) {
    char *errmsg;
    string cmd = "INSERT INTO deleted_ids VALUES ('" + string(id) + "');";
    int res = sqlite3_exec(_priv->dbconn, cmd.c_str(), 0, 0, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Command failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }
  }
}


void Connection::undelete_doc(DocID id)
{
  vector<DocID> deleted_ids;
  get_deleted_ids(deleted_ids);
  bool found = false;
  for (int idx = 0; idx < deleted_ids.size(); ++idx)
    if (deleted_ids[idx] == id) { found = true; break; }
  if (found) {
    char *errmsg;
    string cmd = "DELETE FROM deleted_ids WHERE id='" + string(id) + "';";
    int res = sqlite3_exec(_priv->dbconn, cmd.c_str(), 0, 0, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Command failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }
  }
}


void Connection::purge_deleted(void)
{
  vector<DocID> deleted_ids;
  get_deleted_ids(deleted_ids);
  char *errmsg;
  for (int idx = 0; idx < deleted_ids.size(); ++idx) {
    string id = string(deleted_ids[idx]);
    string cmd = "DELETE FROM doc_data WHERE doc_id='" + id + "';";
    int res = sqlite3_exec(_priv->dbconn, cmd.c_str(), 0, 0, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Command failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }

    cmd = "DELETE FROM documents WHERE id='" + id + "';";
    res = sqlite3_exec(_priv->dbconn, cmd.c_str(), 0, 0, &errmsg);
    if (res != SQLITE_OK) {
      string excmsg = string("Command failed: ") + errmsg;
      sqlite3_free(errmsg);
      throw Exception(Exception::DB_ERROR, excmsg);
    }
  }

  string cmd = "DELETE FROM deleted_ids;";
  int res = sqlite3_exec(_priv->dbconn, cmd.c_str(), 0, 0, &errmsg);
  if (res != SQLITE_OK) {
    string excmsg = string("Command failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }
}


string Connection::journal_abbrev(string full_name)
{
  string retval = full_name;

  char **rows;
  int nrow, ncol;
  char *errmsg;
  string query = "SELECT abbrev_title FROM journal_abbrevs WHERE full_title='";
  query += escape_string(full_name) + "';";
  int res = sqlite3_get_table(_priv->dbconn, query.c_str(),
                              &rows, &nrow, &ncol, &errmsg);
  if (res != SQLITE_OK) {
    string excmsg = string("Query failed: ") + errmsg;
    sqlite3_free(errmsg);
    throw Exception(Exception::DB_ERROR, excmsg);
  }

  if (nrow > 0) retval = rows[1];

  sqlite3_free_table(rows);

  return retval;
}


//----------------------------------------------------------------------
//
//  LOCAL FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

static string escape_string(string in_str)
{
  char *str_buff = sqlite3_mprintf("%q", in_str.c_str());
  string retval = str_buff;
  sqlite3_free(str_buff);
  return retval;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
