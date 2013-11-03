/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  File:   docmgr-schema.sql                                                */
/*  Author: Ian Ross                                                         */
/*  Date:   09-JUN-2004                                                      */
/*                                                                           */
/*  SQL schema for simple document and reference management system           */
/*  (specialised for Sqlite3).                                               */
/*                                                                           */
/* ========================================================================= */
/* ==== THE docmgr DATABASE SHOULD BE CREATED AND SELECTED BEFORE THIS  ==== */
/* ==== SCHEMA IS EXECUTED.                                             ==== */
/* ========================================================================= */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  TABLE DEFINITIONS                                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* documents: Master document list.                                          */
/*                                                                           */
/* Each document entered in the database has an entry in this table, through */
/* which it is assigned a unique document ID (a six-digit number padded on   */
/* the left with zeroes).  The documents table records the document type,    */
/* the type of the holding (whether it's a paper document, a book, something */
/* held electronically, or whatever) and the status (whether I've read it,   */
/* written it, etc.), and the creation date of the entry.  Document types    */
/* are taken from the doc_types table, defined below.                        */
/*                                                                           */
/* +-------------+---------------------------------------------------------+ */
/* | doc_id      | Document ID (six digit number, padded on left with      | */
/* |             | zeroes.                                                 | */
/* +-------------+---------------------------------------------------------+ */
/* | doc_type    | Two character code for document type (taken from        | */
/* |             | doc_types table.                                        | */
/* +-------------+---------------------------------------------------------+ */
/* | holding     | Up to three characters for representing holding types:  | */
/* |             | 'E' for electronic, 'P' for paper, 'B' for a personal   | */
/* |             | book, 'L' for a library book, '-' for none.             | */
/* +-------------+---------------------------------------------------------+ */
/* | status      | Document status: '-' unread, 'R' read, 'P' personal.    | */
/* +-------------+---------------------------------------------------------+ */

CREATE TABLE documents (
  id           CHAR(6)     NOT NULL,
  doc_type     CHAR(2)     NOT NULL,
  holding      VARCHAR(3)  DEFAULT '-',
  status       CHAR(1)     DEFAULT '-',

  PRIMARY KEY (id)
);


/* doc_is: Table for generating new document IDs.                            */
/*                                                                           */
/* Table with single column and single row, recording the next document ID   */
/* to use.                                                                   */
/*                                                                           */
/* +------------+-----------------------------------------------------------+ */
/* | next_value | Next document ID to use (six digits).                     | */
/* +------------+-----------------------------------------------------------+ */

CREATE TABLE doc_id (
  next_value CHAR(6) NOT NULL
);


/* doc_types: Document type list.                                            */
/*                                                                           */
/* Each entry in this table defines a possible document type, e.g. a journal */
/* article, a book, etc.   relevant to the document type.  The two-letter    */
/* field IDs are taken from the field_types table.                           */
/*                                                                           */
/* +-----------+-----------------------------------------------------------+ */
/* | id        | Document type ID (two letters, used to refer to document  | */
/* |           | types in all other tables).                               | */
/* +-----------+-----------------------------------------------------------+ */
/* | doctype   | Full name of the document type - this is actually the     | */
/* |           | name used by BibTeX for documents of this type.           | */
/* +-----------+-----------------------------------------------------------+ */
/* | mandatory | Description of the fields that are REQUIRED for a given   | */
/* |           | document type.  This is a string giving field IDs (as     | */
/* |           | defined in the field_types table) within a Scheme         | */
/* |           | expression representin a Boolean composition of field     | */
/* |           | presence tests.  So, '(AND (OR AU ED) PU TI YR)' means    | */
/* |           | that either the AU or ED field must be present, along     | */
/* |           | with the PU, TI and YR fields.                            | */
/* +-----------+-----------------------------------------------------------+ */

CREATE TABLE doc_types (
  id         CHAR(2)       NOT NULL,
  doctype    VARCHAR(15)   NOT NULL,
  mandatory  VARCHAR(255)  DEFAULT '',

  PRIMARY KEY (id)
);


/* field_types: Document field type list.                                    */
/*                                                                           */
/* Each entry in this table defines a possible field in a document entry,    */
/* e.g. a document title, author, publisher, etc.  The field types are       */
/* identified by two-letter field IDs that are then used to identify field   */
/* types in other tables.                                                    */
/*                                                                           */
/* +-----------+-----------------------------------------------------------+ */
/* | id        | Field type ID (two letters, used to refer to field types  | */
/* |           | in all other tables).                                     | */
/* +-----------+-----------------------------------------------------------+ */
/* | field     | Full name of the field type - this is actually the name   | */
/* |           | used by BibTeX for fields of this type.                   | */
/* +-----------+-----------------------------------------------------------+ */
/* | condition | The SQL query condition used for comparing search         | */
/* |           | expression to field values - some fields use an exact     | */
/* |           | match, and some use regular expression matching (which is | */
/* |           | a PostgreSQL specific extension).                         | */
/* +-----------+-----------------------------------------------------------+ */

