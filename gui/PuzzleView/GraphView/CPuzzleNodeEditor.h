////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleNodeEditor.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleNodeEditor
///               class which is responsible for editing nodes.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLENODEEDITOR_H_
#define _CPUZZLENODEEDITOR_H_

#include <QObject>
#include <QMenu>
#include <QContextMenuEvent>

class CPuzzleGraphModel;
class QLineEdit;
class QWidget;
class CPuzzleSpanCreator;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeEditor
///
/// @description  This class is responsible for editing nodes in the graph.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleNodeEditor : public QObject
{
    Q_OBJECT

public:
    CPuzzleNodeEditor( QWidget *canvas );
    ~CPuzzleNodeEditor();

    bool canvasContextEvent( QContextMenuEvent *e );
    bool isEditing();

    void setModel( CPuzzleGraphModel *model );
    void setSpanCreator( CPuzzleSpanCreator *spanCreator );

public slots:
    void nameDone();
	void heuristicDone();

private slots:
    void deleteNodeClicked();
    void renameNodeClicked();
	void editHeuristicClicked();
    void setGoalNodeClicked( bool on );
    void setStartNodeClicked( bool on );

private:
    void setupActions();
    void setupMenus();

    CPuzzleGraphModel  *m_model;
    QWidget            *m_canvas;
    QLineEdit          *m_nameEdit;
	QLineEdit          *m_heuristicEdit;
    CPuzzleSpanCreator *m_spanCreator;

    int                 m_selNode;

    QMenu *m_nodeMenu;
    QMenu *m_nodeTypeSubMenu;

    QAction *m_nodeStartAction;
    QAction *m_nodeGoalAction;
    QAction *m_nodeRenameAction;
	QAction *m_editHeuristicAction;
    QAction *m_nodeDeleteAction;
};

#endif/*_CPUZZLENODEARTIST_H_*/
