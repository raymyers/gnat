////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleNodeMover.cpp
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CPuzzleNodeMover
///               class which is responsible for allowing nodes to be dragged
///               and moved around the graph.
///
////////////////////////////////////////////////////////////////////////////////

#include "CPuzzleNodeMover.h"
#include "CPuzzleGraphModel.h"
#include "../../CStyleToolkit.h"
#include <QMouseEvent>

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeMover::CPuzzleNodeMover
///
/// @description    This is the class constructor which initialies member
///                 variables.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param parent:  This is a pointer to the parent QObject.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleNodeMover::CPuzzleNodeMover( QObject *parent )
: QObject( parent )
{
    m_style = CStyleToolkit::instance();

    m_selNode = -1;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeMover::~CPuzzleNodeMover
///
/// @description  This is the class destructor and is provided for maintenence
///               purposes only.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleNodeMover::~CPuzzleNodeMover()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeMover::canvasMouseEvent
///
/// @description   This function handles mouse events and forwards it on to the
///                appropriate function to be handled, depending on what type
///                of mouse event it is.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the mouse event that triggered this
///                function call.
///
/// @return bool:  Returns true if the mouse event is used and false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleNodeMover::canvasMouseEvent( QMouseEvent *e )
{
    switch( e->type() )
    {
    case QEvent::MouseButtonPress:
        return mousePress( e );
    case QEvent::MouseMove:
        return mouseMove( e );
    case QEvent::MouseButtonRelease:
        return mouseRelease( e );
    default:
        return false;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeMover::isMoving
///
/// @description   This function returns true if there is a node being moved
///                and false otherwise.
/// @pre           None
/// @post          None
///
/// @return bool:  Returns true if there is a node being moved.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleNodeMover::isMoving()
{
    return ( m_mouseDown && m_selNode != -1 );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeMover::mouseMove
///
/// @description   This function handles mouse move events and moves the
///                selected node accordingly if there is a node being dragged.
/// @pre           None
/// @post          Any node being dragged has its position updated.
///
/// @param e:      This is a pointer to the mouse event which triggered this
///                function call.
///
/// @return bool:  Returns false always.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleNodeMover::mouseMove( QMouseEvent *e )
{
    if( isMoving() )
    {
        m_model->editNodePosition(m_selNode, e->pos() + m_dragOffset);
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeMover::mousePress
///
/// @description   This function handles mouse press events.  If the user
///                presses the left mouse button, it attempts to find if they
///                clicked on a node.  If a node is found, the node is then
///                allowed to be dragged around and moved.
/// @pre           None
/// @post          Any node that was clicked on is prepared to be moved.
///
/// @param e:      This is a pointer to the QMouseEvent which caused this
///                function call.
///
/// @return bool:  Returns true if the event is used and false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleNodeMover::mousePress( QMouseEvent *e )
{
    if( e->button() == Qt::LeftButton )
    {
        m_selNode = m_model->findNode( e->pos() );

        if( m_selNode != -1 )
        {
            m_dragOffset = m_model->getNodeAt(m_selNode)->s_position - e->pos();
            m_mouseDown = true;
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeMover::mouseRelease
///
/// @description   This function processes a mouse button being released.  It
///                finishs any moving that was being done.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the mouse event which triggered this
///                function call.
///
/// @return bool:  Returns false always.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleNodeMover::mouseRelease( QMouseEvent *e )
{
    m_selNode = -1;
    m_mouseDown = false;

    emit done();
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeMover::setModel
///
/// @description   This function sets the internally stored pointer to the model
///                to the given value.
/// @pre           None
/// @post          The member variable pointer for the model is updated.
///
/// @param model:  This is a pointer to the model.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeMover::setModel( CPuzzleGraphModel *model )
{
    m_model = model;
}
