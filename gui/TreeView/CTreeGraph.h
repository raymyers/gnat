////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeGraph.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CTreeGraph class,
///               which stores the actual data of a tree, and the structure
///               which represents the tree nodes.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CTREEGRAPH_H_
#define _CTREEGRAPH_H_

#include <QString>
#include <QList>
#include <QPoint>
#include <QDataStream>

struct STreeNode;

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraph
///
/// @description  The CTreeGraph class is a container for the nodes in a tree
///               graph.  It provides complete and unsecured access to its
///               attributes.
///               The CTreeGraph also encapsulates its own file I/O for both
///               saving and loading.  For image output of the tree layout see
///               the CTreeGraphRenderer.
///
/// @limitations  Direct use of this class can be very dangerous.  It is
///               recommended that this class be placed inside a
///               CTreeGraphModel before interacting with it.
///
////////////////////////////////////////////////////////////////////////////////
class CTreeGraph
{
public:
    CTreeGraph();
    ~CTreeGraph();

    QString getFileName();
    STreeNode * getRootNode() const;

    void load( const QString &fileName = QString() );
    void loadHelper( STreeNode *node, QDataStream &stream );

    void save( const QString &fileName = QString() );
    void saveHelper( const STreeNode *node, QDataStream &stream );

    void setFileName( const QString &fileName );
    void setRootNode(STreeNode *node);

private:
    //This variable is used to store a pointer to the root of the tree graph.
    STreeNode *m_rootNode;

    //This variable is used to store the current file name.
    QString    m_fileName;
};

////////////////////////////////////////////////////////////////////////////////
/// STreeNode
///
/// @description  The STreeNode struct publicly contains all the information
///               needed to define a node in a tree graph.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
struct STreeNode
{
    STreeNode();
    ~STreeNode();

    ///This variable stores the name of the node.
    QString                 s_name;

    ///This variable stores the quiescent state of the node.
    bool                    s_quiescent;

    ///This variable stores the state value of the node.
    qreal                   s_value;
    
    ///This variable stores the preset history table value
    int                     s_history;

    ///This variable stores a pointer to the parent node of the node.
    STreeNode              *s_parent;

    ///This variable stores the location on the screen of the node.
    QPoint                  s_position;

    ///This variable holds a list of the child nodes of the node, sorted
    ///from left to right.
    QList<STreeNode *>      s_childList;
};

#endif /*_CTREEGRAPH_H_*/