CREATE TABLE field_types (
  id         CHAR(2)      NOT NULL,
  field      VARCHAR(15)  NOT NULL,
  condition  VARCHAR(3)   NOT NULL,

  PRIMARY KEY (id)
);


/* doc_data: Master document -> field association table.                     */
/*                                                                           */
/* This table has one row for each field of each document entry, and holds   */
/* the data for each field relevant to the document type.  The two-letter    */
/* field IDs are taken from the field_types table.                           */
/*                                                                           */
/* +----------+------------------------------------------------------------+ */
/* | doc_id   | Document ID (six digit number, padded on left with zeroes. | */
/* +----------+------------------------------------------------------------+ */
/* | field_id | ID of a field in the entry for the document.  These two-   | */
/* |          | letter IDs come from the field_types table - the fields    | */
/* |          | associated with a given document type are defined in the   | */
/* |          | doc_fields table.                                          | */
/* +----------+------------------------------------------------------------+ */
/* | data     | Field data value (free-form ASCII data).                   | */
/* +----------+------------------------------------------------------------+ */

CREATE TABLE doc_data (
  doc_id    CHAR(6)       NOT NULL,
  field_id  CHAR(2)       NOT NULL,
  data      TEXT,

  PRIMARY KEY (doc_id, field_id)
);


/* doc_fields: The fields associated with a given document type.             */
/*                                                                           */
/* This table has an entry for each field in each document type, defining    */
/* the fields that may be associated with a given document.                  */
/*                                                                           */
/* +------------+----------------------------------------------------------+ */
/* | doctype_id | Document type ID.  These two-letter IDs come from the    | */
/* |            | doc_types table.                                         | */
/* +------------+----------------------------------------------------------+ */
/* | field_id   | ID of a field in the entry for the document.  These two- | */
/* |            | letter IDs come from the field_types table.              | */
/* +------------+----------------------------------------------------------+ */

CREATE TABLE doc_fields (
  doctype_id  CHAR(2)  NOT NULL,
  field_id    CHAR(2)  NOT NULL,

  PRIMARY KEY (doctype_id, field_id)
);


/* deleted_ids: List of deleted document IDs.                                */
/*                                                                           */
/* This table lists document IDs that have been deleted from the database    */
/* (as far as users of front-end applications are concerned) but have not    */
/* yet been purged.                                                          */
/*                                                                           */
/* +-------------+---------------------------------------------------------+ */
/* | doc_id      | Document ID (six digit number, padded on left with      | */
/* |             | zeroes.                                                 | */
/* +-------------+---------------------------------------------------------+ */

CREATE TABLE deleted_ids (
  id           CHAR(6)     NOT NULL,

  PRIMARY KEY (id)
);


/* journal_abbrevs: Translation between full and abbreviated journal names.  */
/*                                                                           */
/* This table lists full journal names (as all capitals), along with         */
/* something very close to their ISO abbreviated titles.                     */
/*                                                                           */
/* +--------------+--------------------------------------------------------+ */
/* | full_title   | Full journal title.                                    | */
/* +--------------+--------------------------------------------------------+ */
/* | abbrev_title | Abbreviated title.                                     | */
/* +--------------+--------------------------------------------------------+ */

CREATE TABLE journal_abbrevs (
  full_title TEXT    NOT NULL,
  abbrev_title TEXT  NOT NULL,

  PRIMARY KEY (full_title)
);


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  DATA INITIALISATION                                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* Define field types. */

