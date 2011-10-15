////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleTraceView.cpp
///
/// @author       Bob Buehler
///
/// @compiler     C++
///
/// @project      GNAT
///
/// @description  <desc>
///
////////////////////////////////////////////////////////////////////////////////
#include "CPuzzleTraceView.h"

/*
CPuzzleTraceView::CPuzzleTraceView( QWidget *parent )
: QScrollArea( parent )
{
}
*/

////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeGraphView.cpp
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the CTreeTraceView
///               class. This class is responsible for allowing the user to
///               view the trace portion of a tree.
///
////////////////////////////////////////////////////////////////////////////////
#include "CPuzzleTraceView.h"
#include "../CPuzzleTrace.h"
#include "../GraphView/CPuzzleGraphModel.h"
#include "../../CStyleToolkit.h"
#include "PuzzleTracers/CPuzzleTracerSelector.h"
#include <QComboBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QLabel>

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::CTreeTraceView
///
/// @description    This is the constructor of the CTreeTraceView class. It uses
///                 parent to continue in creating the QObject tree.
/// @pre            None
/// @post           The default values for the stored trace, and style are set
///                 and the CTreeSelector is initialized and connected to the
///                 retrace() slot.
///
/// @param parent:  This is a pointer to the parent QObject.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleTraceView::CPuzzleTraceView( QWidget *parent )
: QTableWidget( parent )
{
    m_trace = NULL;
    m_style = CStyleToolkit::instance();

    //The m_selector encapsulates all the editables in the tool bar that have to
    //do with the trace. AI Selector, Depth, and QS Depth. Any changes to those
    //three will emit the activiated() signal.
    m_selector = new CPuzzleTracerSelector( this );
    connect( m_selector, SIGNAL(activated()), this, SLOT( retrace() ) );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::~CTreeTraceView
///
/// @description  This is the destructor for the CTreeTraceView class which
///               frees up dynamically allocated memory.
/// @pre          None
/// @post         The trace is deleted if it exists.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleTraceView::~CPuzzleTraceView()
{
    if ( m_trace != NULL )
        delete m_trace;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::getAISelector
///
/// @description          This function returns a pointer to the AI Selector.
/// @pre                  None
/// @post                 A pointer to the AI selector is returned.
///
/// @return QComboBox *:  This is a pointer to a QComboBox used in the
///                       CTreeTraceView's CTreeTraceSelector. Changing the
///                       state of this selector will immediately change the
///                       trace displayed as it is connected to the retrace()
///                       slot.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////


QMenu *CPuzzleTraceView::getAIConfigMenu()
{
    return m_selector->getAIConfigMenu();
}



QToolButton * CPuzzleTraceView::getAIConfigButton()
{
    return m_selector->getAIConfigButton();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::getDepthSelector
///
/// @description          This function returns a pointer to the Depth
///                       Selector.
/// @pre                  None
/// @post                 A pointer to the depth selector is returned.
///
/// @return QSpinBox *:   This is a pointer to a QSpinBox used in the
///                       CTreeTraceView's CTreeTraceSelector. Changing the
///                       value of this spinbox will immediately change the
///                       trace displayed if it depends on the depth value.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QSpinBox *CPuzzleTraceView::getDepthSelector()
{
    return m_selector->getDepthSelector();
}


QLabel *CPuzzleTraceView::getAILabel()
{
    return m_selector->getAILabel();
}

QComboBox *CPuzzleTraceView::getAISelector()
{
    return m_selector->getAISelector();
}

QComboBox *CPuzzleTraceView::getTreeGraphSelector()
{
    return m_selector->getTreeGraphSelector();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::retrace
///
/// @description  This function is a Qt slot. Calling it will update the trace
///               to reflect the current states of the Graph Model and the
///               Trace Selector.
/// @pre          None
/// @post         The trace is updated and repainted.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTraceView::retrace()
{
    //Delete any existing trace
    if( m_trace != NULL )
        delete m_trace;

    //Perform the actual trace
    m_trace = m_selector->trace( m_graphModel );

    //Get the string grid
    QList<QStringList> lines = gridFromTrace( m_trace );
    int rows = lines.count();
    int columns = m_trace->s_columnNames.count();

    //Update the table parameters
    setColumnCount(columns);
    setHorizontalHeaderLabels( m_trace->s_columnNames );
    setRowCount(rows);

    //For loop through the 2d array of QStrings
    QTableWidgetItem *item;
    int i,j;
    for( i = 0; i < rows; ++i )
        for( j = 0; j < columns; ++j )
        {
            //create the item, set the text, set the font, plug it in.
            item = new QTableWidgetItem(lines[i][j]);
            item->setFont( m_style->getFont() );
            setItem( i, j, item );
        }

    //Resize all the columns to be just the right width
    for( j = 0; j < columns; ++j )
        resizeColumnToContents(j);
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::gridFromCall
///
/// @description          This is a recursive function that returns a 2d array
///                       of QStrings that represents the trace from the call.
/// @pre                  None
/// @post                 The grid is returned.
///
/// @param call:          This is the call to be converted to a grid of strings.
///
/// @return QList<QStringList>: This is the 2d array of QString
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QList<QStringList> CPuzzleTraceView::gridFromCall( SPuzzleCall *call )
{
    QList<QStringList> lineGrid;

    IPuzzleLine *curLine;
    foreach( curLine, call->s_lines )
        lineGrid += curLine->toStringList();
    //lineGrid.last().append("");
/*
    SPuzzleCall *curCall;
    foreach( curCall, call->s_children )
        lineGrid += gridFromCall( curCall );
*/
    return lineGrid;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleTraceView::gridFromTrace
///
/// @description          This function returns a 2d array of QStrings that
///                       represents the trace.
/// @pre                  None
/// @post                 The grid is returned.
///
/// @param trace:         This is the trace to be converted to a grid of strings.
///
/// @return QList<QStringList>: This is the 2d array of QString
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QList<QStringList> CPuzzleTraceView::gridFromTrace( SPuzzleTrace *trace )
{
    QList<QStringList> stringGrid;

    QStringList blankLine;
    QStringList preludeLine;
    QStringList commentLine;
    for( int i = trace->s_columnNames.count(); i > 0; --i )
        blankLine.append("");

    preludeLine = commentLine = blankLine;

    SPuzzleCall *rootCall;
    foreach( rootCall, trace->s_depths )
    {
        preludeLine.last() = rootCall->s_prelude;
        commentLine.last() = rootCall->s_comment;

        if(rootCall->s_prelude != "")
            stringGrid += preludeLine;
        stringGrid += gridFromCall( rootCall );
        stringGrid += commentLine;
        //stringGrid += blankLine;
    }
    //if( !stringGrid.isEmpty() ) stringGrid.takeLast();

    return stringGrid;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::saveTrace
///
/// @description  Saves the trace to a LaTeX formatted table in a new file.
/// @pre          None
/// @post         The trace grid is converted to latex format and saved into a
///               .tex plain text file.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTraceView::saveTrace()
{
    if( m_trace == NULL )
        return;

    QSettings settings;

    //Get the save file name from the user.
    QString saveFileName( QFileDialog::getSaveFileName(this,
            "Save to LaTeX...",
            settings.value( "filepath" ).toString(),
            "LaTeX files (*.tex)" ) );

    //Make sure the file was valid.
    if (saveFileName.isEmpty())
        return;

    //Append the file extension if the user didn't add it themselves.
    if ( !saveFileName.endsWith(".tex", Qt::CaseInsensitive ) )
        saveFileName.append( ".tex" );

    //Open the file and exit if the file didn't open.
    QFile file(saveFileName);
    file.open(QIODevice::WriteOnly);
    if( !file.isOpen() )
    {
        QMessageBox::critical(this, "Error", "Could not open file for writing." );
        return;
    }
    QTextStream stream(&file);

    //Style
    QString rowEnd(" \\\\\n");
    QString rowSeparator("\\hline ");
    QStringList collumns = m_trace->s_columnNames;
    collumns.pop_back();

    // \begin{tabluar}{l|l|l|l|l}
    SPuzzleCall *curDepth;
    foreach(curDepth, m_trace->s_depths)
    {

        if(curDepth->s_prelude != "")
            stream <<"\n\\emph{" << curDepth->s_prelude << "}\\\\";
    stream <<"\n\\begin{tabular}{l";
        for( int i = columnCount(); i > 2; --i )
            stream<<"|l";
        stream<<"}\n\\hline\n";

        // open &eval &stuff...
        QString output = collumns.join( "\t&" );

        //output.replace( "a,B", "$\\alpha,\\beta$" );

        stream<<output;
        stream<<rowEnd;

        // All the lines
        QList<QStringList> grid = gridFromCall(curDepth);
        QStringList curLine;
        foreach( curLine, grid )
        {
            // If the first and second cells are empty then consider it a depth
            // seperation in the grid.
            if( curLine[0].isEmpty() && curLine[1].isEmpty() )
                stream<<rowSeparator;
            else
            {
                if( !curLine[0].isEmpty() )
                    stream<<rowSeparator;
                stream<<rowSeparator<<"\n";
                //QString comment = curLine.last();

                while( curLine.size() >= columnCount() )
                   curLine.pop_back();
                output = curLine.join( "\t&" );
                output.replace( "-INF", "$-\\infty$");
                output.replace( "INF", "$\\infty$");

                // Change [...] to <...>
                output.replace( "[", "<" );
                output.replace( "]", ">" );

                // Change {...} to [...]
                output.replace( "{", "[" );
                output.replace( "}", "]" );

                // Change <...> to \framebox{...}
                output.replace( "<", "\\framebox{" );
                output.replace( ">", "}" );

        output.replace( "\\", "\\\\" );
        //output.replace( "&", "\\&" );
                output.replace( "%", "\%" );

                //ifoutput.append( "goal found;", "goal found;" );

                // Push it to the file and end the line/row
                stream<<output;
                stream<<rowEnd;
            }
        }
        stream<<"\\hline\n\\end{tabular}\\\\\n" << curDepth->s_comment
              << "\n";
    }

}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::setGraphModel
///
/// @description  This is a setter for the Graph Model that is traced.
/// @pre          None
/// @post         The graph model is set to model and a retrace is issued.
///
/// @param model: This is the new model.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleTraceView::setGraphModel( CPuzzleGraphModel *model )
{
    m_graphModel = model;
    connect( model, SIGNAL(changed()), this, SLOT(retrace()) );
    retrace();
}
