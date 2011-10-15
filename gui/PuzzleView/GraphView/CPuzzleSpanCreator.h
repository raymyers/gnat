////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleSpanCreator.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleSpanCreator
///               class which is responsible for creating new spans and edges.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLESPANCREATOR_H_
#define _CPUZZLESPANCREATOR_H_

#include <QObject>
#include <QMenu>
#include <QContextMenuEvent>
#include "../CPuzzleGraph.h"
#include "../../CStyleToolkit.h"

class CPuzzleGraphModel;
class QWidget;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator
///
/// @description  This class is responsible for creating new spans and edges.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleSpanCreator : public QObject
{
    Q_OBJECT

public:
    CPuzzleSpanCreator( QWidget *canvas );
    ~CPuzzleSpanCreator();

    bool canvasMouseEvent( QMouseEvent *e );
    QMenu * getAddSpanMenu() const;
    bool isAddingSpan() const;
    void paint();
    void setAnchorNode( int node );
    void setAnchorPoint( const QPoint &point );
    void setModel( CPuzzleGraphModel *model );

private slots:
    void addBidirectionalEdge();
    void addDirectionalEdge();

private:
    void addEdgeDone( const QPoint &point );
    qreal angleNorthOfEast( QPoint startPoint, QPoint endPoint );
    void paintArrow( QPoint anchorPoint, QPoint endPoint, QPainter *painter );
    void setupActions();
    void setupMenus();

    CPuzzleGraphModel  *m_model;
    QWidget            *m_canvas;
    int                 m_anchorNode;
    QPoint              m_anchorPoint;
    bool                m_isAddingSpan;
    bool                m_isBidirectional;

    QMenu *m_nodeAddSpanMenu;
    CStyleToolkit *m_style;

    QAction *m_newBidirectionalAction;
    QAction *m_newDirectionalAction;
};

#endif /*_CPUZZLESPANCREATOR_H_*/
