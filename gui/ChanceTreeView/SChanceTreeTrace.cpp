////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeGraph.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CChanceTreeGraph class,
///               which stores the actual data of a tree, and the structure
///               which represents the tree nodes.
///
////////////////////////////////////////////////////////////////////////////////
#include "SChanceTreeTrace.h"

////////////////////////////////////////////////////////////////////////////////
/// SChanceTreeTrace::SChanceTreeTrace
///
/// @description  This is the default struct constructor which initializes class
///               member variables.
/// @pre          None
/// @post         None
///
/// @limitations  It actually does nothing...
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeTrace::SChanceTreeTrace()
{
}

////////////////////////////////////////////////////////////////////////////////
/// SChanceTreeTrace::~SChanceTreeTrace
///
/// @description     This is the destructor which frees up dynamic memory used
///                  by the class. It deletes all the SChanceTreeCalls stored in
///                  s_depths.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeTrace::~SChanceTreeTrace()
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
/// SChanceTreeCall::SChanceTreeCall
///
/// @description  This is the default class constructor which initializes class
///               member variables.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeCall::SChanceTreeCall()
{
    s_returnValue = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// SChanceTreeCall::~SChanceTreeCall
///
/// @description     This is the destructor which frees up dynamic memory used
///                  by the class. It deletes all the SChanceTreeCalls stored in
///                  s_children and all the IChanceTreeLines stored in s_lines.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeCall::~SChanceTreeCall()
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
/// SChanceTreeCall::operator[]
///
/// @description     This overloaded operator is meant to allow an abstraction
///                  of the SChanceTreeCall. Using the [][] its cells can be accessed
///                  as though it were a two dimensional array of QStrings.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
SExpectMLine *SChanceTreeCall::operator[](int index)
{
    if( index >= 0 && index < s_lines.count() )
        return s_lines[index];
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
/// IChanceTreeLine::~IChanceTreeLine
///
/// @description     This is the destructor which frees up dynamic memory used
///                  by the class.
/// @pre             None
/// @post            None
///
/// @limitations     It actually does nothing...
///
////////////////////////////////////////////////////////////////////////////////
IChanceTreeLine::~IChanceTreeLine()
{
}
