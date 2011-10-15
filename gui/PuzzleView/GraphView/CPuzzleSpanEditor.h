////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleSpanEditor.cpp
///
/// @author       Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the CPuzzleSpanEditor class which is used
///               to handle end user interaction with spans.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __CPUZZLESPANEDITOR_H
#define __CPUZZLESPANEDITOR_H

#include <QObject>
#include <QMenu>
#include <QContextMenuEvent>
#include "../CPuzzleGraph.h"

class  CPuzzleGraphModel;
class  QLineEdit;
class  QWidget;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanEditor
/// 
/// @description  This class is used to handle end user interaction with spans.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleSpanEditor : public QObject
{
    Q_OBJECT

public:

    CPuzzleSpanEditor (QWidget *canvas);
    ~CPuzzleSpanEditor(void);

	void setModel(CPuzzleGraphModel *model);
    bool canvasContextEvent(QContextMenuEvent *e);
    bool isEditing(void);

public slots:

    void done();

private slots:

    void bidirectionalClicked(void);
	void directionalClicked  (void);
	void weightClicked       (void);
	void deleteClicked       (void);

private:

    void setupActions(void);
    void setupMenus(void);

    CPuzzleGraphModel *m_model;
    QWidget           *m_canvas;
	QMenu             *m_edgeMenu;
    QLineEdit         *m_lineEdit;

    QAction *m_bidirectionalAction;
    QAction *m_directionalAction;
    QAction *m_weightAction;
    QAction *m_deleteAction;

	const SPuzzleSpan *m_selSpan;	// Pointer to selected span
    int                m_selIndex;	// Selected span index
	int                m_selType;	// Selection type 
									// (from node 1 to node 2) type = 1
									// (from node 2 to node 1) type = 2
									// (from and to node 1, 2) type = 3
};

#endif  //__CPUZZLESPANEDITOR_H_
