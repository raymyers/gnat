////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeTraceView.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CTreeTraceView
///               class.  This class is responsible for allowing the user to
///               view the trace portion of a tree.
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#ifndef _CTREETRACEVIEW_H_
#define _CTREETRACEVIEW_H_

#include <QTableWidget>
#include <QList>
#include <QStringList>
#include <QMenu>
#include <QToolButton>

class QLabel;
class QComboBox;
class QSpinBox;
class CTreeGraphModel;
class CStyleToolkit;
class CTreeTracerSelector;
struct STreeCall;
struct STreeTrace;

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
class CTreeTraceView : public QTableWidget
{
    Q_OBJECT

public:
    CTreeTraceView( QWidget *parent );
    ~CTreeTraceView();

    QSpinBox *getDepthSelector();
    QSpinBox *getQSDepthSelector();
    QToolButton * getAIConfigButton();
    QLabel *getAILabel();


    void setGraphModel( CTreeGraphModel *model );

    QList<QStringList> gridFromCall( STreeCall *call );
    QList<QStringList> gridFromTrace( STreeTrace *trace );

	void signalRepaint(void) { retrace(); }

protected slots:
    void retrace();
    void saveTrace();

private:
    CTreeGraphModel    *m_graphModel;
    CTreeTracerSelector *m_selector;

    STreeTrace      *m_trace;
    CStyleToolkit   *m_style;
};

#endif /*_CTREETRACEVIEW_H_*/
