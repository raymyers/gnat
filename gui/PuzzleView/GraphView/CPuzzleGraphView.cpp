////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleGraphView.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleGraphView
///               class.  This class is responsible for allowing the user to
///               interact with the graph portion of a puzzle.
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#include "CPuzzleGraphView.h"
#include "CPuzzleNodeArtist.h"
#include "CPuzzleSpanArtist.h"
#include "CPuzzleNodeMover.h"
#include "CPuzzleNodeEditor.h"
#include "CPuzzleSpanEditor.h"
#include "CPuzzleNodeCreator.h"
#include "CPuzzleSpanCreator.h"
#include "CPuzzleGraphModel.h"
#include <QResizeEvent>
#include <QMessageBox>

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::CPuzzleGraphView
///
/// @description    This is the constructor of the CPuzzleGraphView class which
///                 is responsible for allowing users to interact with the graph
///                 portion of a puzzle.
/// @pre            None
/// @post           Member variables have been initialized.
///
/// @param parent:  This is a pointer to the parent QObject.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleGraphView::CPuzzleGraphView( QWidget *parent )
: QScrollArea( parent )
{
    setupCanvas();
    setWidget( m_canvas );
    m_canvas->setMouseTracking( true );

    m_model = new CPuzzleGraphModel( this );

    m_nodeArtist = new CPuzzleNodeArtist( m_canvas );
    m_nodeArtist->setModel( m_model );

    m_spanArtist = new CPuzzleSpanArtist( m_canvas );
    m_spanArtist->setModel( m_model );

    m_nodeCreator = new CPuzzleNodeCreator( m_canvas );
    m_nodeCreator->setModel( m_model );

    m_spanCreator = new CPuzzleSpanCreator( m_canvas );
    m_spanCreator->setModel( m_model );

    m_nodeMover = new CPuzzleNodeMover( m_canvas );
    m_nodeMover->setModel( m_model );

    m_nodeEditor = new CPuzzleNodeEditor( m_canvas );
    m_nodeEditor->setModel( m_model );
    m_nodeEditor->setSpanCreator( m_spanCreator );

    m_spanEditor = new CPuzzleSpanEditor( m_canvas );
    m_spanEditor->setModel( m_model );

    connect( m_model, SIGNAL(changed()), this, SLOT(modelChanged()) );
    connect( m_nodeMover, SIGNAL(done()), this, SLOT(moveDone()) );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::~CPuzzleGraphView
///
/// @description  This is the destructor for the CPuzzleGraphView class which
///               frees up dynamically allocated memory.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleGraphView::~CPuzzleGraphView()
{
    if ( m_nodeArtist )
        delete m_nodeArtist;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::eventFilter
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
bool CPuzzleGraphView::eventFilter( QObject *o, QEvent *e )
{
    bool bReturn;
    if( o == m_canvas )
    {
        switch( e->type() )
        {
        case QEvent::Paint:
            m_spanArtist->paintModel();
            m_spanCreator->paint();
            m_nodeArtist->paintModel();
            return true;
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
            bReturn = m_spanCreator->canvasMouseEvent(
                          static_cast<QMouseEvent *>(e) );
            if ( !bReturn )
                bReturn = m_nodeMover->canvasMouseEvent(
                              static_cast<QMouseEvent *>(e) );
            return bReturn;
        case QEvent::ContextMenu:
		case QEvent::MouseButtonDblClick:
            bReturn = m_nodeEditor->canvasContextEvent(
                          static_cast<QContextMenuEvent *>(e) );
            if ( !bReturn )
                bReturn = m_spanEditor->canvasContextEvent(
                              static_cast<QContextMenuEvent *>(e) );
            if ( !bReturn )
                bReturn = m_nodeCreator->canvasContextEvent(
                              static_cast<QContextMenuEvent *>(e) );
            return bReturn;
        default:
            break;
        }

    }

    return QScrollArea::eventFilter(o,e);
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::getModel
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
CPuzzleGraphModel * CPuzzleGraphView::getModel()
{
    return m_model;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::loadFromFile
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
void CPuzzleGraphView::loadFromFile( const QString &fileName )
{
    m_model->load( fileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::modelChanged
///
/// @description  This function is used as a slot to indicated the model
///               changed and the graph need to be repainted.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphView::modelChanged()
{
    m_canvas->update();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::moveDone
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
void CPuzzleGraphView::moveDone()
{
    m_model->realignData();
    updateCanvasSize();
}


////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::toggleHeuristics
///
/// @description  This function is used as a slot to tell the node artist to
///               display or not display node heuristic values
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphView::toggleHeuristics(bool b)
{
	if(!m_nodeArtist)
		return;
	m_nodeArtist->toggleHeuristics(b);
	m_canvas->update();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::autoName
///
/// @description  This function calls the autoName function within the model.
/// @pre          None
/// @post         All the nodes in the graph have alphabetic names.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphView::autoName()
{
    m_model->autoName();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::autoLayout
///
/// @description  This function calls the auto layout function within the model.
/// @pre          None
/// @post         The graph is auto layed out.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphView::autoLayout()
{
    m_model->autoLayout();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::resizeEvent
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
void CPuzzleGraphView::resizeEvent( QResizeEvent *e )
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
void CPuzzleGraphView::saveImage( const QString &fileName )
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
    
    //Spans overlap nodes, so nodes must be drawn last.
    m_spanArtist->paintModel( &painter );
    m_nodeArtist->paintModel( &painter );

    //Save the image.
    image.save( fileName, "PNG" );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::saveToFile
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
void CPuzzleGraphView::saveToFile( const QString &fileName )
{
    m_model->save( fileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::setupCanvas
///
/// @description  This function creates the canvas and prepares it to be drawn
///               on.
/// @pre          None
/// @post         The canvas is ready to be drawn to.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphView::setupCanvas()
{
    m_canvas = new QWidget;

    QPalette pal(m_canvas->palette());
    pal.setColor( QPalette::Active, QPalette::Background, Qt::white );
    m_canvas->setPalette(pal);
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphView::updateCanvasSize
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
void CPuzzleGraphView::updateCanvasSize()
{
    QRect minCanvasRect( m_model->getContainingRect() );
    minCanvasRect.setTopLeft( QPoint(0,0) );

    m_canvas->resize( minCanvasRect.size().expandedTo( viewport()->size() ) );
}
