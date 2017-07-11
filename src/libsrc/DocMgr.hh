//----------------------------------------------------------------------
//
//  FILE:   DocMgr.hh
//  AUTHOR: Ian Ross
//  DATE:   11-SEP-2004
//
//  Main header file for document manager library.
//
//----------------------------------------------------------------------

#ifndef _H_DOCMGR_
#define _H_DOCMGR_

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <string>
#include <vector>
#include <map>
#include <list>

using namespace std;


//----------------------------------------------------------------------
//
//  CLASS DEFINITIONS
//
//----------------------------------------------------------------------

namespace DocMgr {

  class Connection;


  // Document IDs.

  class DocID {
  public:

    DocID() : _id(-1) { }
    DocID(const char *poss_id);
    DocID(string poss_id);
    DocID(int poss_id);

    DocID(const DocID &other) : _id(other._id) { }
    DocID &operator=(const DocID &other) { _id = other._id; return *this; }

    bool operator==(const DocID &other) const { return _id == other._id; }
    bool operator!=(const DocID &other) const { return _id != other._id; }
    bool operator<(const DocID &other) const { return _id < other._id; }
    bool operator<=(const DocID &other) const { return _id <= other._id; }
    bool operator>(const DocID &other) const { return _id > other._id; }
    bool operator>=(const DocID &other) const { return _id >= other._id; }

    bool valid(void) const { return _id != -1; }
    static bool valid(string poss_id);
    static bool valid(int poss_id);

    operator int(void) const { return _id; }
    operator string(void) const;

  private:

    int _id;
  };


  // Field types.

  class FieldType {
  public:
    friend class Connection;

    FieldType() : _id(""), _name(""), _condition("") { }
    FieldType(Connection &db, string poss_id);
    FieldType(const FieldType &other) :
      _id(other._id), _name(other._name), _condition(other._condition) { }
    FieldType &operator=(const FieldType &other)
    { _id = other._id;  _name = other._name;  _condition = other._condition;
    return *this; }

    bool valid(void) const { return _id != ""; }
    static bool valid(Connection &db, string poss_id);

    bool operator==(const FieldType &other) const { return _id == other._id; }
    bool operator!=(const FieldType &other) const { return _id != other._id; }
    bool operator<(const FieldType &other) const { return _id < other._id; }
    bool operator<=(const FieldType &other) const { return _id <= other._id; }
    bool operator>(const FieldType &other) const { return _id > other._id; }
    bool operator>=(const FieldType &other) const { return _id >= other._id; }

    operator string(void) const { return _id; }

    string id(void) const;
    string name(void) const;
    string condition(void) const;

  private:

    FieldType(string id, string name, string condition) :
      _id(id), _name(name), _condition(condition) { }

    string _id;
    string _name;
    string _condition;
  };


  // Document types.

  class DocType {
  public:
    friend class Connection;

    DocType() : _id(""), _name(""), _mandatory("") { }
    DocType(Connection &db, string poss_id);
    DocType(const DocType &other) :
      _id(other._id), _name(other._name), _mandatory(other._mandatory) { }
    DocType &operator=(const DocType &other)
    { _id = other._id;  _name = other._name;  _mandatory = other._mandatory;
    return *this; }

    bool operator==(const DocType &other) const { return _id == other._id; }
    bool operator!=(const DocType &other) const { return _id != other._id; }
    bool operator<(const DocType &other) const { return _id < other._id; }
    bool operator<=(const DocType &other) const { return _id <= other._id; }
    bool operator>(const DocType &other) const { return _id > other._id; }
    bool operator>=(const DocType &other) const { return _id >= other._id; }

    bool valid(void) const { return _id != ""; }
    static bool valid(Connection &db, string poss_id);

    operator string(void) const { return _id; }

    string id(void) const;
    string name(void) const;
    string mandatory(void) const;

  private:

    DocType(string id, string name, string mandatory) :
      _id(id), _name(name), _mandatory(mandatory) { }

    string _id;
    string _name;
    string _mandatory;
  };


  // Entry holding status.

  class Holding {
  public:

    Holding() : _holding("") { }
    Holding(string poss_holding);
    Holding(const Holding &other) : _holding(other._holding) { }
    Holding &operator=(const Holding &other)
    { _holding = other._holding;  return *this; }

    bool operator==(const Holding &other) const
    { return _holding == other._holding; }
    bool operator!=(const Holding &other) const
    { return _holding != other._holding; }

    bool valid(void) const { return _holding != ""; }
    static bool valid(string poss_holding);
    static vector<string> valid_holdings(void);
    operator string(void) const { return _holding; }

  private:

    string _holding;
    static const char *_valid_holdings[];
  };


  // Entry read status.

  class Status {
  public:

    Status() : _status("") { }
    Status(string poss_status);
    Status(const Status &other) : _status(other._status) { }
    Status &operator=(const Status &other)
    { _status = other._status;  return *this; }

