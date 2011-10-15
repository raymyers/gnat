////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeTraceView.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CChanceTreeTraceView
///               class.  This class is responsible for allowing the user to
///               view the trace portion of a tree.
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#ifndef _CCHANCETREETRACEVIEW_H_
#define _CCHANCETREETRACEVIEW_H_

#include <QTableWidget>
#include <QList>
#include <QStringList>
#include <QMenu>
#include <QToolButton>

class QLabel;
class QComboBox;
class QSpinBox;
class CChanceTreeGraphModel;
class CStyleToolkit;
class CChanceTreeTracerSelector;
struct SChanceTreeCall;
struct SChanceTreeTrace;

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeTraceView
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
class CChanceTreeTraceView : public QTableWidget
{
    Q_OBJECT

public:
    CChanceTreeTraceView( QWidget *parent );
    ~CChanceTreeTraceView();

    QSpinBox *getDepthSelector();
    QSpinBox *getQSDepthSelector();
    QSpinBox *getLowerBoundSelector();
    QSpinBox *getUpperBoundSelector();
    QToolButton * getAIConfigButton();
    QLabel *getAILabel();


    void setGraphModel( CChanceTreeGraphModel *model );

    QList<QStringList> gridFromCall( SChanceTreeCall *call );
    QList<QStringList> gridFromTrace( SChanceTreeTrace *trace );

	void signalRepaint(void) { retrace(); }

protected slots:
    void retrace();
    void saveTrace();

private:
    CChanceTreeGraphModel    *m_graphModel;
    CChanceTreeTracerSelector *m_selector;

    SChanceTreeTrace      *m_trace;
    CStyleToolkit   *m_style;
};

#endif /*_CTREETRACEVIEW_H_*/
