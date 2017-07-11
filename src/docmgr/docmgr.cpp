//----------------------------------------------------------------------
//
//  FILE:   docmgr.cpp
//  AUTHOR: Ian Ross
//  DATE:   24-SEP-2005
//
//----------------------------------------------------------------------
//
//  Main program for document manager.
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  HEADER FILES
//
//----------------------------------------------------------------------

// Standard headers.

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <assert.h>

using namespace std;


// Unix headers.

#include <signal.h>
#include <curses.h>
#include <unistd.h>


// Local headers.

#include "DocMgr.hh"
#include "InteractorList.hh"
#include "Menu.hh"
#include "TopLine.hh"
#include "IDList.hh"
#include "ArticleViewForm.hh"
#include "ArticleEditForm.hh"
#include "QuickSearchDialogue.hh"
#include "ArticleTypeDialogue.hh"
#include "OptionsDialogue.hh"
#include "ImportDialogue.hh"
#include "ExportDialogue.hh"
#include "ConfirmDialogue.hh"
#include "Configuration.hh"
#include "Filter.hh"
#include "FilterList.hh"
#include "JumpToDocDialogue.hh"

using namespace DocMgr;


//----------------------------------------------------------------------
//
//  CONSTANT DEFINITIONS
//
//----------------------------------------------------------------------

const int ADD_ACTION      = 1;
const int EDIT_ACTION     = 2;
const int DELETE_ACTION   = 3;
const int PURGE_ACTION    = 4;
const int VIEW_ACTION     = 5;
const int SEARCH_ACTION   = 6;
const int FILTERS_ACTION  = 7;
const int UNFILTER_ACTION = 8;
const int IMPORT_ACTION   = 9;
const int OPTIONS_ACTION  = 10;
const int EXIT_ACTION     = 11;
const int SELECT_ACTION   = 12;
const int COMBINE_ACTION  = 13;
const int JUMP_ACTION     = 14;
const int EXPORT_ACTION   = 15;


//----------------------------------------------------------------------
//
//  GLOBAL VARIABLES
//
//----------------------------------------------------------------------

// Configuration details.
Configuration *config = 0;

// Filter list.
FilterList *filter_list = 0;


//----------------------------------------------------------------------
//
//  LOCAL FUNCTION PROTOTYPES
//
//----------------------------------------------------------------------

static bool run_db_good_screen(DocMgr::Connection *conn);
static Filter *run_filter_screen(DocMgr::Connection *conn, bool db_edits);
static bool run_db_failed_screen(string failure_msg);


//----------------------------------------------------------------------
//
//  MAIN PROGRAM
//
//----------------------------------------------------------------------

int main(void)
{
  try {
    // Read configuration file.
    config = new Configuration("~/.docmgr2/docmgrrc");

    bool done = false;
    while (!done) {
      // Set up Curses.
      signal(SIGINT, SIG_IGN);
      initscr();
      if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(COLOR_RED, COLOR_RED, -1);
      }
      keypad(stdscr, TRUE);
      nonl();
      cbreak();
      noecho();

      // Modifying the database connection parameters necessitates
      // reconnection.  This is most easily handled by just blowing
      // everything away and starting over.

      // Connect to database.
      string failure_msg;
      Connection *conn = 0;
      try {
        conn = new Connection("~/.docmgr2/docmgr.db");
      } catch (DocMgr::Exception &exc) {
        if (exc.type() != DocMgr::Exception::DB_ERROR) throw;
        failure_msg = exc.msg();
      }

      if (!conn)
        done = !run_db_failed_screen(failure_msg);
      else {
        done = !run_db_good_screen(conn);
        delete conn;
      }

      endwin();
    }
  }
  catch (Exception &exc) {
    endwin();
    cout << "UNHANDLED DocMgr EXCEPTION: " << exc.msg() << endl;
  }
}


//----------------------------------------------------------------------
//
//  LOCAL FUNCTION DEFINITIONS
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
//  run_db_failed_screen
//
// Run limited interaction scheme for cases where connection to the
// database has failed (basically just allows user to try to connect
// to another database, or to quit from the program).
//

