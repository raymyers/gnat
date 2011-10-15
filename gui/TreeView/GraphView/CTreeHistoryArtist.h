////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeHistoryArtist.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CTreeNodeArtist
///               class which is responsible for painting nodes to the screen.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CTREEHISTORYARTIST_H_
#define _CTREEHISTORYARTIST_H_

#include <QPaintDevice>
#include <QPainter>
#include "../CTreeGraph.h"
#include "CTreeGraphModel.h"
#include "../../CStyleToolkit.h"

////////////////////////////////////////////////////////////////////////////////
/// CTreeHistoryArtist
///
/// @description  This class is responsible for painting nodes to the canvas on
///               the screen.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CTreeHistoryArtist
{
public:
    CTreeHistoryArtist( QPaintDevice *canvas );
    ~CTreeHistoryArtist();

    void paintTable( QPainter *painter = NULL );
    void setModel( CTreeGraphModel *model );

private:
    QString moveName( const STreeNode* node );

    //This is a pointer to the canvas on which the model is to be painted.
    QPaintDevice       *m_canvas;

    //This is a pointer to the model which is to be painted.
    CTreeGraphModel    *m_model;

    //This is a pointer to the style toolkit for easier reference.
    CStyleToolkit      *m_style;
};

#endif /*_CTREEHISTORYARTIST_H_*/
