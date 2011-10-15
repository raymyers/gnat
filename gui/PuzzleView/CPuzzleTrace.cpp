////////////////////////////////////////////////////////////////////////////////
/// @file         SPuzzleTrace.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CTreeGraph class,
///               which stores the actual data of a tree, and the structure
///               which represents the tree nodes.
///
////////////////////////////////////////////////////////////////////////////////
#include "CPuzzleTrace.h"

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleTrace::SPuzzleTrace
///
/// @description  This is the default struct constructor which initializes class
///               member variables.
/// @pre          None
/// @post         None
///
/// @limitations  It actually does nothing...
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleTrace::SPuzzleTrace()
{
}

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleTrace::~SPuzzleTrace
///
/// @description     This is the destructor which frees up dynamic memory used
///                  by the class. It deletes all the STreeCalls stored in
///                  s_depths.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleTrace::~SPuzzleTrace()
{
    while( s_depths.count() > 0 )
    {
        if( s_depths.first() == NULL )
            s_depths.takeFirst();
        else
            delete s_depths.takeFirst();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleCall::SPuzzleCall
///
/// @description  This is the default class constructor which initializes class
///               member variables.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleCall::SPuzzleCall()
{
    s_returnValue = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleCall::~SPuzzleCall
///
/// @description     This is the destructor which frees up dynamic memory used
///                  by the class. It deletes all the STreeCalls stored in
///                  s_children and all the ITreeLines stored in s_lines.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleCall::~SPuzzleCall()
{
    /*
    while( s_children.count() > 0 )
    {
        if( s_children.first() == NULL )
            s_children.takeFirst();
        else
            delete s_children.takeFirst();
    }
    */

    while( s_lines.count() > 0 )
    {
        if( s_lines.first() == NULL )
            s_lines.takeFirst();
        else
            delete s_lines.takeFirst();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleCall::operator[]
///
/// @description     This overloaded operator is meant to allow an abstraction
///                  of the STreeCall. Using the [][] its cells can be accessed
///                  as though it were a two dimensional array of QStrings.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
IPuzzleLine *SPuzzleCall::operator[](int index)
{
    if( index >= 0 && index < s_lines.count() )
        return s_lines[index];
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
/// IPuzzleLine::~IPuzzleLine
///
/// @description     This is the destructor which frees up dynamic memory used
///                  by the class.
/// @pre             None
/// @post            None
///
/// @limitations     It actually does nothing...
///
////////////////////////////////////////////////////////////////////////////////
IPuzzleLine::~IPuzzleLine()
{
}
