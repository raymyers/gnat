////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleSpanArtist.cpp
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the CPuzzleNodeArtist
///               class which is responsible for painting nodes to the screen.
///
////////////////////////////////////////////////////////////////////////////////
#include "CPuzzleSpanArtist.h"
#include <QPainterPath>
#include <QBitmap>
#include <math.h>

const qreal PI        = 3.14159265358979323846f;
const qreal PI_OVER_2 = 1.57079632679489661923f;
const qreal PI_OVER_4 = 0.78539816339744830962f;

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::CPuzzleSpanArtist
///
/// @description    This is the default constructor for the CPuzzleSpanArtist
///                 class.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param canvas:  This is the area to which all the spans will be painted.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleSpanArtist::CPuzzleSpanArtist( QPaintDevice *canvas )
{
    m_canvas = canvas;
    m_model = NULL;
    m_style = CStyleToolkit::instance();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::~CPuzzleSpanArtist
///
/// @description   This is the class destructor.
/// @pre           None
/// @post          This function frees any dynamically allocated memory made
///                by the class.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleSpanArtist::~CPuzzleSpanArtist()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::angleNorthOfEast
///
/// @description         This function calculates the angle of a line between
///                      the two given points.  The angle is given in radians
///                      with the direction East being 0.  Rotating North gives
///                      a positive value while rotating South gives a negative
///                      value.
/// @pre                 None
/// @post                None
///
/// @param nodeFromPos:  This is the point that the line is originating from.
/// @param nodeToPos:    This is the point that the line will be going to.
///
/// @return qreal:       This is the angle in radians of the line going from the
///                      first node to the second node.  East is considered 0,
///                      with North being a positive rotation and South a
///                      negative rotation.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
qreal CPuzzleSpanArtist::angleNorthOfEast( QPoint nodeFromPos,
                                           QPoint nodeToPos)
{
    qreal dy = nodeFromPos.y() - nodeToPos.y();
    qreal dx = nodeFromPos.x() - nodeToPos.x();
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
/// <function name>
///
/// @description         This function paints an arrow on the curved edge
///                      between the two given nodes, pointing at the second
///                      node.
/// @pre                 None
/// @post                An arrow is painted on the curved edge pointing to
///                      the second node.
///
/// @param nodeFromPos:  This is the point the curved edge begins at.
/// @param nodeToPos:    This is the point the curved edge terminates at.
/// @param painter:      This is a pointer to the QPainter which will paint the
///                      edge.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintCurvedArrow( QPoint nodeFromPos,
                                          QPoint nodeToPos,
                                          QPainter *painter )
{
    QMatrix backupMatrix = painter->matrix();
    QPen pen = painter->pen();

    int w = painter->pen().width();
    int d = m_style->getNodeRadius() * 2;

    painter->translate( nodeToPos );
    painter->rotate( (-angleNorthOfEast( nodeFromPos, nodeToPos ) +
                      PI_OVER_4 / 2.1) * 180 / PI );
    painter->translate( d/2 + w/2, 0 );

    //painter->drawLine( 0, 0, (int)(d*.25),  (int)(d*.25) );
    //painter->drawLine( 0, 0, (int)(d*.25), -(int)(d*.25) );

    pen.setCapStyle( Qt::RoundCap );
    pen.setJoinStyle( Qt::RoundJoin );
    painter->setBrush(Qt::color1);
    painter->setPen(pen);
    painter->drawPolygon(QPolygonF() << QPointF(0,0) 
			 << QPointF(d/6,d/8)
			 << QPointF(d/6,-d/8));


    painter->setMatrix( backupMatrix );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::paintCurvedEdge
///
/// @description         This function paints a curved edge using the supplied
///                      painter from the first node to the second node.
/// @pre                 None
/// @post                A curved edge is painted according to the given
///                      parameters.
///
/// @param nodeFromPos:  This is the point the curved edge begins at.
/// @param nodeToPos:    This is the point the curved edge terminates at.
/// @param painter:      This is a pointer to the QPainter which will paint the
///                      edge.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintCurvedEdge( QPoint nodeFromPos,
                                         QPoint nodeToPos,
                                         QPainter *painter )
{
    QPainterPath path;
    qreal endAngle = -angleNorthOfEast( nodeFromPos, nodeToPos );
    qreal startAngle = endAngle - PI;
    qreal curveAngle = PI_OVER_4 / 2.0;

    startAngle  += curveAngle;
    endAngle    -= curveAngle;

    qreal r = m_style->getNodeRadius();

    QPointF pathStart, pathEnd, pullStart, pullEnd;
    pathStart.setX( nodeFromPos.x() + cos( startAngle ) * r );
    pathStart.setY( nodeFromPos.y() + sin( startAngle ) * r );
    pullStart = nodeFromPos + (pathStart - nodeFromPos) * 3;
    pathEnd.setX( nodeToPos.x() + cos( endAngle ) * r );
    pathEnd.setY( nodeToPos.y() + sin( endAngle ) * r );
    pullEnd = nodeToPos + (pathEnd - nodeToPos) * 3;

    path.moveTo( pathStart );
    path.cubicTo( pullStart, pullEnd, pathEnd );
    
    painter->setBrush(Qt::NoBrush);
    painter->drawPath( path );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::paintCurvedNumber
///
/// @description         This function paints the weight of the given curved
///                      edge.
/// @pre                 None
/// @post                The value given is painted beside the curved edge.
///
/// @param nodeFromPos:  This is the point the curved edge begins at.
/// @param nodeToPos:    This is the point the curved edge terminates at.
/// @param num:          This is the weight of the edge.
/// @param painter:      This is a pointer to the QPainter which will paint the
///                      edge.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintCurvedNumber( QPoint nodeFromPos,
                                           QPoint nodeToPos, qreal num,
                                           QPainter *painter)
{
    QMatrix backupMatrix = painter->matrix();

    qreal radianNoE = angleNorthOfEast( nodeFromPos, nodeToPos );
    qreal degreeNoE = radianNoE*180/PI;
    QRect textRect = painter->boundingRect( 0, 0, 500, 500, Qt::AlignCenter,
                       QString::number( num ) );
    textRect.moveCenter( QPoint(0,0) );

    painter->translate( (nodeFromPos + nodeToPos)/2 );
    painter->rotate( 90.0 - degreeNoE );
    painter->translate( QPoint( (int)(m_style->getNodeRadius() + 5 +
                                fabs(sin(radianNoE)*textRect.width()/2) +
                                fabs(cos(radianNoE)*textRect.height()/2) ),
                                0 ) );
    painter->rotate( degreeNoE - 90 );

    painter->drawText( textRect, Qt::AlignCenter,
                       QString::number( num ) );

    painter->setMatrix( backupMatrix );
}


////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::paintMask
///
/// @description      This function paints the given span's edges originating
///                   from the given node into a pixmap for hit detection on
///                   edges in the graph.
/// @pre              None
/// @post             None
///
/// @param span:      This is a pointer to the span to be painted.
/// @param fromNode:  This is the number of the node within the span (valid
///                   values should be 1 or 2).
///
/// @limitations      NOT WORKING YET
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintMask( const SPuzzleSpan * span, int fromNode )
{
    QPen pen( m_style->getPen() );
    pen.setColor( Qt::color1 );
    QPainter painter( m_canvas );
    painter.setPen( pen );

    switch( span->s_flag )
    {
    case SPuzzleSpan::Bidirectional:
        paintStraightEdge( span->s_node1->s_position,
                           span->s_node2->s_position, &painter );
        break;
    case SPuzzleSpan::From1:
        if( fromNode == 1 )
            paintStraightEdge( span->s_node1->s_position,
                               span->s_node2->s_position, &painter );
        break;
    case SPuzzleSpan::From2:
        if( fromNode == 2 )
            paintStraightEdge( span->s_node2->s_position,
                               span->s_node1->s_position, &painter );
        break;
    case SPuzzleSpan::Split:
        if( fromNode == 1 )
            paintCurvedEdge( span->s_node1->s_position,
                             span->s_node2->s_position, &painter );
        else if( fromNode == 2 )
            paintCurvedEdge( span->s_node2->s_position,
                             span->s_node1->s_position, &painter );
        break;
    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::paintModel
///
/// @description   This function paints all the spans to the screen.
/// @pre           None
/// @post          All the spans are drawn to the screen.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintModel()
{
    if (m_model != NULL)
    {
        QPainter painter( m_canvas );
        for (int i = 0; i < m_model->getSpanCount(); i++)
        {
            paintSpan( m_model->getSpanAt(i), &painter );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::paintModel
///
/// @description   This function paints all the spans to the screen.
/// @pre           None
/// @post          All the spans are drawn to the screen.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintModel( QPainter *painter )
{
    if (m_model != NULL)
    {
        //This variable is used for painting to the canvas if no painter was
        //specified.
        QPainter localPainter( m_canvas );

        //Check if the calling function did not pass a painter and use
        //the local one in that case.
        if ( painter == NULL )
            painter = &localPainter;
        for (int i = 0; i < m_model->getSpanCount(); i++)
        {
            paintSpan( m_model->getSpanAt(i), painter );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist::paintSpan
///
/// @description     This function paints a single span to the screen. It
///                  draws the span using the given QPainter object.
/// @pre             None
/// @post            The given span is painted to the screen.
///
/// @param span:     This is a pointer to the span to be painted to the screen.
/// @param painter:  This is a pointer to the QPainter object to be used to
///                  paint the span on the screen.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintSpan( const SPuzzleSpan * span,
                                   QPainter *painter )
{
    if( span->s_flag == SPuzzleSpan::EmptySpan )
        return;

    painter->setPen( m_style->getPen() );
    painter->setFont( m_style->getFont() );
    //editPen.setCapStyle( Qt::SquareCap );
    painter->setRenderHint(QPainter::Antialiasing);

    switch( span->s_flag )
    {
    case SPuzzleSpan::Bidirectional:
        paintStraightEdge( span->s_node1->s_position,
                           span->s_node2->s_position, painter );
        paintStraightNumber( span->s_node1->s_position,
                             span->s_node2->s_position,
                             span->s_weightFrom1, painter );
        break;
    case SPuzzleSpan::From1:
        paintStraightEdge( span->s_node1->s_position,
                           span->s_node2->s_position, painter );
        paintStraightArrow( span->s_node1->s_position,
                           span->s_node2->s_position, painter );
        paintStraightNumber( span->s_node1->s_position,
                             span->s_node2->s_position,
                             span->s_weightFrom1, painter );
        break;
    case SPuzzleSpan::From2:
        paintStraightEdge( span->s_node2->s_position,
                           span->s_node1->s_position, painter );
        paintStraightArrow( span->s_node2->s_position,
                           span->s_node1->s_position, painter );
        paintStraightNumber( span->s_node1->s_position,
                             span->s_node2->s_position,
                             span->s_weightFrom2, painter );
        break;
    case SPuzzleSpan::Split:
        paintCurvedEdge( span->s_node1->s_position,
                         span->s_node2->s_position, painter );
	paintCurvedEdge( span->s_node2->s_position,
                         span->s_node1->s_position, painter );
	paintCurvedNumber( span->s_node1->s_position,
                           span->s_node2->s_position,
                           span->s_weightFrom1, painter );
        paintCurvedNumber( span->s_node2->s_position,
                           span->s_node1->s_position,
                           span->s_weightFrom2, painter );
	paintCurvedArrow( span->s_node2->s_position,
			  span->s_node1->s_position, painter );
	paintCurvedArrow( span->s_node1->s_position,
			  span->s_node2->s_position, painter );
        break;
    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::paintStraightArrow
///
/// @description         This function paints an arrow on the straight edge
///                      between the two given nodes, pointing at the second
///                      node.
/// @pre                 None
/// @post                An arrow is painted on the straight edge pointing to
///                      the second node.
///
/// @param nodeFromPos:  This is the point the straight edge begins at.
/// @param nodeToPos:    This is the point the straight edge terminates at.
/// @param painter:      This is a pointer to the QPainter which will paint the
///                      edge.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintStraightArrow( QPoint nodeFromPos,
                                            QPoint nodeToPos,
                                            QPainter *painter )
{
    QMatrix backupMatrix = painter->matrix();

    QPen pen = painter->pen();
    int w = painter->pen().width();
    int d = m_style->getNodeRadius() * 2;

    painter->translate( nodeToPos );
    painter->rotate( -angleNorthOfEast( nodeFromPos, nodeToPos )*180/PI );
    painter->translate( (d/2)+(w/2), 0 );

    //painter->drawLine( 0, 0, (int)(d*.25),  (int)(d*.25) );
    //painter->drawLine( 0, 0, (int)(d*.25), -(int)(d*.25) );
 
    pen.setCapStyle( Qt::RoundCap );
    pen.setJoinStyle( Qt::RoundJoin );
    painter->setBrush(Qt::color1);
    painter->setPen(pen);
    painter->drawPolygon(QPolygonF() << QPointF(0,0) 
			 << QPointF(d/6,d/8)
			 << QPointF(d/6,-d/8));
   
    painter->setMatrix( backupMatrix );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::paintStraightEdge
///
/// @description         This function paints a straight edge using the supplied
///                      painter from the first node to the second node.
/// @pre                 None
/// @post                A straight edge is painted according to the given
///                      parameters.
///
/// @param nodeFromPos:  This is the point the straight edge begins at.
/// @param nodeToPos:    This is the point the straight edge terminates at.
/// @param painter:      This is a pointer to the QPainter which will paint the
///                      edge.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintStraightEdge( QPoint nodeFromPos,
                                           QPoint nodeToPos,
                                           QPainter *painter )
{
    painter->drawLine( nodeFromPos, nodeToPos );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::paintStraightNumber
///
/// @description         This function paints the weight of the given straight
///                      edge.
/// @pre                 None
/// @post                The value given is painted beside the straight edge.
///
/// @param nodeFromPos:  This is the point the straight edge begins at.
/// @param nodeToPos:    This is the point the straight edge terminates at.
/// @param num:          This is the weight of the edge.
/// @param painter:      This is a pointer to the QPainter which will paint the
///                      edge.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::paintStraightNumber( QPoint nodeFromPos,
                                             QPoint nodeToPos, qreal num,
                                             QPainter *painter)
{
    QMatrix backupMatrix = painter->matrix();

    qreal radianNoE = angleNorthOfEast( nodeFromPos, nodeToPos );
    qreal degreeNoE = radianNoE*180/PI;
    QRect textRect = painter->boundingRect( 0, 0, 500, 500, Qt::AlignCenter,
                       QString::number( num ) );
    textRect.moveCenter( QPoint(0,0) );

    painter->translate( (nodeFromPos + nodeToPos)/2 );
    painter->rotate( 90.0 - degreeNoE );
    painter->translate( QPoint( (int)(5 +
                                fabs(sin(radianNoE)*textRect.width()/2) +
                                fabs(cos(radianNoE)*textRect.height()/2) ),
                                0 ) );
    painter->rotate( degreeNoE - 90 );

    painter->drawText( textRect, Qt::AlignCenter,
                       QString::number( num ) );

    painter->setMatrix( backupMatrix );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist::setModel
///
/// @description   This function sets the model pointer so that the painting
///                functions can properly access the span lists to paint
///                the spans needed.
/// @pre           None
/// @post          The model pointer is set.
///
/// @param model:  This is a pointer to the model.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleSpanArtist::setModel( const CPuzzleGraphModel *model )
{
    m_model = model;
}
