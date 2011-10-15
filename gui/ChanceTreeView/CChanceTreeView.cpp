////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeView.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CChanceTreeView class
///               which is used as contents in the CGNATWindow class.
///
////////////////////////////////////////////////////////////////////////////////

#include "CChanceTreeView.h"
#include "GraphView/CChanceTreeGraphView.h"
#include "TraceView/CChanceTreeTraceView.h"
#include <QAction>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QToolButton>

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::CChanceTreeView
///
/// @description    This is the default constructor which initializes member
///                 variables.
/// @pre            None
/// @post           Class variables are initialized.
///
/// @param parent:  This is a pointer to the parent QWidget.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeView::CChanceTreeView( QWidget *parent )
: IGNATView( parent )
{
    m_graphView = new CChanceTreeGraphView( this );
    m_traceView = new CChanceTreeTraceView( this );
    m_traceView->setGraphModel( m_graphView->getModel() );
    //m_traceView->hide();

    addWidget( m_graphView );
    addWidget( m_traceView );

    setChildrenCollapsible( false );

    QList<int> sizes;
    sizes.append(200);
    sizes.append(200);

    setSizes( sizes );

    connect( m_graphView, SIGNAL(quickEditOff()), this, SLOT(quickEditOff()));
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::~CChanceTreeView
///
/// @description  This is the class destructor which is provided for maintanence
///               purposes.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeView::~CChanceTreeView()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::connectGraphAction
///
/// @description    This function connects the given action to the toggle
///                 buttons for showing or hiding the trace view.
/// @pre            None
/// @post           None
///
/// @param action:  This is a pointer to the action to connect.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeView::connectGraphAction( QAction *action )
{
    action->setCheckable( true );
    m_graphView->setShown( action->isChecked() );
    connect( action, SIGNAL(toggled(bool)), m_graphView, SLOT(setShown(bool)) );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::connectTraceAction
///
/// @description    This function connects the given action to the toggle
///                 buttons for showing or hiding the trace view.
/// @pre            None
/// @post           None
///
/// @param action:  This is a pointer to the action to connect.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeView::connectTraceAction( QAction *action )
{
    action->setCheckable( true );
    m_traceView->setShown( action->isChecked() );
    connect( action, SIGNAL(toggled(bool)), m_traceView, SLOT(setShown(bool)) );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::loadFromFile
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
void CChanceTreeView::loadFromFile( const QString &fileName )
{
    m_graphView->loadFromFile( fileName );
    m_traceView->setGraphModel( m_graphView->getModel() );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::quickEditOff
///
/// @description  This function merely unchecks the quick edit button if
///               quick edit is turned off by deleting a node in the tree.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeView::quickEditOff()
{
    m_quickEditAction->setChecked( false );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::saveImage
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
void CChanceTreeView::saveImage( const QString &fileName )
{
    m_graphView->saveImage( fileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::saveToFile
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
void CChanceTreeView::saveToFile( const QString &fileName )
{
    m_graphView->saveToFile( fileName );
    m_traceView->setGraphModel( m_graphView->getModel() );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::setupToolBar
///
/// @description  This function initializes the toolbar that is used for the
///               tree view.
/// @pre          None
/// @post         The tree view toolbar is initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeView::setupToolBar()
{
    QAction *tempAction;
    QLabel *tempLabel;
    QToolButton *tempButton;
    // Create toolbar.
    m_toolBar = new QToolBar( "Puzzle View", this );
    
    tempAction = m_toolBar->addAction( QIcon(":/toggleminmaxheuristics.png"),
                                       "Toggle min/max heuristic values" );
    tempAction->setCheckable( true );
    tempAction->setChecked( true );
    connect( tempAction, SIGNAL(toggled(bool)), m_graphView,
             SLOT(toggleMinMaxHeuristics(bool)) );
    
    tempAction = m_toolBar->addAction( QIcon(":/togglechanceheuristics.png"),
                                       "Toggle chance heuristic values" );
    tempAction->setCheckable( true );
    tempAction->setChecked( true );
    connect( tempAction, SIGNAL(toggled(bool)), m_graphView,
             SLOT(toggleChanceHeuristics(bool)) );

    // "Reorient View" button
    tempAction = m_toolBar->addAction( QIcon(":/orient.png"), "Reorient view" );
    connect( tempAction, SIGNAL(activated()), this, SLOT(switchOrientation()) );

    // "Show Graph" toggle button
    tempAction = m_toolBar->addAction( QIcon(":/graph.png"), "Show Graph" );
    tempAction->setCheckable( true );
    tempAction->setChecked( true );
    connect( tempAction, SIGNAL(toggled(bool)), m_graphView,
             SLOT(setShown(bool)) );

    // "Show Trace" toggle button
    tempAction = m_toolBar->addAction(QIcon(":/trace.png"), "Show Trace" );
    tempAction->setCheckable( true );
    tempAction->setChecked( true );
    connect( tempAction, SIGNAL(toggled(bool)), m_traceView,
             SLOT(setShown(bool)) );

    m_toolBar->addSeparator();

    // "Quick Edit Mode" toggle button
    m_quickEditAction = m_toolBar->addAction(QIcon(":/quickedit.png"),
                                      "Toggle Quick Edit Mode" );
    m_quickEditAction->setCheckable( true );
    m_quickEditAction->setChecked( false );
    connect( m_quickEditAction, SIGNAL(toggled(bool)), m_graphView,
             SLOT(setQuickEdit(bool)) );

    // "Generate Tree" button
    tempAction = m_toolBar->addAction(QIcon(":/graph.png"), "Generate Tree" );
    connect( tempAction, SIGNAL(activated()), m_graphView, SLOT(generateTree()) );

    // "Auto Name" button
    tempAction = m_toolBar->addAction(QIcon(":/autoname.png"), "Auto Name" );
    connect( tempAction, SIGNAL(activated()), m_graphView, SLOT(autoName()) );

    // "Auto Number" button
    tempAction = m_toolBar->addAction(QIcon(":/autonumber.png"), "Auto Number" );
    connect( tempAction, SIGNAL(activated()), m_graphView, SLOT(autoNumber()) );

    // "Auto Layout" button
    tempAction = m_toolBar->addAction(QIcon(":/autolayout.png"), "Auto Layout");
    connect( tempAction, SIGNAL(activated()), m_graphView, SLOT(autoLayout()) );

    m_toolBar->addSeparator();
    

    
    m_toolBar->addSeparator();

    // "AI Config Menu" button
    m_toolBar->addWidget(m_traceView->getAIConfigButton());

    // AI Label
    tempLabel = m_traceView->getAILabel();
    m_toolBar->addWidget( tempLabel );

    // Depth spinbox
    tempLabel = new QLabel( m_toolBar );
    tempLabel->setTextFormat(Qt::AutoText);
    tempLabel->setText( "  Depth" );
    m_toolBar->addWidget( tempLabel );
    m_toolBar->addWidget( m_traceView->getDepthSelector() );

    // QS Depth spinbox
    tempLabel = new QLabel( m_toolBar );
    tempLabel->setText( "  QS Depth" );
    m_toolBar->addWidget( tempLabel );
    m_toolBar->addWidget( m_traceView->getQSDepthSelector() );

    // Lower Bound spinbox
    tempLabel = new QLabel( m_toolBar );
    tempLabel->setText( "  Lower Bound" );
    m_toolBar->addWidget( tempLabel );
    m_toolBar->addWidget( m_traceView->getLowerBoundSelector() );

    // Upper Bound spinbox
    tempLabel = new QLabel( m_toolBar );
    tempLabel->setText( "  Upper Bound" );
    m_toolBar->addWidget( tempLabel );
    m_toolBar->addWidget( m_traceView->getUpperBoundSelector() );

    // "Save Trace" button
    tempAction = m_toolBar->addAction(QIcon(":/latex.png"), "Save Trace");
    connect( tempAction, SIGNAL(activated()), m_traceView, SLOT(saveTrace()) );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::switchOrientation
///
/// @description  This function switches the orientation of the QSplitter from
///               horizontal to vertical and back again.
/// @pre          None
/// @post         The QSplitter orientation is toggled
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeView::switchOrientation()
{
    if( orientation() == Qt::Vertical )
        setOrientation(Qt::Horizontal);
    else
        setOrientation(Qt::Vertical);
}


////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeView::signalRepaint
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
void CChanceTreeView::signalRepaint(void)
{
	if(m_graphView) { m_graphView->signalRepaint(); }
	if(m_traceView) { m_traceView->signalRepaint(); }
}