static bool run_db_failed_screen(string failure_msg)
{
  bool resized, quit = false;
  do {
    resized = false;
    try {
      // If the database connection failed, just allow the user to
      // modify the connections options or exit.
      erase();

      // Create bottom-line menu.
      Menu default_menu(0, getmaxy(stdscr) - 1, getmaxx(stdscr));
      default_menu.add_item("&Options",  OPTIONS_ACTION);
      default_menu.add_item("&Exit",     EXIT_ACTION);

      // Global options dialogue.
      OptionsDialogue options_dialogue(config);

      // Basic interaction: all events to menu.
      InteractorList responders;
      responders.clear();
      responders.push_back(default_menu);
      curs_set(0);

      while (!quit) {
        string errmsg = "DATABASE CONNECTION FAILED: "
          "TRY MODIFYING CONNECTION OPTIONS";
        mvaddstr(8, (getmaxx(stdscr) - errmsg.size()) / 2, errmsg.c_str());
        int row = 10;
        int contents_space = getmaxx(stdscr) - 20;
        int min_break_pos = contents_space * 3 / 4;
        string msg = failure_msg;
        while (msg.size() > contents_space) {
          int line_len = contents_space, cut_pos = contents_space;
          if (msg[contents_space] == ' ') {
            line_len = contents_space;
            cut_pos = contents_space + 1;
          } else
            for (int idx = contents_space - 1; idx > min_break_pos; --idx)
              if (msg[idx] == ' ') {
                line_len = idx;
                cut_pos = idx + 1;
                break;
              }

          string line = msg.substr(0, line_len);
          msg = msg.substr(cut_pos);
          mvaddstr(row, (getmaxx(stdscr) - line.size()) / 2, line.c_str());
          ++row;
        }
        mvaddstr(row, (getmaxx(stdscr) - msg.size()) / 2, msg.c_str());
        refresh();
        default_menu.display();
        doupdate();
        responders.process_key();

        if (default_menu.activated()) {
          switch (default_menu.action()) {
          case OPTIONS_ACTION:
            // Run the options dialogue.
            options_dialogue.run();
            break;

          case EXIT_ACTION:
            quit = true;
            break;

          default:
            break;
          }
        }
      }
    } catch (InteractorList::Resize &resize) {
      resized = true;
      endwin();
      initscr();
      if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(COLOR_RED, COLOR_RED, -1);
      }
      keypad(stdscr, TRUE);
      nonl();
      cbreak();
      noecho();
    }
  } while (resized);
}


//----------------------------------------------------------------------
//
//  run_db_good_screen
//
// Run main interaction scheme.
//

