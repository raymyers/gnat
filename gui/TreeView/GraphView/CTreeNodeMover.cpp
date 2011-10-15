////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeNodeMover.cpp
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CTreeNodeMover class.
///               It is used for moving tree graph nodes on the screen.
///
////////////////////////////////////////////////////////////////////////////////

#include "CTreeNodeMover.h"
#include "CTreeGraphModel.h"
#include "../../CStyleToolkit.h"
#include <QApplication>
#include <QMouseEvent>

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeMover::CTreeNodeMover
///
/// @description  This is the default constructor for the CTreeNodeMover class.
///               It initializes member variables.
/// @pre          None
/// @post         Member variables are initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CTreeNodeMover::CTreeNodeMover( QObject *parent )
: QObject( parent )
{
    m_style = CStyleToolkit::instance();

    m_selNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeMover::~CTreeNodeMover
///
/// @description  This is the destructor for the CTreeNodeMover class and is
///               provided for maintenance purposes only.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CTreeNodeMover::~CTreeNodeMover()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeMover::canvasMouseEvent
///
/// @description   This function handles any mouse events which are passed
///                in from the tree graph view.  Mouse events include mouse
///                clicks, mouse movements, and mouse button releases.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the QMouseEvent which triggered this
///                call.
///
/// @return bool:  Returns true if the event is used and false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeNodeMover::canvasMouseEvent( QMouseEvent *e )
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
/// CTreeNodeMover::isMoving
///
/// @description   This function returns true if there is a node currently
///                selected and being moved.
/// @pre           None
/// @post          None
///
/// @return bool:  Returns true if a node is currently being moved.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeNodeMover::isMoving()
{
    return ( m_mouseDown && m_selNode != NULL );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeMover::mouseMove
///
/// @description   This function handles mouse move events.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the QMouseEvent which triggered this
///                call.
///
/// @return bool:  Returns true if the mouse move event was used and false
///                otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeNodeMover::mouseMove( QMouseEvent *e )
{
    //Check if a node is currently being moved and edit its position if it is.
    if( isMoving() )
    {
        if( QApplication::keyboardModifiers() & Qt::ControlModifier )
            m_model->editSubTreePosition(m_selNode, e->pos() + m_dragOffset);
        else
            m_model->editNodePosition(m_selNode, e->pos() + m_dragOffset);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeMover::mousePress
///
/// @description   This function handles mouse button press events.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the QMouseEvent which triggered this
///                call.
///
/// @return bool:  Returns true if the mouse button press event was used and
///                false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeNodeMover::mousePress( QMouseEvent *e )
{
    //Check if the left mouse button was pressed.
    if( e->button() == Qt::LeftButton )
    {
        //Find if there was a node where the user clicked.
        m_selNode = m_model->findNode( e->pos() );

        //Check if a node was found.  If one was, calculate the offset from the
        //center of the node to location clicked and indicate the node is being
        //moved.
        if( m_selNode != NULL )
        {
            m_dragOffset = m_selNode->s_position - e->pos();
            m_mouseDown = true;
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeMover::mouseRelease
///
/// @description   This function handles mouse button release events.
/// @pre           None
/// @post          None
///
/// @param e:      This is a pointer to the QMouseEvent which trigged this
///                call.
///
/// @return bool:  Returns false always.  This works as a cancel or done
///                function.  Returning false allows other objects to use this
///                mouse event.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CTreeNodeMover::mouseRelease( QMouseEvent *e )
{
    //Check if there was a node being used or not.  If there was, release it
    //and signal that the moving is done.
    if ( isMoving() )
    {
        m_selNode = NULL;
        m_mouseDown = false;
        emit done();
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeMover::setModel
///
/// @description   This function sets the member variable pointer to the model
///                so editing functions may be called when a node is moved.
/// @pre           None
/// @post          The model pointer is updated.
///
/// @param model:  This is a pointer to the tree graph model.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CTreeNodeMover::setModel( CTreeGraphModel *model )
{
    m_model = model;
}
