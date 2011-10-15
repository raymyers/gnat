////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleNodeCreator.cpp
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CPuzzleNodeCreator
///               class which is responsible for creating new puzzle graph
///               nodes.
///
////////////////////////////////////////////////////////////////////////////////

#include "CPuzzleNodeCreator.h"
#include "CPuzzleGraphModel.h"
#include <QtGlobal>

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeCreator::CPuzzleNodeCreator
///
/// @description    This is the default constructor for the CPuzzleNodeCreator
///                 class.  It initializes class variables.
/// @pre            None
/// @post           Class variables are initialized.
///
/// @param canvas:  This is a pointer to the canvas where the nodes will be
///                 painted to.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleNodeCreator::CPuzzleNodeCreator( QWidget *canvas )
: QObject(canvas)
{
    m_canvas = canvas;

    setupActions();
    setupMenus();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeCreator::~CPuzzleNodeCreator
///
/// @description  This is the destructor for the CPuzzleNodeCreator class and is
///               provided for maintenence purposes only.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleNodeCreator::~CPuzzleNodeCreator()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeCreator::canvasContextEvent
///
/// @description   This function executes the context menu event when the user
///                right clicks on the canvas.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the context menu event which triggered
///                this function call.
///
/// @return bool:  Returns true if the context menu event was used (always
///                true).
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleNodeCreator::canvasContextEvent( QContextMenuEvent *e )
{
    m_defaultMenu->exec( e->globalPos() );
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeCreator::newNodeClicked
///
/// @description  This function adds a new node to the graph where the user
///               originally clicked to create the context menu event.
/// @pre          The user clicked on the context menu option to create a new
///               node.
/// @post         A new node is created at the location the user originally
///               clicked to create the context menu.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeCreator::newNodeClicked()
{
    int index = m_model->newNode();
    m_model->editNodePosition( index, m_canvas->mapFromGlobal(
                               m_defaultMenu->pos() ) );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeCreator::setModel
///
/// @description   This function sets the member variable pointer for the model
///                to the given value.
/// @pre           None
/// @post          The class pointer to the model is updated.
///
/// @param model:  This is a pointer to the model.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeCreator::setModel( CPuzzleGraphModel *model )
{
    m_model = model;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeCreator::setupActions
///
/// @description  This function creates and initializes the QActions used by the
///               context menu.
/// @pre          None
/// @post         The context menu actions are created and initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeCreator::setupActions()
{
    m_newNodeAction = new QAction( tr( "Add Node Here" ), this );
    connect( m_newNodeAction, SIGNAL( triggered() ), this,
             SLOT( newNodeClicked() ) );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeCreator::setupMenus
///
/// @description  This function creates and initializes the context menu.
/// @pre          None
/// @post         The context menu is created and initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeCreator::setupMenus()
{
    m_defaultMenu = new QMenu( "Default menu", m_canvas );
    m_defaultMenu->addAction( m_newNodeAction );
}