static bool run_db_good_screen(DocMgr::Connection *conn)
{
  bool retval = true;
  bool resized;
  DocMgr::DocID resize_id;
  IDList *id_list = 0;
  do {
    resized = false;
    try {
      bool quick_search_active = false;
      bool db_edits = false;

      // Database connection is good.
      conn->set_view_deleted(config->show_deleted());

      // Build main screen (default configuration has article view:
      // this changes when edit or new article modes are entered).
      ArticleViewForm view_form(conn, 9, 1, -10, -2);
      id_list = new IDList(conn, view_form, 1, 1, 8, -2);
      id_list->display();
      view_form.display();

      // Create default bottom-line menu.
      Menu default_menu(0, getmaxy(stdscr) - 1, getmaxx(stdscr));
      default_menu.add_item("&Add",      ADD_ACTION);
      default_menu.add_item("&Edit",     EDIT_ACTION);
      default_menu.add_item("&Delete",   DELETE_ACTION);
      default_menu.add_item("&Purge",    PURGE_ACTION);
      default_menu.add_item("Vie&w",     VIEW_ACTION);
      default_menu.add_item("&Jump",     JUMP_ACTION);
      default_menu.add_item("&Search",   SEARCH_ACTION);
      default_menu.add_item("&Filter",   FILTERS_ACTION);
      default_menu.add_item("&Unfilter", UNFILTER_ACTION);
      default_menu.add_item("&Import",   IMPORT_ACTION);
      default_menu.add_item("Expo&rt",   EXPORT_ACTION);
      default_menu.add_item("&Options",  OPTIONS_ACTION);
      default_menu.add_item("E&xit",     EXIT_ACTION);
      default_menu.display();

      // Display top-line info.
      TopLine top_line;
      top_line.display();
      top_line.set_entry_count(id_list->entries());

      // Quicksearch dialogue.
      QuickSearchDialogue quick_search;
      string search_string = "";

      // Current filter.
      Filter *filter = 0;

      // Jump To Document dialogue.
      JumpToDocDialogue jump;

      // Dialogues.
      ArticleTypeDialogue article_type(*conn);
      OptionsDialogue options_dialogue(config);
      ImportDialogue import_dialogue(*conn);
      ExportDialogue export_dialogue(*conn);
      ConfirmDialogue confirm_dialogue;

      // Basic interaction in main screen - key events go to the ID
      // list, then to the menu if unprocessed.
      InteractorList responders;
      responders.clear();
      responders.push_back(*id_list);
      responders.push_back(default_menu);
      responders.push_back(view_form);
      curs_set(0);

      if (resize_id.valid()) {
        id_list->set_current(resize_id);
        resize_id = DocMgr::DocID();
      }

      bool db_changed = false;
      for (;;) {
        doupdate();
        responders.process_key();

        if (default_menu.activated()) {
          switch (default_menu.action()) {
          case ADD_ACTION: {
            // Add a new entry: done in two steps - first, select an
            // article type, then run the article edit form.
            DocMgr::DocType doc_type;
            if (article_type.run(doc_type)) {
              view_form.hide();
              ArticleEditForm edit_form(conn, 9, 1, -10, -2);
              DocMgr::DocID new_id = edit_form.add_entry(doc_type);
              if (new_id.valid()) {
                id_list->set_current(new_id);
                top_line.set_entry_count(id_list->entries());
                db_edits = true;
              }
            }
            id_list->display();
            view_form.display();
            default_menu.display();
            top_line.display();
            break;
          }

          case EDIT_ACTION: {
            // Edit an existing entry.
            view_form.hide();
            ArticleEditForm edit_form(conn, 9, 1, -10, -2);
            edit_form.edit(id_list->current());
            view_form.refresh();
            view_form.display();
            default_menu.display();
            top_line.display();
            db_edits = true;
            break;
          }

          case DELETE_ACTION:
            // Toggle the deleted status of an entry.
            id_list->toggle_deleted(id_list->current());
            db_edits = true;
            id_list->refresh();
            view_form.refresh();
            id_list->display();
            view_form.display();
            break;

          case PURGE_ACTION:
            // Purge deleted entries from the database.
            if (confirm_dialogue.run
                ("BTHIS IS AN IRREVERSIBLE ACTION!\n"
                 "NAre you sure you want to purge ALL\n"
                 "Ndeleted records from the database?")) {
              conn->purge_deleted();
              id_list->refresh();
              view_form.set_current(id_list->current());
              view_form.refresh();
            }
            db_edits = true;
            id_list->display();
            view_form.display();
            break;

          case VIEW_ACTION:
            // Display the PDF of a document.
            if (id_list->current().valid()) {
              string id = id_list->current();
              int id_num = id_list->current();
              int range_base = (id_num / 100) * 100;
              char fname_buff[1024];
              string paper_dir = config->paper_directory();
              if (paper_dir[0] == '~') {
                string home = getenv("HOME");
                paper_dir = home + paper_dir.substr(1);
              }
              sprintf(fname_buff, "%s/%06d-%06d/%s.pdf",
                      paper_dir.c_str(), range_base, range_base + 99, id.c_str());
              char cmd_buff[1024];
              sprintf(cmd_buff, config->view_command().c_str(), fname_buff);
              string cmd_str = string(cmd_buff) + " > /dev/null 2>&1 &";
              system(cmd_str.c_str());
            }
            break;

          case SEARCH_ACTION: {
            // Run quick search dialogue and search on the basis of
            // the result.  If the result is an empty string, unfilter
            // the results.
            quick_search.run(search_string);
            if (search_string.size() > 0) {
              if (filter != 0) {
                DocMgr::Query search_query =
                  filter->query() && DocMgr::Query(*conn, search_string);
                id_list->set_query(search_query);
                top_line.set_filter_name
                  (filter->name() + string(" [QS (") + search_string + ")]");
                top_line.set_entry_count(id_list->entries());
                quick_search_active = true;
              } else {
                DocMgr::Query search_query(*conn, search_string);
                id_list->set_query(search_query);
                top_line.set_filter_name(string("QS (") + search_string + ")");
                top_line.set_entry_count(id_list->entries());
                quick_search_active = true;
              }
            } else {
              id_list->clear_query();
              top_line.set_filter_name("n/a");
              top_line.set_entry_count(id_list->entries());
              filter = 0;
            }
            break;
          }

          case JUMP_ACTION: {
            // Jump to a specified document number.
            if (jump.run(*conn, id_list)) {
              top_line.set_filter_name("n/a");
              top_line.set_entry_count(id_list->entries());
            }
            id_list->display();
            view_form.display();
            default_menu.display();
            top_line.display();
            break;
          }

          case FILTERS_ACTION: {
            // Run filter screen.
            filter = run_filter_screen(conn, db_edits);
            db_edits = false;
            if (filter) {
              DocMgr::Query search_query(filter->query());
              id_list->set_query(search_query);
              top_line.set_filter_name(filter->name());
              top_line.set_entry_count(id_list->entries());
              quick_search_active = false;
            } else if (!quick_search_active) {
              id_list->clear_query();
              top_line.set_filter_name("n/a");
              top_line.set_entry_count(id_list->entries());
            }
            id_list->display();
            view_form.display();
            default_menu.display();
            top_line.display();
            break;
          }

          case UNFILTER_ACTION:
            // Switch off filtering of articles.
            id_list->clear_query();
            top_line.set_filter_name("n/a");
            top_line.set_entry_count(id_list->entries());
            quick_search_active = false;
            filter = 0;
            break;

          case IMPORT_ACTION:
            // Run the import dialogue.
            import_dialogue.run(config->default_import_file());
            id_list->refresh();
            id_list->display();
            view_form.display();
            default_menu.display();
            top_line.display();
            db_edits = true;
            break;

          case EXPORT_ACTION:
            // Run the export dialogue.
            export_dialogue.run(id_list->current());
            id_list->refresh();
            id_list->display();
            view_form.display();
            default_menu.display();
            top_line.display();
            db_edits = true;
            break;

          case OPTIONS_ACTION:
            // Run the options dialogue.
            options_dialogue.run();
            conn->set_view_deleted(config->show_deleted());
            id_list->refresh();
            id_list->display();
            view_form.display();
            default_menu.display();
            top_line.display();
            break;

          default:
            break;
          }

          // Handle database reconnection condition.
          if (db_changed) break;

          // Handle exit condition.
          if (default_menu.action() == EXIT_ACTION) {
            retval = false;
            break;
          }
        }
      }
    } catch (InteractorList::Resize &resize) {
      resized = true;
      resize_id = id_list->current();
      delete filter_list;
      filter_list = 0;
      endwin();
      initscr();
      if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(COLOR_RED, COLOR_RED, -1);
      }
      keypad(stdscr, TRUE);
      nonl();
      cbreak();
      noecho();
    }
    delete id_list;
  } while (resized);

  return retval;
}


