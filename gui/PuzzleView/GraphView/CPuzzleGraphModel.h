////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleGraphModel.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owe, Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CPuzzleGraphModel
///               class which is a wrapper for the CPuzzleGraph class.  It is
///               designed to allow for notification of data modification by
///               requiring that all data modification be done through this
///               class.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLEGRAPHMODEL_H_
#define _CPUZZLEGRAPHMODEL_H_

#include "../CPuzzleGraph.h"
#include "../../CStyleToolkit.h"
#include <QObject>
#include <QRect>

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel
///
/// @description  The CPuzzleGraphModel class is a wrapper for the CPuzzleGraph.
///               It is created to allow QObjects to connect to and recieve
///               signals whenever any class has modified the underlying data.
///
/// @limitations  The signals will only be emitted if the CPuzzleGraphModel is
///               used to edit. If it is bypassed and the CPuzzleGraph is
///               directly manipulated then there will be no signals sent.
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleGraphModel : public QObject
{
    Q_OBJECT

public:
    CPuzzleGraphModel( QObject *parent );
    ~CPuzzleGraphModel();

    void addEdge( int node1, int node2, bool bidirectional );
    void addNode(SPuzzleNode *node);
    void addSpan(SPuzzleSpan *span);

    void createNewData();

    void deleteNodeAt(int index);
    void deleteSpanAt(int index);

    void editNodeEndFlag(int index, bool flag);
    void editNodeName(int index, QString name);
    void editNodeHeuristic(int index, qreal value );
    void editNodePosition(int index, QPoint point);
    void editNodeStartFlag(int index, bool flag);

    void editSpanFlag       (int index, SPuzzleSpan::SpanFlag flag);
    void editSpanWeightFrom1(int index, qreal weight);
    void editSpanWeightFrom2(int index, qreal weight);

    void findEdge  (QPoint p, int &span, int &type) const;
    void findWeight(QPoint p, int &span, int &type) const;

    int findNode( QPoint p ) const;
    int findSpan( QPoint p ) const;
    int findSpan( int node1, int node2 ) const;

    void autoName();
    void autoLayout();
    void snapToGrid(int x, int y);

    QRect getContainingRect() const;

    SPuzzleNode * getStartNode() const;
    QList<SPuzzleNode *> getGoalNodeList() const;

    const SPuzzleNode * getNodeAt(int index) const;
    const SPuzzleSpan * getSpanAt(int index) const;
    int getNodeCount() const;
    int getSpanCount() const;

    void load( const QString &filename );

    int newNode();
    int newSpan( int node1, int node2 );

    void realignData();

    void save( const QString &filename ) const;

private:

    bool testEdgeNormal(QPoint p, int index, int &type) const;
    bool testEdgeSplit (QPoint p, int index, int &type) const;
    bool testEdgeWeight(QPoint p, int index, int &type) const;

    void getPlaneEquation(float x1, float y1, float x2, float y2,
                          float &a, float &b, float &c) const;

    CPuzzleGraph   *m_data;
    CStyleToolkit  *m_style;

signals:
    void changed();
};

#endif
