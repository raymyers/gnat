////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleView.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CPuzzleView class
///               which is used as contents in the CGNATWindow class.
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLEVIEW_H_
#define _CPUZZLEVIEW_H_

#include "../IGNATView.h"

class CPuzzleGraphView;
class CPuzzleTraceView;
class QAction;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleView
///
/// @description  This class contains the graph and trace views used while
///               working on a puzzle style dataset. It lays out the two
///               views as a QSplitter to give the user control of each one's
///               size. By default the QSplitter holds the views as
///               non-collapsible and so it is recommended that the programmer
///               connect toggle actions to show/hide the views through the
///               connectGraphAction and connectTraceAction functions;
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleView : public IGNATView
{
    Q_OBJECT

public:
    CPuzzleView(QWidget *parent=0);
    ~CPuzzleView();

    void loadFromFile( const QString &fileName );
    void saveImage( const QString &fileName );
    void saveToFile( const QString &fileName );

	void signalRepaint(void);

private:
    void setupToolBar();

    CPuzzleGraphView *m_graphView;
    CPuzzleTraceView *m_traceView;
};

#endif /*_CPUZZLEVIEW_H_*/