INSERT INTO field_types VALUES ('AD', 'address',      '~*');
INSERT INTO field_types VALUES ('BT', 'booktitle',    '~*');
INSERT INTO field_types VALUES ('CA', 'chapter',      '=');
INSERT INTO field_types VALUES ('EN', 'edition',      '=');
INSERT INTO field_types VALUES ('ED', 'editor',       '~*');
INSERT INTO field_types VALUES ('HO', 'howpublished', '~*');
INSERT INTO field_types VALUES ('IN', 'institution',  '~*');
INSERT INTO field_types VALUES ('JN', 'journal',      '~*');
INSERT INTO field_types VALUES ('MO', 'month',        '=');
INSERT INTO field_types VALUES ('NT', 'note',         '~*');
INSERT INTO field_types VALUES ('NO', 'number',       '=');
INSERT INTO field_types VALUES ('OG', 'organization', '~*');
INSERT INTO field_types VALUES ('PU', 'publisher',    '~*');
INSERT INTO field_types VALUES ('SH', 'school',       '~*');
INSERT INTO field_types VALUES ('TI', 'title',        '~*');
INSERT INTO field_types VALUES ('TY', 'type',         '=');
INSERT INTO field_types VALUES ('UR', 'url',          '~*');
INSERT INTO field_types VALUES ('YR', 'year',         '=');
INSERT INTO field_types VALUES ('SE', 'series',       '~*');
INSERT INTO field_types VALUES ('VO', 'volume',       '=');
INSERT INTO field_types VALUES ('AU', 'author',       '~*');
INSERT INTO field_types VALUES ('LA', 'language',     '~*');
INSERT INTO field_types VALUES ('IS', 'isbn',         '=');
INSERT INTO field_types VALUES ('KW', 'keywords',     '~*');
INSERT INTO field_types VALUES ('EP', 'eprint',       '=');
INSERT INTO field_types VALUES ('PG', 'pages',        '~*');
INSERT INTO field_types VALUES ('XR', 'crossref',     '=');
INSERT INTO field_types VALUES ('TG', '',             '~*');


/* Define document types with their associated fields. */

INSERT INTO doc_types VALUES ('AT', 'Article', '(AND AU JN TI YR)');
INSERT INTO doc_fields VALUES ('AT', 'AU');
INSERT INTO doc_fields VALUES ('AT', 'EP');
INSERT INTO doc_fields VALUES ('AT', 'JN');
INSERT INTO doc_fields VALUES ('AT', 'KW');
INSERT INTO doc_fields VALUES ('AT', 'LA');
INSERT INTO doc_fields VALUES ('AT', 'MO');
INSERT INTO doc_fields VALUES ('AT', 'NO');
INSERT INTO doc_fields VALUES ('AT', 'NT');
INSERT INTO doc_fields VALUES ('AT', 'PG');
INSERT INTO doc_fields VALUES ('AT', 'TI');
INSERT INTO doc_fields VALUES ('AT', 'UR');
INSERT INTO doc_fields VALUES ('AT', 'VO');
INSERT INTO doc_fields VALUES ('AT', 'YR');
INSERT INTO doc_fields VALUES ('AT', 'TG');

INSERT INTO doc_types VALUES ('BK', 'Book', '(AND (OR AU ED) PU TI YR)');
INSERT INTO doc_fields VALUES ('BK', 'AD');
INSERT INTO doc_fields VALUES ('BK', 'AU');
INSERT INTO doc_fields VALUES ('BK', 'ED');
INSERT INTO doc_fields VALUES ('BK', 'EN');
INSERT INTO doc_fields VALUES ('BK', 'IS');
INSERT INTO doc_fields VALUES ('BK', 'KW');
INSERT INTO doc_fields VALUES ('BK', 'LA');
INSERT INTO doc_fields VALUES ('BK', 'MO');
INSERT INTO doc_fields VALUES ('BK', 'NT');
INSERT INTO doc_fields VALUES ('BK', 'PU');
INSERT INTO doc_fields VALUES ('BK', 'SE');
INSERT INTO doc_fields VALUES ('BK', 'TI');
INSERT INTO doc_fields VALUES ('BK', 'UR');
INSERT INTO doc_fields VALUES ('BK', 'VO');
INSERT INTO doc_fields VALUES ('BK', 'YR');
INSERT INTO doc_fields VALUES ('BK', 'TG');

INSERT INTO doc_types VALUES ('BL', 'Booklet', 'TI');
INSERT INTO doc_fields VALUES ('BL', 'AD');
INSERT INTO doc_fields VALUES ('BL', 'AU');
INSERT INTO doc_fields VALUES ('BL', 'EN');
INSERT INTO doc_fields VALUES ('BL', 'HO');
INSERT INTO doc_fields VALUES ('BL', 'KW');
INSERT INTO doc_fields VALUES ('BL', 'LA');
INSERT INTO doc_fields VALUES ('BL', 'MO');
INSERT INTO doc_fields VALUES ('BL', 'NT');
INSERT INTO doc_fields VALUES ('BL', 'TI');
INSERT INTO doc_fields VALUES ('BL', 'UR');
INSERT INTO doc_fields VALUES ('BL', 'YR');
INSERT INTO doc_fields VALUES ('BL', 'TG');

