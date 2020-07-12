/*
    tools.c
*/

#include <userinterface.h>
#include <fonttexts.h>
#include <_stdio.h>
#include "tools.h"
#include "files.h"

void tools_initialise ()
{
    mchar* fileContent;
    mchar buffer[BUFFER_SIZE];
    mchar filePathName[100];

    strcpy0 (filePathName, "Resources/Messages.txt");

    if(!Openfile (filePathName, buffer))
        fileContent = NULL;
    else fileContent = buffer;

    if(!fonttexts_load_messages (fileContent, ENGLISH))
    {
        strcpy0 (buffer+500, "Inside file %s:\n\n%s\n\nDefault settings used.\n");
        sprintf1(buffer, buffer+500, filePathName, error_message);
        display_error_message (buffer); return;
    }

    strcpy0 (filePathName, "Resources/Operations.txt");

    if(!Openfile (filePathName, buffer))
        fileContent = NULL;
    else fileContent = buffer;

    if(!fonttexts_load_operations (fileContent, ENGLISH))
    {
        strcpy0 (buffer+500, "Inside file %s:\n\n%s\n\nDefault settings used.\n");
        sprintf1(buffer, buffer+500, filePathName, error_message);
        display_error_message (buffer); return;
    }

    display_error_message (NULL);
}

void tools_pause_resume()
{
}
