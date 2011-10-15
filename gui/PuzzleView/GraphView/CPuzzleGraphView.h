////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleGraphView.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleGraphView
///               class.  This class is responsible for allowing the user to
///               interact with the graph portion of a puzzle.
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLEGRAPHVIEW_H_
#define _CPUZZLEGRAPHVIEW_H_

#include <QScrollArea>
#include <QString>

class CPuzzleGraphModel;
class CPuzzleNodeArtist;
class CPuzzleSpanArtist;
class CPuzzleNodeMover;
class CPuzzleNodeEditor;
class CPuzzleSpanEditor;
class CPuzzleNodeCreator;
class CPuzzleSpanCreator;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView
///
/// @description  This class allows for user interaction with the graph portion
///               of a puzzle.  It contains all the artists and editors
///               necessary to paint and modify the graph.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleGraphView : public QScrollArea
{
    Q_OBJECT

public:
    CPuzzleGraphView( QWidget *parent );
    ~CPuzzleGraphView();

    CPuzzleGraphModel *getModel();

    void loadFromFile( const QString &fileName );
    void saveToFile(const QString &fileName);
    
    void saveImage( const QString &fileName );

    void signalRepaint(void) { modelChanged(); }

protected:
    bool eventFilter( QObject *o, QEvent *e );
    void resizeEvent( QResizeEvent *e );

protected slots:
    void modelChanged();
    void moveDone();
    void toggleHeuristics(bool b);
    void autoName();
    void autoLayout();

private:
    void setupCanvas();
    void updateCanvasSize();

    QWidget *m_canvas;
    CPuzzleNodeArtist  *m_nodeArtist;
    CPuzzleSpanArtist  *m_spanArtist;

    CPuzzleNodeMover   *m_nodeMover;
    CPuzzleNodeEditor  *m_nodeEditor;
    CPuzzleSpanEditor  *m_spanEditor;
    CPuzzleNodeCreator *m_nodeCreator;
    CPuzzleSpanCreator *m_spanCreator;

    CPuzzleGraphModel  *m_model;
};

#endif
