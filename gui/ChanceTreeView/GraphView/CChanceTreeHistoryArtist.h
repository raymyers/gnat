////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeHistoryArtist.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CChanceTreeNodeArtist
///               class which is responsible for painting nodes to the screen.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CCHANCETREEHISTORYARTIST_H_
#define _CCHANCETREEHISTORYARTIST_H_

#include <QPaintDevice>
#include <QPainter>
#include "../CChanceTreeGraph.h"
#include "CChanceTreeGraphModel.h"
#include "../../CStyleToolkit.h"

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeHistoryArtist
///
/// @description  This class is responsible for painting nodes to the canvas on
///               the screen.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CChanceTreeHistoryArtist
{
public:
    CChanceTreeHistoryArtist( QPaintDevice *canvas );
    ~CChanceTreeHistoryArtist();

    void paintTable( QPainter *painter = NULL );
    void setModel( CChanceTreeGraphModel *model );

private:
    QString moveName( const SChanceTreeNode* node );

    //This is a pointer to the canvas on which the model is to be painted.
    QPaintDevice       *m_canvas;

    //This is a pointer to the model which is to be painted.
    CChanceTreeGraphModel    *m_model;

    //This is a pointer to the style toolkit for easier reference.
    CStyleToolkit      *m_style;
};

#endif /*_CTREEHISTORYARTIST_H_*/
