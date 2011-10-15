////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeNodeArtist.h
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
#ifndef _CCHANCETREENODEARTIST_H_
#define _CCHANCETREENODEARTIST_H_

#include <QPaintDevice>
#include <QPainter>
#include "../CChanceTreeGraph.h"
#include "CChanceTreeGraphModel.h"
#include "../../CStyleToolkit.h"

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeNodeArtist
///
/// @description  This class is responsible for painting nodes to the canvas on
///               the screen.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CChanceTreeNodeArtist
{
public:
    CChanceTreeNodeArtist( QPaintDevice *canvas );
    ~CChanceTreeNodeArtist();

    void paintModel( QPainter *painter = NULL );
    void paintModelHelper( SChanceTreeNode* node, QPainter *painter );
    void paintNode( const SChanceTreeNode* node, QPainter *painter );
    void paintChanceNode( const SChanceTreeNode* node, QPainter *painter );
    void setModel( CChanceTreeGraphModel *model );
    void toggleChanceHeuristics(bool b);
    void toggleMinMaxHeuristics(bool b);

private:
    QPoint paintSpanNumber( QPoint nodeFromPos, QPoint nodeToPos, qreal num, QPainter *painter);
    qreal angleNorthOfEast( QPoint nodeFromPos, QPoint nodeToPos );

    //This is a pointer to the canvas on which the model is to be painted.
    QPaintDevice       *m_canvas;

    //This is a pointer to the model which is to be painted.
    CChanceTreeGraphModel    *m_model;

    //This is a pointer to the style toolkit for easier reference.
    CStyleToolkit      *m_style;

    bool m_showChanceHeuristics;
    bool m_showMinMaxHeuristics;
};

void drawTriangle( const QPoint center, const int radius, const bool up, QPainter *painter );

#endif /*_CTREENODEARTIST_H_*/
