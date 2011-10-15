////////////////////////////////////////////////////////////////////////////////
/// @file         gnat.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains global const and enumeration values for the
///               GNAT program.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _GNAT_H_
#define _GNAT_H_

#include <QtGlobal>

///This const is used to indicate that a file is usable by this program.  The
///first 4 bytes of a file must match this number to be loaded into the GNAT
///program.
extern const quint32 SAVEFILE_MAGICNUM;

///This const is used to indicate the program version number for use throughout
///the GNAT program.
extern const float   PROGRAM_VERSIONNUM;

///This const is used to indicate the minimum allowed value for edges and nodes.
extern const float   MIN_FLOAT_VAL;

///This const is used to indicate the maximum allowed value for edges and nodes.
extern const float   MAX_FLOAT_VAL;

////////////////////////////////////////////////////////////////////////////////
/// PROGRAM_MODE
///
/// @description  This enumeration is used to determine which mode the program
///               is currently in.  It is also used when saving and loading
///               files to determine what type of file it is.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
enum PROGRAM_MODE
{
    MODE_UNDEFINED = 0, ///< This is the default mode.
    MODE_PUZZLE    = 1, ///< This is used to indicate the puzzle mode.
    MODE_TREE      = 2, ///< This is used to indicate the tree mode.
    MODE_CHANCE    = 3  ///< This is used to indicate the chance tree mode.
};

#endif /*_GNAT_H_*/
