////////////////////////////////////////////////////////////////////////////////
/// @file         CTreeNodeArtist.h
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
#ifndef _CTREENODEARTIST_H_
#define _CTREENODEARTIST_H_

#include <QPaintDevice>
#include <QPainter>
#include "../CTreeGraph.h"
#include "CTreeGraphModel.h"
#include "../../CStyleToolkit.h"

////////////////////////////////////////////////////////////////////////////////
/// CTreeNodeArtist
///
/// @description  This class is responsible for painting nodes to the canvas on
///               the screen.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CTreeNodeArtist
{
public:
    CTreeNodeArtist( QPaintDevice *canvas );
    ~CTreeNodeArtist();

    void paintModel( QPainter *painter = NULL );
    void paintModelHelper( const STreeNode* node, QPainter *painter );
    void paintNode( const STreeNode* node, QPainter *painter );
    void setModel( CTreeGraphModel *model );

private:
    //This is a pointer to the canvas on which the model is to be painted.
    QPaintDevice       *m_canvas;

    //This is a pointer to the model which is to be painted.
    CTreeGraphModel    *m_model;

    //This is a pointer to the style toolkit for easier reference.
    CStyleToolkit      *m_style;
};

#endif /*_CTREENODEARTIST_H_*/
