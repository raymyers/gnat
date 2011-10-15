////////////////////////////////////////////////////////////////////////////////
/// @file         SPuzzleTrace.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleTrace class
///               and the classes it uses.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLETRACE_H_
#define _CPUZZLETRACE_H_

#include <QList>
#include <QString>
#include <QStringList>
#include <QSize>

struct SPuzzleCall;
struct IPuzzleLine;

////////////////////////////////////////////////////////////////////////////////
/// STreeTrace
///
/// @description  The STreeTrace struct is the data structure used to represent
///               the trace of a tree. While traces are usually displayed in a
///               two dimensional grid that is now how the data is stored. The
///               grid is a representation of a depth first search of a tree of
///               calls, and thus the data for the trace is stored in just such
///               a tree.
///               The STreeTrace has attributes to store the data created from a
///               trace. It stores the name of the tracer as well as its column
///               headings. The other attribute is a list of calls that
///               represent the different depths used in an Iterative-Deepening
///               (ID) trace. If the trace is not ID then there will only be one
///               entry in the list.
///
/// @limitations  This is a struct. It is meant as a way of combining data, but
///               it provides no encapsulation or data hiding. This has definite
///               speed benefits but sacrifices most forms of error checking.
///
////////////////////////////////////////////////////////////////////////////////
struct SPuzzleTrace
{
    SPuzzleTrace();
    ~SPuzzleTrace();
    
    //This variable is the name of the tracer algorithm that created this trace.
    QString             s_aiName;

    //This variable is a list of the names or headers for each column. It is
    //often used to get a count() of how many columns there are in the trace.
    QStringList         s_columnNames;

    //This variable represent the different depths used in an
    //Iterative-Deepening(ID) trace. If the trace is not ID then there will only
    //be one entry in the list.
    QList<SPuzzleCall *>  s_depths;
};

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleCall
///
/// @description  The SPuzzleCall class represents a call in the algorithm. The
///               calls themselves form a tree, and the grid that is a displayed
///               trace is a depth first search representation of this grid.
///               The STreeCall has two lists. The first list are it's children
///               in the tree of calls. The second list is the lines that makup
///               the call. It also holds a float representation of the return
///               value from the call.
///
/// @limitations  This is a struct. It is meant as a way of combining data, but
///               it provides no encapsulation or data hiding. This has definite
///               speed benefits but sacrifices most forms of error checking.
///
////////////////////////////////////////////////////////////////////////////////
struct SPuzzleCall
{
    SPuzzleCall();
    ~SPuzzleCall();

    //QList<SPuzzleCall *>  s_children;

    QList<IPuzzleLine *>  s_lines;
    float               s_returnValue;
    
    IPuzzleLine *operator[](int index);
    QString s_prelude; // Message that appears in the trace just before the call.
                       // example: "depth-limit=1"
                       // Contents will be emph'd in the LaTeX output.

    QString s_comment; // TODO: Change this to s_aftermath or perhaps s_postscript.
};

////////////////////////////////////////////////////////////////////////////////
/// IPuzzleLine
///
/// @description  The ITreeLine represents an actual line in a trace grid. The
///               different cells are returned as QStrings through the []
///               operator or they can be extracted as a list by the
///               toStringList function.
///
/// @limitations  This is an Interface style structure. It is not meant to be,
///               and can not be instantiated. All subclasses must have the []
///               operator and toStringList overridden.
///
////////////////////////////////////////////////////////////////////////////////
struct IPuzzleLine
{
    virtual ~IPuzzleLine();
    virtual QString operator[](int index) = 0;
    virtual QStringList toStringList() = 0;


};

#endif /*_STREETRACE_H_*/
