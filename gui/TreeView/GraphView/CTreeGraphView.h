////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeGraphView.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CTreeGraphView
///               class.  This class is responsible for allowing the user to
///               interact with the graph portion of a tree.
///
// comment
////////////////////////////////////////////////////////////////////////////////
#ifndef _CTREEGRAPHVIEW_H_
#define _CTREEGRAPHVIEW_H_

#include <QScrollArea>
#include <QString>

class CTreeHistoryArtist;
class CTreeNodeArtist;
class CTreeNodeEditor;
class CTreeNodeQuickEditor;
class CTreeNodeMover;
class CTreeGraphModel;

struct STreeNode;

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView
///
/// @description  This class allows for user interaction with the graph portion
///               of a tree.  It contains all the artists and editors necessary
///               to paint and modify the graph.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CTreeGraphView : public QScrollArea
{
    Q_OBJECT

public:
    CTreeGraphView( QWidget *parent );
    ~CTreeGraphView();

    CTreeGraphModel *getModel();
    void loadFromFile( const QString &fileName );
    void saveImage( const QString &fileName );
    void saveToFile( const QString &fileName );

	void signalRepaint(void) { modelChanged(); }

protected:
    bool eventFilter( QObject *o, QEvent *e );
    void resizeEvent( QResizeEvent *e );

protected slots:
    void autoLayout();
    void autoName();
    void autoNumber();
    void generateTree();
    void modelChanged();
    void moveDone();
    void setQuickEdit( bool on );
    void setQuickEditOff();

private:
    void setupCanvas();
    void updateCanvasSize();
    bool generateSubTree( STreeNode * root, int depth, 
                          int minBranch, int maxBranch, int qp, int &treeSize);

    QWidget                *m_canvas;
    CTreeHistoryArtist     *m_historyArtist;
    CTreeNodeArtist        *m_nodeArtist;
    CTreeNodeEditor        *m_nodeEditor;
    CTreeNodeQuickEditor   *m_nodeQuickEditor;
    CTreeNodeMover         *m_nodeMover;

    bool                    m_quickEdit;

    CTreeGraphModel        *m_model;

signals:
    void quickEditOff();
};

#endif /*_CTREEGRAPHVIEW_H_*/
