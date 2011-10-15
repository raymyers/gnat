////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleNodeMover.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleNodeMover
///               class which is responsible for allowing nodes to be dragged
///               and moved around the graph.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLENODEMOVER_H_
#define _CPUZZLENODEMOVER_H_

#include <QObject>
#include <QPoint>

class CPuzzleGraphModel;
class CStyleToolkit;
class QMouseEvent;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeMover
///
/// @description  This class is responsible for moving nodes on the canvas.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleNodeMover : public QObject
{
    Q_OBJECT

public:
    CPuzzleNodeMover( QObject *parent );
    ~CPuzzleNodeMover();

    bool canvasMouseEvent( QMouseEvent *e );
    bool isMoving();
    void setModel( CPuzzleGraphModel *model );

private:
    bool mouseMove( QMouseEvent *e );
    bool mousePress( QMouseEvent *e );
    bool mouseRelease( QMouseEvent *e );

    CPuzzleGraphModel  *m_model;
    CStyleToolkit      *m_style;

    bool    m_mouseDown;
    int     m_selNode;
    QPoint  m_dragOffset;

signals:
    void done();
};

#endif /*_CPUZZLENODEMOVER_H_*/