INSERT INTO doc_types VALUES ('IB', 'InBook', '(AND AU (OR CA PG) TI XR)');
INSERT INTO doc_fields VALUES ('IB', 'AU');
INSERT INTO doc_fields VALUES ('IB', 'CA');
INSERT INTO doc_fields VALUES ('IB', 'KW');
INSERT INTO doc_fields VALUES ('IB', 'LA');
INSERT INTO doc_fields VALUES ('IB', 'NT');
INSERT INTO doc_fields VALUES ('IB', 'PG');
INSERT INTO doc_fields VALUES ('IB', 'TI');
INSERT INTO doc_fields VALUES ('IB', 'TY');
INSERT INTO doc_fields VALUES ('IB', 'YR');
INSERT INTO doc_fields VALUES ('IB', 'XR');
INSERT INTO doc_fields VALUES ('IB', 'TG');

INSERT INTO doc_types VALUES ('IC', 'InCollection', '(AND AU TI YR XR)');
INSERT INTO doc_fields VALUES ('IC', 'AU');
INSERT INTO doc_fields VALUES ('IC', 'CA');
INSERT INTO doc_fields VALUES ('IC', 'KW');
INSERT INTO doc_fields VALUES ('IC', 'LA');
INSERT INTO doc_fields VALUES ('IC', 'NT');
INSERT INTO doc_fields VALUES ('IC', 'PG');
INSERT INTO doc_fields VALUES ('IC', 'TI');
INSERT INTO doc_fields VALUES ('IC', 'TY');
INSERT INTO doc_fields VALUES ('IC', 'YR');
INSERT INTO doc_fields VALUES ('IC', 'XR');
INSERT INTO doc_fields VALUES ('IC', 'TG');

INSERT INTO doc_types VALUES ('IP', 'InProceedings', '(AND AU TI YR XR)');
INSERT INTO doc_fields VALUES ('IP', 'AU');
INSERT INTO doc_fields VALUES ('IP', 'KW');
INSERT INTO doc_fields VALUES ('IP', 'LA');
INSERT INTO doc_fields VALUES ('IP', 'NT');
INSERT INTO doc_fields VALUES ('IP', 'PG');
INSERT INTO doc_fields VALUES ('IP', 'TI');
INSERT INTO doc_fields VALUES ('IP', 'YR');
INSERT INTO doc_fields VALUES ('IP', 'XR');
INSERT INTO doc_fields VALUES ('IP', 'TG');

INSERT INTO doc_types VALUES ('MA', 'Manual', 'TI');
INSERT INTO doc_fields VALUES ('MA', 'AD');
INSERT INTO doc_fields VALUES ('MA', 'AU');
INSERT INTO doc_fields VALUES ('MA', 'EN');
INSERT INTO doc_fields VALUES ('MA', 'KW');
INSERT INTO doc_fields VALUES ('MA', 'LA');
INSERT INTO doc_fields VALUES ('MA', 'MO');
INSERT INTO doc_fields VALUES ('MA', 'NT');
INSERT INTO doc_fields VALUES ('MA', 'OG');
INSERT INTO doc_fields VALUES ('MA', 'TI');
INSERT INTO doc_fields VALUES ('MA', 'UR');
INSERT INTO doc_fields VALUES ('MA', 'YR');
INSERT INTO doc_fields VALUES ('MA', 'TG');

INSERT INTO doc_types VALUES ('MT', 'MastersThesis', '(AND AU SH TI YR)');
INSERT INTO doc_fields VALUES ('MT', 'AD');
INSERT INTO doc_fields VALUES ('MT', 'AU');
INSERT INTO doc_fields VALUES ('MT', 'EP');
INSERT INTO doc_fields VALUES ('MT', 'KW');
INSERT INTO doc_fields VALUES ('MT', 'LA');
INSERT INTO doc_fields VALUES ('MT', 'MO');
INSERT INTO doc_fields VALUES ('MT', 'NT');
INSERT INTO doc_fields VALUES ('MT', 'SH');
INSERT INTO doc_fields VALUES ('MT', 'TI');
INSERT INTO doc_fields VALUES ('MT', 'TY');
INSERT INTO doc_fields VALUES ('MT', 'UR');
INSERT INTO doc_fields VALUES ('MT', 'YR');
INSERT INTO doc_fields VALUES ('MT', 'TG');

