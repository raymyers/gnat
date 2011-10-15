////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleTraceView.h
///
/// @author       Ray Myers
///
/// @compiler     C++
///
/// @project      GNAT
///
/// @description  <desc>
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLETRACEVIEW_H_
#define _CPUZZLETRACEVIEW_H_
/*
#include <QScrollArea>

class CPuzzleTraceView : public QScrollArea
{
    Q_OBJECT

public:
    CPuzzleTraceView( QWidget *parent );
};
*/
#include <QTableWidget>
#include <QList>
#include <QStringList>
#include <QMenu>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>
//class QLabel;
//class QComboBox;
class QSpinBox;
class CPuzzleGraphModel;
class CStyleToolkit;
class CPuzzleTracerSelector;
struct SPuzzleCall;
struct SPuzzleTrace;

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView
///
/// @description  This class allows the user to view the trace of a tree. It is
///               based on the QTableWidget which it uses for all of the display
///               and drawing functionality.
///
/// @limitations  The QTableWidget allows for minimal customization in how the
///               grid is drawn. Row and Column headers and the fonts can be
///               changed as well as the cell contents. It cannot change the
///               thickness or color of individual lines, nor can it display any
///               not within a cell.
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleTraceView : public QTableWidget
{
    Q_OBJECT

public:
    CPuzzleTraceView( QWidget *parent );
    ~CPuzzleTraceView();

    QSpinBox *getDepthSelector();
    //QSpinBox *getQSDepthSelector();
    QMenu *getAIConfigMenu();
    QToolButton * getAIConfigButton();
    QComboBox *getAISelector();
    QComboBox *getTreeGraphSelector();
    QLabel *getAILabel();


    void setGraphModel( CPuzzleGraphModel *model );

    QList<QStringList> gridFromCall( SPuzzleCall *call );
    QList<QStringList> gridFromTrace( SPuzzleTrace *trace );

    void signalRepaint(void) { retrace(); }

protected slots:
    void retrace();
    void saveTrace();

    /*
    void toggleID();
    void toggleDL();
    void toggleAB();

    void spawnConfigMenu();
    */
private:
    CPuzzleGraphModel     *m_graphModel;
    CPuzzleTracerSelector *m_selector;

    SPuzzleTrace    *m_trace;
    CStyleToolkit   *m_style;
};
#endif
