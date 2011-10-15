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
#include "CTreeTraceView.h"
#include "../STreeTrace.h"
#include "../GraphView/CTreeGraphModel.h"
#include "../../CStyleToolkit.h"
#include "TreeTracers/CTreeTracerSelector.h"
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
CTreeTraceView::CTreeTraceView( QWidget *parent )
: QTableWidget( parent )
{
    m_trace = NULL;
    m_style = CStyleToolkit::instance();

    //The m_selector encapsulates all the editables in the tool bar that have to
    //do with the trace. AI Selector, Depth, and QS Depth. Any changes to those
    //three will emit the activiated() signal.
    m_selector = new CTreeTracerSelector( this );
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
CTreeTraceView::~CTreeTraceView()
{
    if ( m_trace != NULL )
        delete m_trace;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::getAIConfigButton
///
/// @description          This function returns a pointer to the AI config button.
/// @pre                  None
/// @post                 A pointer to the AI config button is returned.
///
/// @return QToolButton *:  This is a pointer to the tool button that will
///                       pop up the ai config menu.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QToolButton * CTreeTraceView::getAIConfigButton()
{
    return m_selector->getAIConfigButton();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::getDepthSelector
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
QSpinBox *CTreeTraceView::getDepthSelector()
{
    return m_selector->getDepthSelector();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::getQSDepthSelector
///
/// @description          This function returns a pointer to the Quiescent
///                       Search Depth Selector.
/// @pre                  None
/// @post                 A pointer to the qs depth selector is returned.
///
/// @return QSpinBox *:   This is a pointer to a QSpinBox used in the
///                       CTreeTraceView's CTreeTraceSelector. Changing the
///                       value of this spinbox will immediately change the
///                       trace displayed if it depends on the QS depth value.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QSpinBox *CTreeTraceView::getQSDepthSelector()
{
    return m_selector->getQSDepthSelector();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::getAILabel
///
/// @description          This function returns a pointer to the AI label.
/// @pre                  None
/// @post                 A pointer to the AI label is returned.
///
/// @return QLabel *:     This is a pointer to the AI label.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
QLabel *CTreeTraceView::getAILabel()
{
    return m_selector->getAILabel();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::retrace
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
void CTreeTraceView::retrace()
{
    //Delete any existing trace
    if( m_trace != NULL )
        delete m_trace;

    //Perform the actual trace
    m_trace = m_selector->trace( m_graphModel->getRootNode() );

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
/// CTreeTraceView::gridFromCall
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
QList<QStringList> CTreeTraceView::gridFromCall( STreeCall *call )
{
    QList<QStringList> lineGrid;

    SGeneralMLine *curLine;
    foreach( curLine, call->s_lines )
        lineGrid += curLine->toStringList();

    STreeCall *curCall;
    foreach( curCall, call->s_children )
        lineGrid += gridFromCall( curCall );

    return lineGrid;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::gridFromTrace
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
QList<QStringList> CTreeTraceView::gridFromTrace( STreeTrace *trace )
{
    QList<QStringList> stringGrid;

    QStringList blankLine;
    for( int i = trace->s_columnNames.count(); i > 0; --i )
        blankLine.append("");

    STreeCall *rootCall;
    foreach( rootCall, trace->s_depths )
    {
        stringGrid += gridFromCall( rootCall );
        stringGrid += blankLine;
    }
    if( !stringGrid.isEmpty() )
        stringGrid.takeLast();

    return stringGrid;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::saveTrace
///
/// @description  Saves the trace to a latex formatted table in a new file.
/// @pre          None
/// @post         The trace grid is converted to latex format and saved into a
///               .tex plain text file.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeTraceView::saveTrace()
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
    QString rowEnd(" \\\\ \n");
    QString rowSeparator("\\hline ");

    // \begin{tabluar}{l|l|l|l|l}
    stream<<"\\begin{tabular}{l";
    for( int i = columnCount(); i > 1; --i )
        stream<<"|l";
    stream<<"}\n";

    // open &eval &stuff...
    QString output = m_trace->s_columnNames.join( "\t&" );
    output.replace( "a,B", "$\\alpha,\\beta$" );
    stream<<output;
    stream<<rowEnd;

    // All the lines
    QList<QStringList> grid = gridFromTrace(m_trace);
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

            //output.replace( "\\", "\\\\" );
            //output.replace( "&", "\\&" );
            output.replace( "%", "\\%" );

            // Push it to the file and end the line/row
            stream<<output;
            stream<<rowEnd;
        }
    }
    stream<<"\n\\end{tabular}\n";
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeTraceView::setGraphModel
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
void CTreeTraceView::setGraphModel( CTreeGraphModel *model )
{
    m_graphModel = model;
    connect( model, SIGNAL(retrace()), this, SLOT(retrace()) );
    retrace();
}
