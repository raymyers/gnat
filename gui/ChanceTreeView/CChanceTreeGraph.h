////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeGraph.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CChanceTreeGraph class,
///               which stores the actual data of a tree, and the structure
///               which represents the tree nodes.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CCHANCETREEGRAPH_H_
#define _CCHANCETREEGRAPH_H_

#include <QString>
#include <QList>
#include <QPoint>
#include <QDataStream>

struct SChanceTreeNode;

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph
///
/// @description  The CChanceTreeGraph class is a container for the nodes in a tree
///               graph.  It provides complete and unsecured access to its
///               attributes.
///               The CChanceTreeGraph also encapsulates its own file I/O for both
///               saving and loading.  For image output of the tree layout see
///               the CChanceTreeGraphRenderer.
///
/// @limitations  Direct use of this class can be very dangerous.  It is
///               recommended that this class be placed inside a
///               CChanceTreeGraphModel before interacting with it.
///
////////////////////////////////////////////////////////////////////////////////
class CChanceTreeGraph
{
public:
    CChanceTreeGraph();
    ~CChanceTreeGraph();

    QString getFileName();
    SChanceTreeNode * getRootNode() const;

    void load( const QString &fileName = QString() );
    void loadHelper( SChanceTreeNode *node, QDataStream &stream );

    void save( const QString &fileName = QString() );
    void saveHelper( const SChanceTreeNode *node, QDataStream &stream );

    void setFileName( const QString &fileName );
    void setRootNode(SChanceTreeNode *node);

private:
    //This variable is used to store a pointer to the root of the tree graph.
    SChanceTreeNode *m_rootNode;

    //This variable is used to store the current file name.
    QString    m_fileName;
};

////////////////////////////////////////////////////////////////////////////////
/// SChanceTreeNode
///
/// @description  The SChanceTreeNode struct publicly contains all the information
///               needed to define a node in a tree graph.
///
/// @limitations  Currently encompasses both chance nodes and min/max nodes.
///               It would probably be more proper to use inheritance.
///
////////////////////////////////////////////////////////////////////////////////
struct SChanceTreeNode
{
    SChanceTreeNode();
    ~SChanceTreeNode();

    ///This variable stores the name of the node.
    QString                 s_name;

    ///This variable indicates whether this is a chance node.
    bool                    s_chance;

    qreal                   s_probability;

    ///This variable indicates if this is a min or a max node.
    ///If it is a chance node, it indicate the status of the parent.
    bool s_max;
    
    ///This variable stores the quiescent state of the node.
    bool                    s_quiescent;

    ///This variable stores the state value of the node.
    qreal                   s_value;
    
    ///This variable stores the preset history table value
    int                     s_history;

    ///This variable stores a pointer to the parent node of the node.
    SChanceTreeNode              *s_parent;

    ///This variable stores the location on the screen of the node.
    QPoint                  s_position;

    ///This variable stores the location of the probability text, if any.
    QPoint                  s_probPosition;

    ///This variable holds a list of the child nodes of the node, sorted
    ///from left to right.
    QList<SChanceTreeNode *>      s_childList;

};

#endif /*_CCHANCETREEGRAPH_H_*/
