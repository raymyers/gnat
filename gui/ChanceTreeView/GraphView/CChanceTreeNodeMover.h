////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeNodeMover.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CChanceTreeNodeMover
///               class. It is used for moving tree graph nodes on the screen.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CCHANCETREENODEMOVER_H_
#define _CCHANCETREENODEMOVER_H_

#include <QObject>
#include <QPoint>

class CChanceTreeGraphModel;
class CStyleToolkit;
class QMouseEvent;
class SChanceTreeNode;

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeMover
///
/// @description  This class is responsible for moving nodes on the canvas.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CChanceTreeNodeMover : public QObject
{
    Q_OBJECT

public:
    CChanceTreeNodeMover( QObject *parent );
    ~CChanceTreeNodeMover();

    bool canvasMouseEvent( QMouseEvent *e );
    bool isMoving();
    void setModel( CChanceTreeGraphModel *model );

private:
    bool mousePress( QMouseEvent *e );
    bool mouseMove( QMouseEvent *e );
    bool mouseRelease( QMouseEvent *e );

    //This variable is used to store a pointer to the model for editing node
    //positions.
    CChanceTreeGraphModel    *m_model;

    //This variable is used to store a pointer to the style toolkit for easier
    //reference.
    CStyleToolkit      *m_style;

    //This variable is true when the mouse is clicked down and a node is being
    //moved.
    bool       m_mouseDown;

    //This variable is used to store a pointer to the node being moved.
    SChanceTreeNode *m_selNode;

    //This variable stores an offset from the center point of the node so the
    //moving of a node doesn't matter where in the node is clicked.
    QPoint     m_dragOffset;

signals:
    void done();
};

#endif /*_CTREENODEMOVER_H_*/