INSERT INTO doc_types VALUES ('MS', 'Misc', '');
INSERT INTO doc_fields VALUES ('MS', 'AU');
INSERT INTO doc_fields VALUES ('MS', 'EP');
INSERT INTO doc_fields VALUES ('MS', 'HO');
INSERT INTO doc_fields VALUES ('MS', 'IN');
INSERT INTO doc_fields VALUES ('MS', 'KW');
INSERT INTO doc_fields VALUES ('MS', 'LA');
INSERT INTO doc_fields VALUES ('MS', 'MO');
INSERT INTO doc_fields VALUES ('MS', 'NT');
INSERT INTO doc_fields VALUES ('MS', 'PU');
INSERT INTO doc_fields VALUES ('MS', 'TI');
INSERT INTO doc_fields VALUES ('MS', 'UR');
INSERT INTO doc_fields VALUES ('MS', 'VO');
INSERT INTO doc_fields VALUES ('MS', 'YR');
INSERT INTO doc_fields VALUES ('MS', 'TG');

INSERT INTO doc_types VALUES ('PH', 'PhdThesis', '(AND AU SH TI YR)');
INSERT INTO doc_fields VALUES ('PH', 'AD');
INSERT INTO doc_fields VALUES ('PH', 'AU');
INSERT INTO doc_fields VALUES ('PH', 'EP');
INSERT INTO doc_fields VALUES ('PH', 'KW');
INSERT INTO doc_fields VALUES ('PH', 'LA');
INSERT INTO doc_fields VALUES ('PH', 'MO');
INSERT INTO doc_fields VALUES ('PH', 'NT');
INSERT INTO doc_fields VALUES ('PH', 'SH');
INSERT INTO doc_fields VALUES ('PH', 'TI');
INSERT INTO doc_fields VALUES ('PH', 'TY');
INSERT INTO doc_fields VALUES ('PH', 'UR');
INSERT INTO doc_fields VALUES ('PH', 'YR');
INSERT INTO doc_fields VALUES ('PH', 'TG');

INSERT INTO doc_types VALUES ('PR', 'Proceedings', '(AND TI YR)');
INSERT INTO doc_fields VALUES ('PR', 'AD');
INSERT INTO doc_fields VALUES ('PR', 'ED');
INSERT INTO doc_fields VALUES ('PR', 'EP');
INSERT INTO doc_fields VALUES ('PR', 'KW');
INSERT INTO doc_fields VALUES ('PR', 'LA');
INSERT INTO doc_fields VALUES ('PR', 'MO');
INSERT INTO doc_fields VALUES ('PR', 'NT');
INSERT INTO doc_fields VALUES ('PR', 'OG');
INSERT INTO doc_fields VALUES ('PR', 'PU');
INSERT INTO doc_fields VALUES ('PR', 'SE');
INSERT INTO doc_fields VALUES ('PR', 'TI');
INSERT INTO doc_fields VALUES ('PR', 'UR');
INSERT INTO doc_fields VALUES ('PR', 'VO');
INSERT INTO doc_fields VALUES ('PR', 'YR');
INSERT INTO doc_fields VALUES ('PR', 'TG');

INSERT INTO doc_types VALUES ('TR', 'TechReport', '(AND AU IN TI YR)');
INSERT INTO doc_fields VALUES ('TR', 'AD');
INSERT INTO doc_fields VALUES ('TR', 'AU');
INSERT INTO doc_fields VALUES ('TR', 'EP');
INSERT INTO doc_fields VALUES ('TR', 'IN');
INSERT INTO doc_fields VALUES ('TR', 'KW');
INSERT INTO doc_fields VALUES ('TR', 'LA');
INSERT INTO doc_fields VALUES ('TR', 'MO');
INSERT INTO doc_fields VALUES ('TR', 'NO');
INSERT INTO doc_fields VALUES ('TR', 'NT');
INSERT INTO doc_fields VALUES ('TR', 'TI');
INSERT INTO doc_fields VALUES ('TR', 'TY');
INSERT INTO doc_fields VALUES ('TR', 'UR');
INSERT INTO doc_fields VALUES ('TR', 'YR');
INSERT INTO doc_fields VALUES ('TR', 'TG');

INSERT INTO doc_types VALUES ('UP', 'Unpublished', '(AND AU TI)');
INSERT INTO doc_fields VALUES ('UP', 'AU');
INSERT INTO doc_fields VALUES ('UP', 'EP');
INSERT INTO doc_fields VALUES ('UP', 'KW');
INSERT INTO doc_fields VALUES ('UP', 'LA');
INSERT INTO doc_fields VALUES ('UP', 'MO');
INSERT INTO doc_fields VALUES ('UP', 'NT');
INSERT INTO doc_fields VALUES ('UP', 'TI');
INSERT INTO doc_fields VALUES ('UP', 'UR');
INSERT INTO doc_fields VALUES ('UP', 'YR');
INSERT INTO doc_fields VALUES ('UP', 'TG');

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
