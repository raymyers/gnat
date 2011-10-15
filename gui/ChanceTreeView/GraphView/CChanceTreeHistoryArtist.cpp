////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeHistoryArtist.cpp
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

#include "CChanceTreeHistoryArtist.h"

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeHistoryArtist::CChanceTreeHistoryArtist
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
CChanceTreeHistoryArtist::CChanceTreeHistoryArtist( QPaintDevice *canvas )
{
    m_canvas = canvas;
    m_model = NULL;
    m_style = CStyleToolkit::instance();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeHistoryArtist::~CChanceTreeHistoryArtist
///
/// @description   This is the class destructor and is provided for maintenance
///                purposes only.
/// @pre           None
/// @post          None
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeHistoryArtist::~CChanceTreeHistoryArtist()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeHistoryArtist::moveName
///
/// @description     This function returns the name of the move to reach node.
/// @pre             None
/// @post            The name of the move is returned.
///
/// @param node:     The SChanceTreeNode to be reached by the move.
///
/// @return QString: A QString representing the move to reach node is returned.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
QString CChanceTreeHistoryArtist::moveName( const SChanceTreeNode* node )
{
    if( node == NULL )
        return QString();
    if( node->s_parent == NULL )
        return QString();
    
    QString n = node->s_name;
    QString p = node->s_parent->s_name;
    
    if( n.count() > 1 || p.count() > 1 )
        return QString( "%1-%2" ).arg(p).arg(n);
    return QString( "%1%2" ).arg(p).arg(n);
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeHistoryArtist::paintTable
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
void CChanceTreeHistoryArtist::paintTable( QPainter *painter )
{
    //Make sure the pointer to the model has been initialized.
    if (m_model == NULL)
        return;
        
    //Make sure the root node of the tree isn't NULL, then call the
    //paintModelHelper function to pain the model.
    if ( m_model->getRootNode() == NULL )
        return;

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

    HistoryTable historyTable = m_model->getHistoryTable();
    HistoryTable::const_iterator i = historyTable.constBegin();
    QRect cellRect( painter->boundingRect( 0,0,0,0,0,"WW-WW" ) );
    cellRect.adjust( -4,-4,4,4 );
    cellRect.moveTopLeft( m_model->getHistoryPosition() );

    while ( i != historyTable.constEnd() )
    {
        painter->drawRect( cellRect );
        painter->drawText( cellRect, Qt::AlignCenter,
                           moveName(i->first) );
        
        cellRect.moveTopLeft( cellRect.bottomLeft() );
        
        painter->drawRect( cellRect );
        painter->drawText( cellRect, Qt::AlignCenter, 
                           QString::number(i->second) );
        
        cellRect.moveBottomLeft( cellRect.topRight() );
        
        ++i;
    }
}


////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeHistoryArtist::setModel
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
void CChanceTreeHistoryArtist::setModel( CChanceTreeGraphModel *model )
{
    m_model = model;
}
