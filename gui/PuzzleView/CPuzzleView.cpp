////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleView.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
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

#include "CPuzzleView.h"
#include "GraphView/CPuzzleGraphView.h"
#include "TraceView/CPuzzleTraceView.h"
#include <QSpinBox>
#include <QAction>

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleView::CPuzzleView
///
/// @description    This is the default constructor for the CPuzzleView class.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param parent:  This is a pointer to the parent object.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleView::CPuzzleView( QWidget *parent )
: IGNATView( parent )
{
    m_graphView = new CPuzzleGraphView( this );
    m_traceView = new CPuzzleTraceView( this );
    m_traceView->setGraphModel( m_graphView->getModel() );
    //m_traceView->hide();

    addWidget( m_graphView );
    addWidget( m_traceView );

    setChildrenCollapsible( false );

    QList<int> sizes;
    sizes.append(200);
    sizes.append(200);

    setSizes( sizes );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleView::~CPuzzleView
///
/// @description  This is the destructor for the CPuzzleView class and is
///               provided for maintenance purposes only.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleView::~CPuzzleView()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleView::loadFromFile
///
/// @description     This function loads the data in the indicated file.
/// @pre             None
/// @post            The data stored in the indicated file is loaded, provided
///                  it is a valid file.
///
/// @param fileName: This is a reference to a QString containing the filename
///                  to load data from.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleView::loadFromFile( const QString &fileName )
{
    m_graphView->loadFromFile( fileName );
    m_traceView->setGraphModel( m_graphView->getModel() );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleView::saveImage
///
/// @description      This function saves an image of the tree to the indicated
///                   file.
/// @pre              None
/// @post             The current tree graph view is saved to the indicated
///                   image file, provided it is valid.
///
/// @param fileName:  This is a reference to the QString containing the filename
///                   to save the tree graph image to.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleView::saveImage( const QString &fileName )
{
    m_graphView->saveImage( fileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleView::saveToFile
///
/// @description     This function saves the current data to the indicated file.
/// @pre             None
/// @post            The current data is saved to the indicated file, provided
///                  it is a valid file.
///
/// @param fileName: This is a reference to a QString containing the filename
///                  to save the current data to.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleView::saveToFile(const QString &fileName)
{
    m_graphView->saveToFile( fileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleView::setupToolBar
///
/// @description  This function initializes the toolbar that is used for the
///               tree view.
/// @pre          None
/// @post         The tree view toolbar is initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleView::setupToolBar()
{
    QAction *tempAction;
    QLabel *tempLabel;
    m_toolBar = new QToolBar( "Puzzle View", this );

    tempAction = m_toolBar->addAction( QIcon(":/toggleheuristics.png"),
                                       "Toggle visible heuristic values" );
    tempAction->setCheckable( true );
    tempAction->setChecked( false );
    connect( tempAction, SIGNAL(toggled(bool)), m_graphView,
             SLOT(toggleHeuristics(bool)) );


    tempAction = m_toolBar->addAction( QIcon(":/graph.png"), "Show Graph" );
    tempAction->setCheckable( true );
    tempAction->setChecked( true );
    connect( tempAction, SIGNAL(toggled(bool)), m_graphView,
             SLOT(setShown(bool)) );


    tempAction = m_toolBar->addAction(QIcon(":/trace.png"), "Show Trace" );
    tempAction->setCheckable( true );
    tempAction->setChecked( true );
    connect( tempAction, SIGNAL(toggled(bool)), m_traceView,
             SLOT(setShown(bool)) );
    
    m_toolBar->addSeparator();
    
    // "Auto Name" button
    tempAction = m_toolBar->addAction(QIcon(":/autoname.png"), "Auto Name" );
    connect( tempAction, SIGNAL(activated()), m_graphView, SLOT(autoName()) );

    // "Auto Number" button
    //tempAction = m_toolBar->addAction(QIcon(":/autonumber.png"), "Auto Number" );
    //connect( tempAction, SIGNAL(activated()), m_graphView, SLOT(autoNumber()) );

    // "Auto Layout" button
    tempAction = m_toolBar->addAction(QIcon(":/autolayout.png"), "Auto Layout");
    connect( tempAction, SIGNAL(activated()), m_graphView, SLOT(autoLayout()) );
    
    m_toolBar->addSeparator();

    // "AI Config Menu" button
    m_toolBar->addWidget(m_traceView->getAIConfigButton());


    m_toolBar->addWidget( m_traceView->getAISelector() );
    m_toolBar->addWidget( m_traceView->getTreeGraphSelector() );


    // AI Label
    tempLabel = m_traceView->getAILabel();
    m_toolBar->addWidget( m_traceView->getAILabel() );

    // Depth spinbox
    tempLabel = new QLabel( m_toolBar );
    tempLabel->setTextFormat(Qt::AutoText);
    tempLabel->setText( "  Depth" );
    m_toolBar->addWidget( tempLabel );
    m_toolBar->addWidget( m_traceView->getDepthSelector() );

    // "Save Trace" button
    tempAction = m_toolBar->addAction(QIcon(":/latex.png"), "Save Trace");
    connect( tempAction, SIGNAL(activated()), m_traceView, SLOT(saveTrace()) );

}


////////////////////////////////////////////////////////////////////////////////
/// CPuzzleView::signalRepaint
///
/// @description  This function tells the graph and trace views that they need
///               to redraw because a parameter has changed in the style tool
///               kit.
///               
/// @pre          None
/// @post         The graph and trace view are redrawn.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleView::signalRepaint(void)
{
	if(m_graphView) m_graphView->signalRepaint();
	if(m_traceView) m_traceView->signalRepaint();
}
