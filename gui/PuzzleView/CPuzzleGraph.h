////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleGraph.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleGraph class,
///               which stores the actual data of a puzzle, and the structures
///               which represent the puzzle nodes and spans.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLE_H_
#define _CPUZZLE_H_

#include <QString>
#include <QList>
#include <QPoint>
#include <QDataStream>

struct SPuzzleNode;
struct SPuzzleSpan;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph
///
/// @description  The CPuzzleGraph class is a container for the Nodes and Spans
///               in a puzzle graph. It provides a complete and unsecured access
///               to its attributes.
///               The CPuzzleGraph also encapsulates its own file I/O for both
///               saving and loading. For image output of the graph layout see
///               the CPuzzleGraphRenderer.
///
/// @limitations  Direct use of this class can be very dangerous. For speed
///               optimization the two lists have been made accesible by
///               reference. Since they are both pointer lists, there exists
///               both the danger of deleting them without removing them from
///               the list and vice versa. It is recommended that this class be
///               placed inside a CPuzzleGraphModel before interacting with it.
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleGraph
{
public:
    CPuzzleGraph();
    ~CPuzzleGraph();

    QString getFileName();
    QList<SPuzzleNode *> &getNodeList();
    QList<SPuzzleSpan *> &getSpanList();
    SPuzzleNode * getStartNode() const;

    void load( const QString &fileName = QString() );
    void save( const QString &fileName = QString() );

    void setFileName( const QString &fileName );
    void setStartNode(SPuzzleNode *node);

private:
    QList<SPuzzleNode *> m_nodeList;
    QList<SPuzzleSpan *> m_spanList;

    SPuzzleNode *m_startNode;

    QString m_fileName;
};

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleNode
///
/// @description  The SPuzzleNode struct publicly contains all the information
///               needed to define a node in a puzzle graph.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
struct SPuzzleNode
{
    SPuzzleNode();
    ~SPuzzleNode();

    enum NodeFlag
    {
        MidNode     = 0x0000,   //0000
        StartNode   = 0x0001,   //0001
        EndNode     = 0x0002    //0010
    };

    QString                 s_name;
    qreal                   s_heuristic;
    QPoint                  s_position;
    NodeFlag                s_flag;
    QList<SPuzzleSpan *>    s_spanList;
};

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleSpan
///
/// @description  The SPuzzleSpan struct publicly contains all the information
///               needed to define all possible link combinations between two
///               nodes in a puzzle graph.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
struct SPuzzleSpan
{
    SPuzzleSpan();
    SPuzzleSpan( SPuzzleNode* node1, SPuzzleNode* node2 );
    ~SPuzzleSpan();

    enum SpanFlag
    {
        EmptySpan       = 0x0000,                   //0000
        From1           = 0x0001,                   //0001
        From2           = 0x0002,                   //0010
        To1             = From2,                    //0010
        To2             = From1,                    //0001
        Bidirectional   = 0x0004 | From1 | From2,   //0111
        Split           = From1 | From2             //0011
    };

    SPuzzleNode    *s_node1;
    SPuzzleNode    *s_node2;
    qreal           s_weightFrom1;
    qreal           s_weightFrom2;
    SpanFlag        s_flag;
};

#endif /*_CPUZZLE_H_*/
