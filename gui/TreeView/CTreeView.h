////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeView.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CTreeView class
///               which is used as contents in the CGNATWindow class.
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#ifndef _CTREEVIEW_H_
#define _CTREEVIEW_H_

#include "../IGNATView.h"

class CTreeGraphView;
class CTreeTraceView;
class QAction;

////////////////////////////////////////////////////////////////////////////////
/// CTreeView
///
/// @description  This class contains the graph and trace views used while
///               working on a tree style dataset. It lays out the two
///               views as a QSplitter to give the user control of each one's
///               size. By default the QSplitter holds the views as
///               non-collapsible and so it is recommended that the programmer
///               connect toggle actions to show/hide the views through the
///               connectGraphAction and connectTraceAction functions.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CTreeView : public IGNATView
{
    Q_OBJECT

public:
    CTreeView( QWidget *parent = 0 );
    ~CTreeView();

    void connectGraphAction( QAction *action );
    void connectTraceAction( QAction *action );

    void loadFromFile( const QString &filename );
    void saveImage( const QString &fileName );
    void saveToFile( const QString &filename );

	void signalRepaint(void);

protected slots:
    void quickEditOff();
    void switchOrientation();

private:
    void setupToolBar();

    QAction *m_quickEditAction;

    ///This variable stores a pointer to the tree graph view.
    CTreeGraphView *m_graphView;

    ///This variable stores a pointer to the tree trace view.
    CTreeTraceView *m_traceView;
};

#endif /*_CTREEVIEW_H_*/
