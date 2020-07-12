/*
    files.h

    Handle file related issues.
*/
#ifndef _FILES_H
#define _FILES_H

#include "user_main_gui.h"


extern int Openfile (const wchar* file_name, wchar* file_content);


extern int save_file ();           // save entry text field in currently opened file

extern int save_file_as ();        // save entry text field in a file

extern int save_all_objects ();    // save all graphs and cameras in a file

extern int load_all_objects (const wchar* filename); // load all graphs and cameras from a file

extern int noObjects;                       // is TRUE iff remove_all_objects(hWnd) is called


extern int open_file ();           // open a file and load into entry text field

extern int load_file (const wchar* filename); // load a file into entry text field

extern int reload_file ();         // reload the currently opened file


extern int new_file ();            // start a new, untitled and empty file entry

extern int check_save_changes ();  // notify user if entry text field has been modified


extern int get_name_from_path_name (wchar* file_name, const wchar* file_path_name);


#endif
