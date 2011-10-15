////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeGraphModel.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CChanceTreeGraphModel
///               class which is a wrapper for the CChanceTreeGraph class.  It is
///               designed to allow for notification of data modification by
///               requiring that all data modification be done through this
///               class.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CCHANCETREEGRAPHMODEL_H_
#define _CCHANCETREEGRAPHMODEL_H_

#include "../CChanceTreeGraph.h"
#include "../../CStyleToolkit.h"
#include <QObject>
#include <QRect>
#include <QList>

typedef QPair<const SChanceTreeNode*, int> HistoryValue;
typedef QList<HistoryValue> HistoryTable;

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphModel
///
/// @description  The CChanceTreeGraphModel class is a wrapper for the CChanceTreeGraph.
///               It is created to allow QObjects to connect to and recieve
///               signals whenever any class has modified the underlying data.
///
/// @limitations  The signals will only be emitted if the CChanceTreeGraphModel is
///               used to edit. If it is bypassed and the CChanceTreeGraph is
///               directly manipulated then there will be no signals sent.
///
////////////////////////////////////////////////////////////////////////////////
class CChanceTreeGraphModel : public QObject
{
    Q_OBJECT

public:
    CChanceTreeGraphModel( QObject *parent );
    ~CChanceTreeGraphModel();

    void addChildNode( SChanceTreeNode *node, QPoint point );
    void addParentNode( SChanceTreeNode *node );
    void autoLayout();
    void autoName();
    void autoNumber( int min, int max );

    SChanceTreeNode * addChildNodeBasic( SChanceTreeNode *node );

    void createNewData();
    void deleteNode( SChanceTreeNode *node );

    void editHistoryPosition( QPoint point );
    void editNodeHistory( SChanceTreeNode *node, int history );
    void editNodeName( SChanceTreeNode *node, QString name );
    void editNodePosition( SChanceTreeNode *node, QPoint point );
    void editNodeQuiescentFlag( SChanceTreeNode *node, bool quiescent );
    void editNodeValue( SChanceTreeNode *node, qreal value );
    void editNodeProb( SChanceTreeNode *node, qreal value );
    void editSubTreePosition( SChanceTreeNode *parent, QPoint point );

    int findMaxDepth( SChanceTreeNode *node );
    SChanceTreeNode * findNode( QPoint point );
    SChanceTreeNode * findProb( QPoint point );
    QRect getContainingRect() const;
    QPoint getHistoryPosition() const;
    HistoryTable getHistoryTable() const;
    SChanceTreeNode * getRootNode() const;

    void load( const QString &filename );
    void realignData();
    void rehook( SChanceTreeNode *parent, SChanceTreeNode *node );
    void save( const QString &filename ) const;
    void setRootNode( SChanceTreeNode *node );

    void unhook( SChanceTreeNode *node );

private:
    void autoLayoutX( SChanceTreeNode *node, int &x );
    void autoLayoutY( SChanceTreeNode *node );
    void autoNumberHelper( int min, int max, SChanceTreeNode *node );
    SChanceTreeNode * findNodeHelper( SChanceTreeNode *node, QRect &rect );
    SChanceTreeNode * findProbHelper( SChanceTreeNode *node, QRect &rect );
    QRect getContainingRectHelper( SChanceTreeNode *parent ) const;
    void getHistoryTableHelper( const SChanceTreeNode *node,
         HistoryTable &table ) const;
    void shiftSubTree( SChanceTreeNode *parent, QPoint point );
    bool sortChildNodes( SChanceTreeNode *parent );

    //This variable is a pointer to the data set currently being used.
    CChanceTreeGraph     *m_data;
    
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

#endif /*_CCHANCEREEGRAPHMODEL_H_*/
