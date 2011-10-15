////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeNodeArtist.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the CChanceTreeNodeArtist
///               class which is responsible for painting nodes to the screen.
///
////////////////////////////////////////////////////////////////////////////////

#include "CChanceTreeNodeArtist.h"
#include <cmath>

const qreal PI        = 3.14159265358979323846f;
const qreal PI_OVER_2 = 1.57079632679489661923f;
const qreal PI_OVER_4 = 0.78539816339744830962f;

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::CChanceTreeNodeArtist
///
/// @description    This is the default constructor for the CChanceTreeNodeArtist
///                 class.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param canvas:  This is the area to which all the nodes will be painted.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeNodeArtist::CChanceTreeNodeArtist( QPaintDevice *canvas )
{
    m_canvas = canvas;
    m_model = NULL;
    m_style = CStyleToolkit::instance();
    m_showChanceHeuristics = true;
    m_showMinMaxHeuristics = true;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::~CChanceTreeNodeArtist
///
/// @description   This is the class destructor and is provided for maintenance
///                purposes only.
/// @pre           None
/// @post          None
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeNodeArtist::~CChanceTreeNodeArtist()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::paintModel
///
/// @description     This function paints all the nodes to the screen.
/// @pre             None
/// @post            All the nodes are drawn to the screen.
///
/// @param painter:  This is a pointer to the painter to use.  If no value is
///                  passed, create a painter to use.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeNodeArtist::paintModel( QPainter *painter )
{
    //Make sure the pointer to the model has been initialized.
    if (m_model != NULL)
    {
        //This variable is used for painting to the canvas if no painter was
        //specified.
        QPainter localPainter( m_canvas );

        //Check if the calling function did not pass a painter and use
        //the local one in that case.
        if ( painter == NULL )
            painter = &localPainter;

        //Create the QPainter object to paint the model.
        painter->setPen( m_style->getPen() );
        painter->setBrush( m_style->getBrush() );
        painter->setFont( m_style->getFont() );
        painter->setRenderHint( QPainter::Antialiasing );

        //Make sure the root node of the tree isn't NULL, then call the
        //paintModelHelper function to pain the model.
        if ( m_model->getRootNode() != NULL )
            paintModelHelper( m_model->getRootNode(), painter );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::paintModelHelper
///
/// @description     This function paints the given node and all its children,
///                  along with connecting lines up to its parent node by
///                  recursively calling itself on the child nodes of the given
///                  node.
/// @pre             None
/// @post            The given node and all its children have been painted.
///
/// @param node:     This is a pointer to the node to paint.
/// @param painter:  This is a pointer to the QPainter to use for painting nodes
///                  to the screen.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeNodeArtist::paintModelHelper( SChanceTreeNode *node,
                                          QPainter *painter )
{
    //Check if the given node has a parent node.  If it does, paint a line from
    //the current node to the parent node.
    if ( node->s_parent != NULL )
    {
        int radius = m_style->getNodeRadius();
        QPoint spaceY = QPoint(0, radius-7);
        if(node->s_parent->s_chance)
        {
            painter->drawLine( node->s_position - spaceY, node->s_parent->s_position + spaceY );
            node->s_probPosition = 
               paintSpanNumber( node->s_position - spaceY, node->s_parent->s_position + spaceY, 
                             node->s_probability, painter);
        }
        else
            painter->drawLine( node->s_position - spaceY, node->s_parent->s_position + spaceY);
    }
    // painter->drawLine( node->s_position, node->s_parent->s_position );

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop recursively calls this function on
    //                all the child nodes of the current node.
    //
    // Precondition:  None
    // Postcondition: All the child nodes of the current node are
    //                painted.
    //
    //////////////////////////////////////////////////////////////////
    for ( int i = 0; i < node->s_childList.size(); i++ )
        paintModelHelper( node->s_childList[i], painter );

    //Paint the current node.
    paintNode( node, painter );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::paintSpanNumber
///
/// @description         This function paints the weight of the given straight
///                      edge. Adapted from the CPuzzleSpanArtist class.
/// @pre                 None
/// @post                The value given is painted beside the straight edge.
///
/// @param nodeFromPos:  This is the point the straight edge begins at.
/// @param nodeToPos:    This is the point the straight edge terminates at.
/// @param num:          This is the weight of the edge.
/// @param painter:      This is a pointer to the QPainter which will paint the
///                      edge.
///
/// @Return QPoint       Center of text.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
QPoint CChanceTreeNodeArtist::paintSpanNumber( QPoint nodeFromPos, QPoint nodeToPos, qreal num, QPainter *painter)
{
    QMatrix backupMatrix = painter->matrix();

    qreal radianNoE = angleNorthOfEast( nodeFromPos, nodeToPos );
    qreal degreeNoE = radianNoE*180/PI;
    QRect textRect = painter->boundingRect( 0, 0, 500, 500, Qt::AlignCenter,
                       QString::number( num/100.0 ) );
    textRect.moveCenter( QPoint(0,0) );

    qreal ratio = 0.5;
    int c = 1;
    if(degreeNoE <= 90   && degreeNoE >= -90) c = -1;

    painter->translate( (nodeFromPos + nodeToPos) * ratio );
    painter->rotate( 90.0 - degreeNoE );
    painter->translate( QPoint( c*(int)(5 +
                                fabs(sin(radianNoE)*textRect.width()/2) +
                                fabs(cos(radianNoE)*textRect.height()/2) ),
                                0 ) );
    painter->rotate( degreeNoE - 90 );

    painter->drawText( textRect, Qt::AlignCenter,
                       QString::number( num/100.0 ) );
    
    QPoint textCenter = painter->matrix().map(textRect.center());

    painter->setMatrix( backupMatrix );
    //textCenter = painter->matrix().inverted().map(textRect.center());
    //m_model->getRootNode()->s_position = textCenter;
    return textCenter;

}
////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::angleNorthOfEast
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
qreal CChanceTreeNodeArtist::angleNorthOfEast( QPoint nodeFromPos, QPoint nodeToPos )
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

void drawTriangle( const QPoint center, const int radius, const bool up, QPainter *painter )
{
    int c = up ? -1 : 1;
    static const qreal alpha = 0.5*sqrt(3);
    static const qreal beta  = 1.0;
    int centerX = center.x();
    int centerY = center.y();

    QPoint points[3] = {
        QPoint(centerX-beta*radius, centerY-alpha*radius*c),
        QPoint(centerX+beta*radius, centerY-alpha*radius*c),
        QPoint(centerX,             centerY+alpha*radius*c),
    };

    painter->drawPolygon(points, 3);
}
    

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::paintNode
///
/// @description     This function paints a single node to the screen.  It
///                  draws the node using the given QPainter object.
/// @pre             None
/// @post            The given node is painted to the screen.
///
/// @param node:     This is a pointer to the node to be painted to the screen.
/// @param painter:  This is a pointer to the QPainter object to be used to
///                  paint the node on the screen.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeNodeArtist::paintNode( const SChanceTreeNode *node,
                                   QPainter *painter )
{
    if(node->s_chance)
    {
        paintChanceNode(node, painter);
        return;
    }
    //Create a bounding rectangle for the current node.
    int radius = m_style->getNodeRadius();
    int c = node->s_max ? 1 : -1;
    QRect rect( 0, 0, radius * 2, radius * 2 );
    rect.moveCenter( node->s_position );
    rect.translate(0, 10*c);
    QPen backupPen = painter->pen();
    QPen pen = painter->pen();

    //Make the node appear bolded if it is quiescent.
    if ( node->s_quiescent )
        pen.setWidth( pen.width() + m_style->getBoldWidth() );
    painter->setPen( pen );

    //Draw the triangle indicating the node.
    //painter->drawEllipse( rect );
    drawTriangle(node->s_position, radius, node->s_max, painter);


    //Create the string to appear in the node and draw it.
    QString string = node->s_name;
    if(m_showMinMaxHeuristics)
	string.append( QString::number( node->s_value ) );
    painter->drawText( rect, Qt::AlignCenter, string );
    painter->setPen( backupPen );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::paintChanceNode
///
/// @description     This function paints a single node to the screen.  It
///                  draws the node using the given QPainter object.
/// @pre             None
/// @post            The given node is painted to the screen.
///
/// @param node:     This is a pointer to the node to be painted to the screen.
/// @param painter:  This is a pointer to the QPainter object to be used to
///                  paint the node on the screen.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeNodeArtist::paintChanceNode( const SChanceTreeNode *node,
                                   QPainter *painter )
{
    //Create a bounding rectangle for the current node.
    int radius = m_style->getNodeRadius();
    QRect rect( 0, 0, radius * 2, radius * 2 );
    rect.moveCenter( node->s_position );

    QPen backupPen = painter->pen();
    QPen pen = painter->pen();

    //Make the node appear bolded if it is quiescent.
    if ( node->s_quiescent )
        pen.setWidth( pen.width() + m_style->getBoldWidth() );
    painter->setPen( pen );

    //Draw the circle indicating the node.
    painter->drawEllipse( rect );

    //Create the string to appear in the node and draw it.
    QString string = node->s_name;
    if(m_showChanceHeuristics)
        string.append( QString::number( node->s_value ) );
    painter->drawText( rect, Qt::AlignCenter, string );
    painter->setPen( backupPen );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::setModel
///
/// @description   This function sets the model pointer so that the painting
///                functions can properly access the model to paint
///                the nodes needed.
/// @pre           None
/// @post          The model pointer is set.
///
/// @param model:  This is a pointer to the model.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeNodeArtist::setModel( CChanceTreeGraphModel *model )
{
    m_model = model;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::toggleChanceHeuristics
///
/// @description   This function toggles the visibility of chance 
///                node heuristic values
///
/// @pre           None
/// @post          Visible heuristics are toggled
///
/// @param b:      Take a guess.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeNodeArtist::toggleChanceHeuristics(bool b)
{
	m_showChanceHeuristics = b;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist::toggleMinMaxHeuristics
///
/// @description   This function toggles the visibility of min/max
///                node heuristic values
///
/// @pre           None
/// @post          Visible heuristics are toggled
///
/// @param b:      Take a guess.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeNodeArtist::toggleMinMaxHeuristics(bool b)
{
	m_showMinMaxHeuristics = b;
}
