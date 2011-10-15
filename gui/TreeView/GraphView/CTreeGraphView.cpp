////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeGraphView.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the CTreeGraphView
///               class.  This class is responsible for allowing the user to
///               interact with the graph portion of a tree.
///
////////////////////////////////////////////////////////////////////////////////

#include "CTreeGraphView.h"
#include "CTreeHistoryArtist.h"
#include "CTreeNodeArtist.h"
#include "CTreeNodeEditor.h"
#include "CTreeNodeQuickEditor.h"
#include "CTreeNodeMover.h"
#include "CTreeGraphModel.h"
#include "../../../gnat.h"
#include <QResizeEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::CTreeGraphView
///
/// @description    This is the constructor of the CTreeGraphView class which is
///                 responsible for allowing users to interact with the graph
///                 portion of a tree.
/// @pre            None
/// @post           Member variables have been initialized.
///
/// @param parent:  This is a pointer to the parent QObject.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CTreeGraphView::CTreeGraphView( QWidget *parent )
: QScrollArea( parent )
{
    //Initialize the canvas.
    setupCanvas();
    setWidget( m_canvas );
    m_canvas->setMouseTracking( true );
    m_canvas->setFocusPolicy(Qt::StrongFocus);

    //Create the model, node artist, and editors.
    m_model = new CTreeGraphModel( this );

    m_historyArtist = new CTreeHistoryArtist( m_canvas );
    m_historyArtist->setModel( m_model );

    m_nodeArtist = new CTreeNodeArtist( m_canvas );
    m_nodeArtist->setModel( m_model );

    m_nodeMover = new CTreeNodeMover( m_canvas );
    m_nodeMover->setModel( m_model );

    m_nodeEditor = new CTreeNodeEditor( m_canvas );
    m_nodeEditor->setModel( m_model );
    m_nodeEditor->setArtist( m_nodeArtist );

    m_nodeQuickEditor = new CTreeNodeQuickEditor( m_canvas );
    m_nodeQuickEditor->setModel( m_model );
    m_quickEdit = false;

    connect( m_model, SIGNAL(repaint()), this, SLOT(modelChanged()) );
    connect( m_model, SIGNAL(quickEditOff()), this, SLOT(setQuickEditOff()));
    connect( m_nodeMover, SIGNAL(done()), this, SLOT(moveDone()) );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::~CTreeGraphView
///
/// @description  This is the destructor for the CTreeGraphView class which
///               frees up dynamically allocated memory.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CTreeGraphView::~CTreeGraphView()
{
    if ( m_nodeArtist )
        delete m_nodeArtist;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::autoLayout
///
/// @description  This function calls the auto layout function within the model.
/// @pre          None
/// @post         The graph is auto layed out.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::autoLayout()
{
    m_model->autoLayout();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::autoName
///
/// @description  This function calls the auto name function within the model.
/// @pre          None
/// @post         All the nodes in the graph are auto named.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::autoName()
{
    m_model->autoName();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::autoNumber
///
/// @description  This function prompts the user for a minimum and maximum
///               integer to use when randomly generating node values for the
///               graph.
/// @pre          None
/// @post         If a valid number range is given, all nodes are given a
///               randomly generated value within the range.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::autoNumber()
{
    bool ok;

    //Prompt the user for a minimum value.
    int min = QInputDialog::getInteger( parentWidget(), tr("Minimum Value"),
                  tr("Enter minimum value"), 0, (int)MIN_FLOAT_VAL,
                  (int)MAX_FLOAT_VAL, 1, &ok );

    //Check if the user clicked cancel and return if they did.
    if ( !ok )
        return;

    //Prompt the user for a maximum value.
    int max = QInputDialog::getInteger( parentWidget(), tr("Maximum Value"),
                  tr("Enter maximum value"), 0, (int)MIN_FLOAT_VAL,
                  (int)MAX_FLOAT_VAL, 1, &ok );

    //Check if the user clicked ok instead of cancel.
    if ( ok )
        //Check if the range was valid and randomly generate numbers for the
        //graph nodes.
        if ( min < max )
            m_model->autoNumber( min, max );
        //Otherwise, warn the user the range wasn't valid.
        else
            QMessageBox::critical( NULL, tr("Error!"), tr("Max value must be "
                                   "greater than min value."),
                                   QMessageBox::Ok, QMessageBox::NoButton );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::generateTree
///
/// @description  This function prompts the user for a depth, a minimum and maximum
///               branching factor, and a quiescent percentage. The current tree is
///               then replaced by a random tree matching that description.
/// @pre          None
/// @post         Tree is replaced by a random tree
///
/// @limitations  Tree will stop being generated at 702 nodes, because the
///               naming convention can accomodate no more.
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::generateTree()
{
    // Declaring these static will make the dialogs default to the value most
    // recently entered.
    static int depth = 4;
    static int minBranch = 1;
    static int maxBranch = 4;
    static int qp = 0;
    bool ok;
    depth = QInputDialog::getInteger( parentWidget(),
                    tr("Depth"), // Title
                    tr("Enter depth"), // Label
                    depth, // Value
                    0, // MinValue
                    20, // MaxValue
                    1, // Step
                    &ok ); // Ok

    if(!ok) return;

    minBranch = QInputDialog::getInteger( parentWidget(),
                    tr("Branching Factor"), // Title
                    tr("Enter MIN branching factor"), // Label
                    minBranch, // Value
                    1, // MinValue
                    8, // MaxValue
                    1, // Step
                    &ok ); // Ok

    if(!ok) return;

    maxBranch = QInputDialog::getInteger( parentWidget(),
                    tr("Branching Factor"), // Title
                    tr("Enter MAX branching factor"), // Label
                    maxBranch, // Value
                    minBranch, // MinValue
                    8, // MaxValue
                    1, // Step
                    &ok ); // Ok

    if(!ok) return;


    qp = QInputDialog::getInteger( parentWidget(),
                    tr("Quiescent Percentage"), // Title
                    tr("Enter Quiescent Percentage"), // Label
                    qp, // Value
                    0, // MinValue
                    100, // MaxValue
                    1, // Step
                    &ok ); // Ok

    if(!ok) return;
    m_model->createNewData();
    int treeSize = 1;
    generateSubTree( m_model->getRootNode(), depth, minBranch, maxBranch, qp, treeSize );

    autoLayout();
    autoName();

}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::generateSubTree
///
/// @description  Helper for generateTree(). Generates a subtree with given node
///               as the root.
/// @pre          root must point to a node.
/// @post         Random subtree with given features is created, treeSize is increased
///               appropriately.
///
/// @param root:      Pointer to the beginning of the new subtree.
/// @param depth:     Desired depth of the subtree.
/// @param minBranch: Minimum branching factor.
/// @param maxBranch: Maximum branching factor.
/// @param qp:        quiescent percentage.
/// @param treeSize:  Current treesize. This will be incremented as subtree grows, and
///                   process will halt prematurely if it reaches 702.
/// @return bool: True if tree was completed. False if not.               
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeGraphView::generateSubTree( STreeNode * root, int depth, 
                                      int minBranch, int maxBranch, int qp, int &treeSize)
{
    if(depth > 0)
    {
        int branch = minBranch + ( rand() % (maxBranch - minBranch + 1) );
        for(int i=0; i<branch; i++)
        {
            STreeNode *child = m_model->addChildNodeBasic(root);
            treeSize++;
            if(treeSize >= 702)
            {
                QMessageBox::information(this, "GNAT",
                    "Node limit reached.\n"
                    "Tree generation terminated.");
                return false;
            }
            if((rand() % 100) < qp)
                child -> s_quiescent = true;
            if(!generateSubTree( child, depth-1, minBranch, maxBranch, qp, treeSize ))
                return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::eventFilter
///
/// @description   This function filters events so they get passed on to the
///                appropriate child object.
/// @pre           None
/// @post          None
///
/// @param o:      This is a pointer to the QObject where the event was
///                triggered from.
/// @param e:      This is a pointer to the QEvent that was triggered.
///
/// @return bool:  Returns true if the event was used and false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeGraphView::eventFilter( QObject *o, QEvent *e )
{
    bool bReturn;

    //Make sure the object where the event was triggered is the canvas.
    if( o == m_canvas )
    {
        switch( e->type() )
        {
        case QEvent::Paint:
            //Call the paint function in the node editor to paint any new
            //nodes or subtrees being moved.
            m_nodeEditor->paint();

            //Paint the model
            m_nodeArtist->paintModel();
            
            //Paint the History Table
            m_historyArtist->paintTable();
            return true;

        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            //Pass the event first to the node editor.
            bReturn = m_nodeEditor->canvasMouseEvent(
                          static_cast<QMouseEvent *>(e) );

            //If the node editor didn't use the event, let the node mover have
            //it.
            if ( !bReturn )
                bReturn = m_nodeMover->canvasMouseEvent(
                              static_cast<QMouseEvent *>(e) );

            //If the node mover didn't use the event and quick edit is turned
            //on, pass the event to the node quick editor.
            if ( !bReturn && m_quickEdit )
                bReturn = m_nodeQuickEditor->canvasMouseEvent(
                              static_cast<QMouseEvent *>(e) );
            return bReturn;

        case QEvent::ContextMenu:
            //Pass the context menu event to the node editor.
            return m_nodeEditor->canvasContextEvent(
                       static_cast<QContextMenuEvent *>(e) );

        case QEvent::KeyPress:
            //If quick edit is turned on, pass keypress events to the node
            //quick editor.
            if( m_quickEdit )
                return m_nodeQuickEditor->canvasKeyPressEvent(
                       static_cast<QKeyEvent *>(e) );
        default:
            break;
        }
    }

    //Pass the event to the QScrollArea.
    return QScrollArea::eventFilter(o,e);
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::getModel
///
/// @description                This function returns a pointer to the model.
/// @pre                        None
/// @post                       None
///
/// @return CTreeGraphModel *:  This is a pointer to the model.
///
/// @limitations                None
///
////////////////////////////////////////////////////////////////////////////////
CTreeGraphModel * CTreeGraphView::getModel()
{
    return m_model;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::loadFromFile
///
/// @description      This function loads the given file into the program.
/// @pre              None
/// @post             The given file is loaded.
///
/// @param fileName:  This is the path and file name of the file to be loaded.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::loadFromFile( const QString &fileName )
{
    m_model->load( fileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::modelChanged
///
/// @description  This function is used as a slot to indicated the model
///               changed and the graph need to be repainted.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::modelChanged()
{
    m_canvas->update();
    updateCanvasSize();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::moveDone
///
/// @description  This function is used as a slot to indicate a node is done
///               being moved.  It makes sure that the graph is fully shown on
///               the screen.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::moveDone()
{
    m_model->realignData();
    //updateCanvasSize();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::resizeEvent
///
/// @description  This function handles resizing the window.
/// @pre          None
/// @post         None
///
/// @param e:     This is a pointer to the resize event.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::resizeEvent( QResizeEvent *e )
{
    updateCanvasSize();
    QScrollArea::resizeEvent( e ); // Performs the rest of the logic for a
                                   // resize such as updating scroll bars
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::saveImage
///
/// @description      This function saves the current graph image as a PNG file.
/// @pre              None
/// @post             The graph image has been saved to the given PNG file name.
///
/// @param fileName:  This is the name of the file to save the graph image to.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::saveImage( const QString &fileName )
{
    //Get the containing rect of the graph to properly size the image.
    QRect rect = m_model->getContainingRect();

    //Create the pixmap to properly fit the image.
    QPixmap image( rect.size() );

    //Check transparent or white.
    int choice = QMessageBox::question( this,
        "Select Background Color",
        "Select prefered background color",
        "Transparent",
        "White");
    
    QColor background(255,255,255,0);
    if( choice == 1 )
        background.setAlpha(255);
        

    //Make the background of the image white.
    image.fill(background);
    QPainter painter( &image );

    //Translate the painter so all excess white space on the graph is removed.
    painter.translate( -( rect.topLeft() ) );
    m_nodeArtist->paintModel( &painter );

    //Save the image.
    image.save( fileName, "PNG" );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::saveToFile
///
/// @description      This function saves the graph to the given file.
/// @pre              None
/// @post             The current graph is saved to the given file, if valid.
///
/// @param fileName:  This is the name of the file to save the current graph to.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::saveToFile( const QString &fileName )
{
    m_model->save( fileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::setQuickEdit
///
/// @description  This function toggles the quick edit functionality.
/// @pre          The quick edit button was pressed.
/// @post         None
///
/// @param on:    This boolean is used to indicate if the quick edit function
///               is being turned on or off.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::setQuickEdit( bool on )
{
    m_quickEdit = on;
    m_nodeQuickEditor->stop( on );
    m_canvas->setFocus(Qt::OtherFocusReason);
}


////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::setQuickEditOff
///
/// @description  This function turns off the quick edit feature whenever
///               a node is deleted.
/// @pre          None
/// @post         Quick edit feature is turned off.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::setQuickEditOff()
{
    setQuickEdit( false );
    emit quickEditOff();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::setupCanvas
///
/// @description  This function creates the canvas and prepares it to be drawn
///               on.
/// @pre          None
/// @post         The canvas is ready to be drawn to.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::setupCanvas()
{
    m_canvas = new QWidget;

    QPalette pal(m_canvas->palette());
    pal.setColor( QPalette::Active, QPalette::Background, Qt::white );
    m_canvas->setPalette(pal);
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeGraphView::updateCanvasSize
///
/// @description  This function resizes the canvas if a node is dragged off the
///               canvas screen so that scroll bars appear and the node can
///               be viewed still.
/// @pre          None
/// @post         The canvas is resized to the smallest size which fits the
///               window and allows all nodes to still be seen.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeGraphView::updateCanvasSize()
{
    QRect minCanvasRect( m_model->getContainingRect() );
    minCanvasRect.setTopLeft( QPoint(0,0) );

    m_canvas->resize( minCanvasRect.size().expandedTo( viewport()->size() ) );
}
