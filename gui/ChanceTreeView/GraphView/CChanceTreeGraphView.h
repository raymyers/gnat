////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeGraphView.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CChanceTreeGraphView
///               class.  This class is responsible for allowing the user to
///               interact with the graph portion of a tree.
///
// comment
////////////////////////////////////////////////////////////////////////////////
#ifndef _CCHANCETREEGRAPHVIEW_H_
#define _CCHANCETREEGRAPHVIEW_H_

#include <QScrollArea>
#include <QString>

class CChanceTreeHistoryArtist;
class CChanceTreeNodeArtist;
class CChanceTreeNodeEditor;
class CChanceTreeNodeQuickEditor;
class CChanceTreeNodeMover;
class CChanceTreeGraphModel;

struct SChanceTreeNode;

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphView
///
/// @description  This class allows for user interaction with the graph portion
///               of a tree.  It contains all the artists and editors necessary
///               to paint and modify the graph.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CChanceTreeGraphView : public QScrollArea
{
    Q_OBJECT

public:
    CChanceTreeGraphView( QWidget *parent );
    ~CChanceTreeGraphView();

    CChanceTreeGraphModel *getModel();
    void loadFromFile( const QString &fileName );
    void saveImage( const QString &fileName );
    void saveToFile( const QString &fileName );

	void signalRepaint(void) { modelChanged(); }

protected:
    bool eventFilter( QObject *o, QEvent *e );
    void resizeEvent( QResizeEvent *e );

protected slots:
    void autoLayout();
    void toggleChanceHeuristics(bool b);
    void toggleMinMaxHeuristics(bool b);
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
    bool generateSubTree( SChanceTreeNode * root, int depth, 
                          int minBranch, int maxBranch, int qp, int &treeSize);

    QWidget                *m_canvas;
    CChanceTreeHistoryArtist     *m_historyArtist;
    CChanceTreeNodeArtist        *m_nodeArtist;
    CChanceTreeNodeEditor        *m_nodeEditor;
    CChanceTreeNodeQuickEditor   *m_nodeQuickEditor;
    CChanceTreeNodeMover         *m_nodeMover;

    bool                    m_quickEdit;

    CChanceTreeGraphModel        *m_model;

signals:
    void quickEditOff();
};

#endif /*_CTREEGRAPHVIEW_H_*/