    bool operator==(const Status &other) const
    { return _status == other._status; }
    bool operator!=(const Status &other) const
    { return _status != other._status; }

    bool valid(void) const { return _status != ""; }
    static bool valid(string poss_status);
    static vector<string> valid_statuses(void);
    operator string(void) const { return _status; }

  private:

    string _status;
    static const char *_valid_statuses[];
  };


  // Exceptions.

  class Exception {
  public:

    enum ExcType { MISC,
                   INVALID_HOLDING, INVALID_STATUS,
                   INVALID_DOCID, INVALID_DATE,
                   INVALID_FIELDTYPE, INVALID_DOCTYPE, INVALID_QUERY,
                   DOCID_NOT_FOUND, SEQUENCE, DB_ERROR };

    Exception(string msg) : _type(MISC), _msg(msg) { }
    Exception(ExcType type, string msg) : _type(type), _msg(msg) { }
    virtual ~Exception() { }

    ExcType type(void) const { return _type; }
    string msg(void) const { return _msg; }

  private:

    ExcType _type;
    string _msg;
  };


  // Article entries.

  class DocRecord {
  public:

    friend class Connection;

    DocRecord(Connection &db, DocType type);

    DocType type(void) const { return _type; }
    DocID id(void) const { return _id; }
    Holding holding(void) const { return _holding; }
    Status status(void) const { return _status; }
    map<FieldType, string> &fields(void) { return _fields; }

    void set_holding(Holding holding);
    void set_status(Status status);
    void set_field(FieldType field_id, string value);

    bool valid(void) const { return _id.valid(); }
    bool interned(void) const { return _id.valid(); }
    bool modified(void) const { return _modified; }
    void intern(void);
    void update(void);

    bool mandatory_fields_ok(list<FieldType> &bad_field_types);

    void display(ostream &ostr) const;

    static DocRecord *isi_import(Connection &db,
                                 map<string, string> &fields);

  private:

    DocRecord(Connection &db, DocType type, DocID id);
    void clear_modified(void) { _modified = false; }

    bool mandatory_check(string mandatory, list<FieldType> &bad_field_types);

    Connection &_db;
    DocType _type;
    DocID _id;
    Holding _holding;
    Status _status;
    map<FieldType, string> _fields;
    bool _modified;
  };


  // Database queries.

  class Connection;
  class Query {
  public:

    Query(Connection &db) : _db(&db), _tree(0) { }
    Query(Connection &db, string quick);
    Query(Connection &db, Status status);
    Query(Connection &db, Holding holding);
    Query(Connection &db, FieldType field, string val);
    Query(const Query &other);
    ~Query() { delete_tree(_tree); }
    Query &operator=(const Query &other);

    Query operator&&(const Query &other);
    Query operator||(const Query &other);

    operator string(void) const;
    void run(vector<DocID> &ids);

  private:

    typedef pair<FieldType, string> QClause;
    enum QType { EMPTY, QUICK, SIMPLE, HOLDING, STATUS, AND, OR };

    struct QTreeNode {
      QType type;
      string *quick;
      Status *status;
      Holding *holding;
      QClause *clause;
      QTreeNode *node1;
      QTreeNode *node2;
    };

    static void delete_tree(QTreeNode *orig);
    static QTreeNode *copy_tree(QTreeNode *orig);
    static string process_tree(QTreeNode *node);

    Connection *_db;
    QTreeNode *_tree;
  };


  // Database connection.

  struct ConnectionPriv;
  class Connection {
  public:

    friend class DocRecord;

    Connection(string dbfile);
    ~Connection();

    DocID max_doc_id(void);
    DocRecord *get_doc_by_id(DocID id);
    void get_ids(string query, vector<DocID> &ids);
    void get_deleted_ids(vector<DocID> &ids);

    const vector<DocType> &doc_types(void) { return _doc_types; }
    const vector<FieldType> &field_types(void) { return _field_types; }
    const vector<FieldType> &doc_fields(DocType doc_type)
    { return _doc_fields[doc_type]; }

    bool view_deleted(void) const { return _view_deleted; }
    void set_view_deleted(bool view_deleted) { _view_deleted = view_deleted; }

    void delete_doc(DocID id);
    void undelete_doc(DocID id);

    void purge_deleted(void);

    string journal_abbrev(string full_name);

  private:

    ConnectionPriv *priv(void) const { return _priv; }

    ConnectionPriv *_priv;
    vector<DocType> _doc_types;
    vector<FieldType> _field_types;
    map<DocType, vector<FieldType> > _doc_fields;
    bool _view_deleted;
  };

  inline ostream &operator<<(ostream &ostr, const DocRecord &doc)
  {
    doc.display(ostr);
    return ostr;
  }
};

#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
