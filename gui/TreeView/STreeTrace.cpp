////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeGraph.cpp
///
/// @author       Bob Buehler and Matt Dissinger
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
#include "STreeTrace.h"

////////////////////////////////////////////////////////////////////////////////
/// STreeTrace::STreeTrace
///
/// @description  This is the default struct constructor which initializes class
///               member variables.
/// @pre          None
/// @post         None
///
/// @limitations  It actually does nothing...
///
////////////////////////////////////////////////////////////////////////////////
STreeTrace::STreeTrace()
{
}

////////////////////////////////////////////////////////////////////////////////
/// STreeTrace::~STreeTrace
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
STreeTrace::~STreeTrace()
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
/// STreeCall::STreeCall
///
/// @description  This is the default class constructor which initializes class
///               member variables.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
STreeCall::STreeCall()
{
    s_returnValue = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// STreeCall::~STreeCall
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
STreeCall::~STreeCall()
{
    while( s_children.count() > 0 )
    {
        if( s_children.first() == NULL )
            s_children.takeFirst();
        else
            delete s_children.takeFirst();
    }

    while( s_lines.count() > 0 )
    {
        if( s_lines.first() == NULL )
            s_lines.takeFirst();
        else
            delete s_lines.takeFirst();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// STreeCall::operator[]
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
SGeneralMLine *STreeCall::operator[](int index)
{
    if( index >= 0 && index < s_lines.count() )
        return s_lines[index];
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
/// ITreeLine::~ITreeLine
///
/// @description     This is the destructor which frees up dynamic memory used
///                  by the class.
/// @pre             None
/// @post            None
///
/// @limitations     It actually does nothing...
///
////////////////////////////////////////////////////////////////////////////////
ITreeLine::~ITreeLine()
{
}
