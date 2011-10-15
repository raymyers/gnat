////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleNodeArtist.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the CPuzzleNodeArtist
///               class which is responsible for painting nodes to the screen.
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#include "CPuzzleNodeArtist.h"

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist::CPuzzleNodeArtist
///
/// @description    This is the default constructor for the CPuzzleNodeArtist
///                 class.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param canvas:  This is the area to which all the nodes will be painted.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleNodeArtist::CPuzzleNodeArtist( QPaintDevice *canvas )
{
    m_canvas = canvas;
    m_model = NULL;
    m_style = CStyleToolkit::instance();
	m_showHeuristics = false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist::~CPuzzleNodeArtist
///
/// @description   This is the class destructor.
/// @pre           None
/// @post          This function frees any dynamically allocated memory made
///                by the class.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleNodeArtist::~CPuzzleNodeArtist()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist::drawArrow
///
/// @description              This function draws an arrow indicating a start
///                           node.
/// @pre                      None
/// @post                     An arrow is drawn on the screen indicating the
///                           start node.
///
/// @param painter:           This is a pointer to the QPainter object which
///                           draws the arrow to the screen.
/// @param boundingRect:      This is the bounding rectangle of the node which
///                           is the start node.
/// @param angleNorthOfEast:  This is the angle in degrees from the East
///                           direction going North to draw the arrow at.
///
/// @limitations              None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeArtist::drawArrow( QPainter *painter, QRect &boundingRect,
                                   qreal angleNorthOfEast )
{
    QMatrix backupMatrix = painter->matrix();
    QPen backupPen = painter->pen();

    QPen editPen = m_style->getPen();
    int w = painter->pen().width();
    int d = m_style->getNodeRadius() * 2;

    painter->translate( boundingRect.center() );
    painter->rotate( -angleNorthOfEast );
    painter->translate( (d/2)+(w*2), 0 );

    editPen.setCapStyle( Qt::FlatCap );
    painter->setPen( editPen );
    painter->drawLine( 5,  4, (int)(d*.60),  4 );
    painter->drawLine( 5, -4, (int)(d*.60), -4 );

    editPen.setCapStyle( Qt::SquareCap );
    painter->setPen( editPen );
    painter->drawLine( 0, 0, (int)(d*.25),  (int)(d*.25) );
    painter->drawLine( 0, 0, (int)(d*.25), -(int)(d*.25) );

    painter->setPen( backupPen );
    painter->setMatrix( backupMatrix );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist::paintModel
///
/// @description   This function paints all the nodes to the screen.
/// @pre           None
/// @post          All the nodes are drawn to the screen.
///
/// @param painter:  This is a pointer to the painter to use.  If no value is
///                  passed, create a painter to use.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeArtist::paintModel( QPainter *painter )
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

        for (int i = 0; i < m_model->getNodeCount(); i++)
        {
            paintNode( m_model->getNodeAt(i), painter );
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist::paintModel
///
/// @description   This function paints all the nodes to the screen.
/// @pre           None
/// @post          All the nodes are drawn to the screen.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeArtist::paintModel()
{
    if (m_model != NULL)
    {
        QPainter painter( m_canvas );
        for (int i = 0; i < m_model->getNodeCount(); i++)
        {
            paintNode( m_model->getNodeAt(i), &painter );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist::paintNode
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
void CPuzzleNodeArtist::paintNode( const SPuzzleNode * node,
                                   QPainter *painter )
{
    int radius = m_style->getNodeRadius();
    QRect rect(0, 0, radius * 2, radius * 2);
    rect.moveCenter(node->s_position-QPoint(1,0));
    QPen pen = m_style->getPen();
    if ( node->s_flag & SPuzzleNode::EndNode )
        pen.setWidth( pen.width() + m_style->getBoldWidth() );
    painter->setPen(pen);
    painter->setBrush(m_style->getBrush());
    painter->setFont(m_style->getFont());
    painter->setRenderHint(QPainter::Antialiasing);

	QString text;

	text.append(node->s_name);

	if(node->s_name.isEmpty() == false)
		text.append(QString(" "));

	if(m_showHeuristics)
		text.append(QString::number(node->s_heuristic));

    painter->drawEllipse(rect);
    painter->drawText(rect, Qt::AlignCenter, text);
    if( node->s_flag & SPuzzleNode::StartNode )
        drawArrow( painter, rect );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist::setModel
///
/// @description   This function sets the model pointer so that the painting
///                functions can properly access the node lists to paint
///                the nodes needed.
/// @pre           None
/// @post          The model pointer is set.
///
/// @param model:  This is a pointer to the model.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeArtist::setModel( CPuzzleGraphModel *model )
{
    m_model = model;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist::toggleHeuristics
///
/// @description   This function toggles the visibility of node heuristic values
///
/// @pre           None
/// @post          Visible heuristics are toggled
///
/// @param b:      Take a guess.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleNodeArtist::toggleHeuristics(bool b)
{
	m_showHeuristics = b;
}

