////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleNodeCreator.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleNodeCreator
///               class which is responsible for creating new puzzle graph
///               nodes.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLENODECREATOR_H_
#define _CPUZZLENODECREATOR_H_

#include <QObject>
#include <QMenu>
#include <QContextMenuEvent>

class CPuzzleGraphModel;
class QWidget;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeCreator
///
/// @description  This class is responsible for creating new nodes.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleNodeCreator : public QObject
{
    Q_OBJECT

public:
    CPuzzleNodeCreator( QWidget *canvas );
    ~CPuzzleNodeCreator();

    bool canvasContextEvent( QContextMenuEvent *e );
    void setModel( CPuzzleGraphModel *model );

private slots:
    void newNodeClicked();

private:
    void setupActions();
    void setupMenus();

    CPuzzleGraphModel  *m_model;
    QWidget            *m_canvas;

    QMenu *m_defaultMenu;

    QAction *m_newNodeAction;
};

#endif /*_CPUZZLENODECREATOR_H_*/