//----------------------------------------------------------------------
//
//  run_filter_screen
//
// Filter editing and selection screen.
//

static Filter *run_filter_screen(DocMgr::Connection *conn, bool db_edits)
{
  bool resized;
  do {
    resized = false;
    try {
      try {
        Filter *retval = 0;

        // Build the filter list.
        static DocMgr::Connection *old_conn = 0;
        if (conn != old_conn || !filter_list) {
          delete filter_list;
          filter_list = new FilterList(conn, config, 1, 1, -2, -2);
          old_conn = conn;
        } else if (db_edits)
          filter_list->refresh();
        filter_list->display();

        // Create bottom-line menu.
        Menu menu(0, getmaxy(stdscr) - 1, getmaxx(stdscr));
        menu.add_item("&Add",                   ADD_ACTION);
        menu.add_item("&Edit",                  EDIT_ACTION);
        menu.add_item("&Delete",                DELETE_ACTION);
        menu.add_item("&Select",                SELECT_ACTION);
        menu.add_item("&Combine",               COMBINE_ACTION);
        menu.add_item("&Back to main screen",   EXIT_ACTION);
        menu.display();

        // Dialogues.
        ConfirmDialogue confirm_dialogue(false);

        // Basic interaction in filter screen - key events go to the
        // filter list, then to the menu if unprocessed.
        InteractorList responders;
        responders.clear();
        responders.push_back(*filter_list);
        responders.push_back(menu);
        curs_set(0);

        for (;;) {
          doupdate();
          responders.process_key();

          if (menu.activated()) {
            switch (menu.action()) {
            case ADD_ACTION:
              // Add a new filter.
              filter_list->add_filter();
              menu.display();
              break;

            case EDIT_ACTION:
              // Edit an existing filter.
              filter_list->edit_current();
              menu.display();
              break;

            case DELETE_ACTION:
              // Toggle the deleted status of an entry.
              if (filter_list->current_filter() &&
                  confirm_dialogue.run
                  ("NAre you sure you want to delete this filter?"))
                filter_list->delete_current();
              else
                filter_list->display();
              break;

            case COMBINE_ACTION:
              // Handle combining existing filters.
              filter_list->combine_filters();
              break;

            default:
              break;
            }

            // Handle exit conditions.
            if (menu.action() == SELECT_ACTION) {
              if (filter_list->current_filter()) {
                retval = filter_list->current_filter();
                break;
              }
            }
            if (menu.action() == EXIT_ACTION) break;
          }
        }

        menu.hide();
        filter_list->hide();
        return retval;
      } catch (DocMgr::Exception &exc) {
        if (exc.type() == DocMgr::Exception::MISC &&
            exc.msg() == "Invalid filter definition!")
          throw DocMgr::Exception
            (DocMgr::Exception::MISC,
             "UH-OH: Invalid filter definition in .docmgr!");
        else
          throw;
      }
    } catch (InteractorList::Resize &resize) {
      resized = true;
      delete filter_list;
      filter_list = 0;
      endwin();
      initscr();
      if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(COLOR_RED, COLOR_RED, -1);
      }
      keypad(stdscr, TRUE);
      nonl();
      cbreak();
      noecho();
    }
  } while (resized);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
