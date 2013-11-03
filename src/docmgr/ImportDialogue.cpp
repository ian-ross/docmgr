//----------------------------------------------------------------------
//
//  FILE:   ImportDialogue.cpp
//  AUTHOR: Ian Ross
//  DATE:   02-OCT-2005
//
//----------------------------------------------------------------------
//
//  Import dialogue.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <vector>
#include <cstdlib>

using namespace std;


// Unix headers.

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>


// Local headers.

#include "ImportDialogue.hh"
#include "InteractorList.hh"
#include "TextField.hh"
#include "Importer.hh"


//----------------------------------------------------------------------
//
//  CONSTANT DEFINITIONS
//
//----------------------------------------------------------------------

const int DIALOGUE_WIDTH = 70;
const int DIALOGUE_HEIGHT = 7;


//----------------------------------------------------------------------
//
//  MEMBER FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

ImportDialogue::ImportDialogue(DocMgr::Connection &db) :
  Widget((getmaxx(stdscr) - DIALOGUE_WIDTH) / 2,
         (getmaxy(stdscr) - DIALOGUE_HEIGHT) / 2,
         DIALOGUE_WIDTH, DIALOGUE_HEIGHT),
  _db(db)
{ }


bool ImportDialogue::process_key(int ch)
{
  bool retval = false;
  switch(ch) {
  case KEY_TAB:
  case KEY_DOWN:
    // Try to perform completion on the filename.
    if (_field->cursor_pos() == _file_name.size()) {
      complete_file(_file_name);
      _field->process_key(CTRL('E'));
    }
    retval = true;
    break;

  case KEY_ENTER:
  case CTRL('J'):
  case CTRL('M'):
  case META('i'):
    _completed = true;
    _import = true;
    break;

  case META('a'):
    _completed = true;
    _import = false;
    break;
  }
  return retval;
}


void ImportDialogue::run(void)
{
  clear();
  frame();
  string title = " Import ISI Records ";
  writestr((_w - title.size()) / 2, 0, title);
  draw_hline(_h - 3);
  mvwaddch(_win, _h - 3, 0, ACS_LTEE);
  mvwaddch(_win, _h - 3, _w - 1, ACS_RTEE);
  string menu_str = "Import   Abandon";
  writestr((_w - menu_str.size()) / 2, _h - 2, menu_str);
  writestr((_w - menu_str.size()) / 2, _h - 2, "I", A_BOLD);
  writestr((_w - menu_str.size()) / 2 + 9, _h - 2, "A", A_BOLD);

  curs_set(1);
  _file_name = "~/savedrecs.txt";
  writestr(3, 2, "Import file:");
  _field = new TextField(_x + 16, _y + 2, _w - 20, _file_name, "/");
  _field->focus(true);
  _field->display();
  doupdate();

  InteractorList interaction;
  interaction.push_back(*_field);
  interaction.push_back(*this);

  _completed = _import = false;
  while (!_completed) { interaction.process_key(); doupdate(); }
  curs_set(0);

  if (_import) {
    Importer importer(_db, _file_name);
    if (!importer.valid()) {
      // Something's broken.
      clear();
      frame();
      writestr((_w - title.size()) / 2, 0, title);
      string line1 = "IMPORT FAILED";
      writestr((_w - line1.size()) / 2, 2, line1, A_BOLD);
      string line2 = importer.reason();
      writestr((_w - line2.size()) / 2, 3, line2);
      string line3 = "Press any key to continue";
      writestr((_w - line3.size()) / 2, 5, line3);
      refresh();
      doupdate();
      InteractorList::get_char();
    } else {
      for (vector<DocMgr::DocRecord *>::iterator it = importer.docs().begin();
           it != importer.docs().end(); ++it)
        (*it)->intern();
    }
  }

  delete _field;
  _field = 0;
  clear();
}


void ImportDialogue::complete_file(string &head)
{
  string search_head = head;
  if (search_head[0] == '~') {
    string home = getenv("HOME");
    search_head.replace(0, 1, home);
  }

  string::size_type last_slash = search_head.rfind('/');
  string parent_dir_name, child_name_head;
  if (last_slash == string::npos) {
    parent_dir_name = ".";
    child_name_head = search_head;
  } else {
    parent_dir_name = search_head.substr(0, last_slash);
    child_name_head = search_head.substr(last_slash + 1);
  }

  DIR *parent_dir = opendir(parent_dir_name.c_str());
  if (parent_dir) {
    vector<string> head_matches;
    for (dirent *entry = readdir(parent_dir);
         entry; entry = readdir(parent_dir)) {
      string current = entry->d_name;
      if (child_name_head == current.substr(0, child_name_head.size()))
        head_matches.push_back(current.substr(child_name_head.size()));
    }
    closedir(parent_dir);

    if (head_matches.size() == 1) {
      head += head_matches[0];
      search_head += head_matches[0];
      struct stat stat_buff;
      if (0 == stat(search_head.c_str(), &stat_buff))
        if (S_ISDIR(stat_buff.st_mode)) head += '/';
    } else if (head_matches.size() > 1) {
      int common_prefix_len = 0;
      int max_common_prefix_len = head_matches[0].size();
      for (int idx = 1; idx < head_matches.size(); ++idx)
        if (head_matches[idx].size() < max_common_prefix_len)
          max_common_prefix_len = head_matches[idx].size();

      bool done = false;
      string first = head_matches[0];
      while (common_prefix_len < max_common_prefix_len && !done) {
        for (int idx = 1; idx < head_matches.size(); ++idx) {
          string curr = head_matches[idx];
          if (curr[common_prefix_len] != first[common_prefix_len]) {
            done = true;
            break;
          }
        }
        if (!done) ++common_prefix_len;
      }
      head += head_matches[0].substr(0, common_prefix_len);
    }
  }
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
