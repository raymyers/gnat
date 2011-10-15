////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleSpanCreator.cpp
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CPuzzleSpanCreator
///               class which is responsible for creating new spans and edges.
///
////////////////////////////////////////////////////////////////////////////////

#include "CPuzzleSpanCreator.h"
#include "CPuzzleGraphModel.h"

#include <QtGlobal>
#include <QPainter>
#include <math.h>

const qreal PI        = 3.14159265358979323846f;
const qreal PI_OVER_2 = 1.57079632679489661923f;
const qreal PI_OVER_4 = 0.78539816339744830962f;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::CPuzzleSpanCreator
///
/// @description    This is the default class constructor which initializes
///                 class variables.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param canvas:  This is a pointer to the canvas on which the spans are
///                 drawn.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleSpanCreator::CPuzzleSpanCreator( QWidget *canvas )
: QObject(canvas)
{
    m_canvas = canvas;
    m_isAddingSpan = false;
    m_isBidirectional = true;
    m_style = CStyleToolkit::instance();

    setupActions();
    setupMenus();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::~CPuzzleSpanCreator
///
/// @description  This is the class destructor which is provided for maintenence
///               purposes only.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleSpanCreator::~CPuzzleSpanCreator()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::addBidirectionalEdge
///
/// @description  This function is called when the user selects to add a
///               bidirectional edge.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::addBidirectionalEdge()
{
    m_isAddingSpan = true;
    m_isBidirectional = true;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::addDirectionalEdge
///
/// @description  This function is called when the user selects to add a
///               directional edge.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::addDirectionalEdge()
{
    m_isAddingSpan = true;
    m_isBidirectional = false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::addEdgeDone
///
/// @description   This function is called when the user selects a terminating
///                point for an edge that is currently being created.
/// @pre           None
/// @post          An edge is created if the terminating point of the edge is
///                a node other than the first node.
///
/// @param point:  This is the point where the user clicked to terminate the
///                edge that is currently being created.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::addEdgeDone( const QPoint &point )
{
    m_isAddingSpan = false;

    int anchorNode2 = m_model->findNode( point );
    if ( anchorNode2 != -1 )
    {
        m_model->addEdge( m_anchorNode, anchorNode2, m_isBidirectional );
    }

    m_canvas->repaint();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::angleNorthOfEast
///
/// @description         This function calculates the angle of a line between
///                      the two given points.  The angle is given in radians
///                      with the direction East being 0.  Rotating North gives
///                      a positive value while rotating South gives a negative
///                      value.
/// @pre                 None
/// @post                None
///
/// @param startPoint:   This is the point that the line is originating from.
/// @param endPoint:     This is the point that the line will be going to.
///
/// @return qreal:       This is the angle in radians of the line going from the
///                      first node to the second node.  East is considered 0,
///                      with North being a positive rotation and South a
///                      negative rotation.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
qreal CPuzzleSpanCreator::angleNorthOfEast( QPoint startPoint,
                                            QPoint endPoint )
{
    qreal dy = startPoint.y() - endPoint.y();
    qreal dx = startPoint.x() - endPoint.x();
    qreal angle;
    if( dx > 0 )
    {
        angle = atan(dy/dx);
    }
    else if( dx < 0 )
    {
        angle = atan(dy/dx);
        angle -= PI;
    }
    else
        angle = (dy>0)?PI_OVER_2:-PI_OVER_2;

    return -angle;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::canvasMouseEvent
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
bool CPuzzleSpanCreator::canvasMouseEvent( QMouseEvent *e )
{
    switch( e->type() )
    {
    case QEvent::MouseButtonPress:
        return isAddingSpan();
    case QEvent::MouseMove:
        setAnchorPoint( e->pos() );
        m_canvas->repaint();
        return false;
    case QEvent::MouseButtonRelease:
        if ( isAddingSpan() )
        {
            addEdgeDone( e->pos() );
            return true;
        }
        else
            return false;
    default:
        return false;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::getAddSpanMenu
///
/// @description      This function returns a pointer to the add span menu for
///                   use by the node editor.
/// @pre              None
/// @post             None
///
/// @return QMenu *:  This is a pointer to the add span menu.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
QMenu * CPuzzleSpanCreator::getAddSpanMenu() const
{
    return m_nodeAddSpanMenu;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::isAddingSpan
///
/// @description   This function returns true if a span is currently being
///                added and false otherwise.
/// @pre           None
/// @post          None
///
/// @return bool:  Returns true if a span is being added and false otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleSpanCreator::isAddingSpan() const
{
    return m_isAddingSpan;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::paint
///
/// @description  This function paints any in progress edges.
/// @pre          None
/// @post         Any edge being created is painted to the canvas.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::paint()
{
    if ( isAddingSpan() )
    {
        QPainter painter( m_canvas );
        QPen pen = m_style->getPen();
        pen.setColor( m_style->getNewColor() );
        painter.setPen( pen );
        painter.setRenderHint(QPainter::Antialiasing);

        painter.drawLine( m_anchorPoint,
                          m_model->getNodeAt( m_anchorNode )->s_position );
        if ( !m_isBidirectional )
            paintArrow( m_model->getNodeAt( m_anchorNode )->s_position,
                        m_anchorPoint, &painter );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::paintArrow
///
/// @description         This function paints an arrow on the straight edge
///                      between the two given points, pointing at the second
///                      point.
/// @pre                 None
/// @post                An arrow is painted on the straight edge pointing to
///                      the second point.
///
/// @param nodeFromPos:  This is the point the straight edge begins at.
/// @param nodeToPos:    This is the point the straight edge terminates at.
/// @param painter:      This is a pointer to the QPainter which will paint the
///                      edge.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::paintArrow( QPoint anchorPoint, QPoint endPoint,
                                     QPainter *painter )
{
    QMatrix backupMatrix = painter->matrix();

    QPen pen = painter->pen();
    int d = m_style->getNodeRadius() * 2;

    painter->translate( endPoint );
    painter->rotate( -angleNorthOfEast( anchorPoint, endPoint )*180/PI );

    //pen.setCapStyle( Qt::SquareCap );
    //painter->drawLine( 0, 0, (int)(d*.25),  (int)(d*.25) );
    //painter->drawLine( 0, 0, (int)(d*.25), -(int)(d*.25) );
    pen.setCapStyle( Qt::RoundCap );
    pen.setJoinStyle( Qt::RoundJoin );
    painter->setBrush( m_style->getNewColor() );
    painter->setPen(pen);
    painter->drawPolygon(QPolygonF() << QPointF(0,0) 
			 << QPointF(d/6,d/8)
			 << QPointF(d/6,-d/8));
   
    painter->setMatrix( backupMatrix );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::setAnchorNode
///
/// @description  This function sets the node which the edge being created is
///               originally anchored to as a start point.
/// @pre          None
/// @post         The anchor node is set.
///
/// @param node:  This is the index to the anchor node.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::setAnchorNode( int node )
{
    m_anchorNode = node;
    if ( m_anchorNode != -1 )
        setAnchorPoint( m_model->getNodeAt( m_anchorNode )->s_position );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::setAnchorPoint
///
/// @description   This function sets the end anchor point for the new edge
///                being created.
/// @pre           None
/// @post          The end anchor point is set.
///
/// @param point:  This is the point where the user currently has the mouse, to
///                indicate the current end point of the edge being drawn.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::setAnchorPoint( const QPoint &point )
{
    m_anchorPoint = point;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::setModel
///
/// @description   This function sets the model pointer.
/// @pre           None
/// @post          The model pointer is set.
///
/// @param model:  This is a pointer to the model.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::setModel( CPuzzleGraphModel *model )
{
    m_model = model;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::setupActions
///
/// @description  This function creates and initializes the QActions used by the
///               add span menu.
/// @pre          None
/// @post         The add span menu actions are created and initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::setupActions()
{
    m_newBidirectionalAction = new QAction( tr( "Bidirectional" ), this );
    connect( m_newBidirectionalAction, SIGNAL( triggered() ), this,
             SLOT( addBidirectionalEdge() ) );

    m_newDirectionalAction = new QAction( tr( "Directional" ), this );
    connect( m_newDirectionalAction, SIGNAL( triggered() ), this,
             SLOT( addDirectionalEdge() ) );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanCreator::setupMenus
///
/// @description  This function creates and initializes the add span menu.
/// @pre          None
/// @post         The add span menu is created and initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanCreator::setupMenus()
{
    m_nodeAddSpanMenu = new QMenu( "Add Edge", m_canvas );
    m_nodeAddSpanMenu->addAction( m_newBidirectionalAction );
    m_nodeAddSpanMenu->addAction( m_newDirectionalAction );
}
