////////////////////////////////////////////////////////////////////////////////
/// @file         gnat.cpp
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file is the contains the main function which begins
///               execution of the GNAT program.
///
////////////////////////////////////////////////////////////////////////////////

#include <QApplication>
#include "gnat.h"
#include "gui/CGNATWindow.h"

const quint32 SAVEFILE_MAGICNUM   = 0xA7B4ED63;
const float   PROGRAM_VERSIONNUM  = 0.30F;
const float   MIN_FLOAT_VAL       = -1000.0F;
const float   MAX_FLOAT_VAL       = 1000.0F;

////////////////////////////////////////////////////////////////////////////////
/// main
///
/// @description  This is the main function which begins and ends execution
///               of the GNAT program.
/// @pre          None
/// @post         The program is terminating.
///
/// @param argc:  This variable is used to indicate how many arguments were
///               supplied when the program was executed.
/// @param argv:  This variable is an array of char arrays which contain the
///               arguments supplied when the program was executed.
///
/// @return int:  This value is used by the operating system to determine if
///               the application was closed properly or not.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CGNATWindow w;
    w.show();
    return app.exec();
}
