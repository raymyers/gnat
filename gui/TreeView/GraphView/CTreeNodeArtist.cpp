////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeNodeArtist.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the CTreeNodeArtist
///               class which is responsible for painting nodes to the screen.
///
////////////////////////////////////////////////////////////////////////////////

#include "CTreeNodeArtist.h"

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeArtist::CTreeNodeArtist
///
/// @description    This is the default constructor for the CTreeNodeArtist
///                 class.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param canvas:  This is the area to which all the nodes will be painted.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CTreeNodeArtist::CTreeNodeArtist( QPaintDevice *canvas )
{
    m_canvas = canvas;
    m_model = NULL;
    m_style = CStyleToolkit::instance();
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeArtist::~CTreeNodeArtist
///
/// @description   This is the class destructor and is provided for maintenance
///                purposes only.
/// @pre           None
/// @post          None
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
CTreeNodeArtist::~CTreeNodeArtist()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeArtist::paintModel
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
void CTreeNodeArtist::paintModel( QPainter *painter )
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
/// CTreeNodeArtist::paintModelHelper
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
void CTreeNodeArtist::paintModelHelper( const STreeNode *node,
                                          QPainter *painter )
{
    //Check if the given node has a parent node.  If it does, paint a line from
    //the current node to the parent node.
    if ( node->s_parent != NULL )
        painter->drawLine( node->s_position, node->s_parent->s_position );

    //////////////////////////////////////////////////////////////////
    //PATH=%PATH%;C:\Program Files\My Application
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
/// CTreeNodeArtist::paintNode
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
void CTreeNodeArtist::paintNode( const STreeNode *node,
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
    string.append( QString::number( node->s_value ) );
    painter->drawText( rect, Qt::AlignCenter, string );
    painter->setPen( backupPen );
}

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeArtist::setModel
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
void CTreeNodeArtist::setModel( CTreeGraphModel *model )
{
    m_model = model;
}
