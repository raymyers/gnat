////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeGraphModel.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CTreeGraphModel
///               class which is a wrapper for the CTreeGraph class.  It is
///               designed to allow for notification of data modification by
///               requiring that all data modification be done through this
///               class.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CTREEGRAPHMODEL_H_
#define _CTREEGRAPHMODEL_H_

#include "../CTreeGraph.h"
#include "../../CStyleToolkit.h"
#include <QObject>
#include <QRect>
#include <QList>

typedef QPair<const STreeNode*, int> HistoryValue;
typedef QList<HistoryValue> HistoryTable;

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphModel
///
/// @description  The CTreeGraphModel class is a wrapper for the CTreeGraph.
///               It is created to allow QObjects to connect to and recieve
///               signals whenever any class has modified the underlying data.
///
/// @limitations  The signals will only be emitted if the CTreeGraphModel is
///               used to edit. If it is bypassed and the CTreeGraph is
///               directly manipulated then there will be no signals sent.
///
////////////////////////////////////////////////////////////////////////////////
class CTreeGraphModel : public QObject
{
    Q_OBJECT

public:
    CTreeGraphModel( QObject *parent );
    ~CTreeGraphModel();

    void addChildNode( STreeNode *node, QPoint point );
    void addParentNode( STreeNode *node );
    void autoLayout();
    void autoName();
    void autoNumber( int min, int max );

    STreeNode * addChildNodeBasic( STreeNode *node );

    void createNewData();
    void deleteNode( STreeNode *node );

    void editHistoryPosition( QPoint point );
    void editNodeHistory( STreeNode *node, int history );
    void editNodeName( STreeNode *node, QString name );
    void editNodePosition( STreeNode *node, QPoint point );
    void editNodeQuiescentFlag( STreeNode *node, bool quiescent );
    void editNodeValue( STreeNode *node, qreal value );
    void editSubTreePosition( STreeNode *parent, QPoint point );

    int findMaxDepth( STreeNode *node );
    STreeNode * findNode( QPoint point );
    QRect getContainingRect() const;
    QPoint getHistoryPosition() const;
    HistoryTable getHistoryTable() const;
    STreeNode * getRootNode() const;

    void load( const QString &filename );
    void realignData();
    void rehook( STreeNode *parent, STreeNode *node );
    void save( const QString &filename ) const;
    void setRootNode( STreeNode *node );

    void unhook( STreeNode *node );

private:
    void autoLayoutX( STreeNode *node, int &x );
    void autoLayoutY( STreeNode *node );
    void autoNumberHelper( int min, int max, STreeNode *node );
    STreeNode * findNodeHelper( STreeNode *node, QRect &rect );
    QRect getContainingRectHelper( STreeNode *parent ) const;
    void getHistoryTableHelper( const STreeNode *node,
         HistoryTable &table ) const;
    void shiftSubTree( STreeNode *parent, QPoint point );
    bool sortChildNodes( STreeNode *parent );

    //This variable is a pointer to the data set currently being used.
    CTreeGraph     *m_data;
    
    //This variable represents the upper left corner of the history table for
    //painting.
    QPoint          m_historyPosition;

    //This is a pointer to the style toolkit instance for quicker reference.
    CStyleToolkit  *m_style;

signals:
    void quickEditOff();
    void repaint();
    void retrace();
};

#endif /*_CTREEGRAPHMODEL_H_*/
