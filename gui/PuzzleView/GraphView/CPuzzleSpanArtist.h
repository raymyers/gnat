////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleSpanArtist.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleSpanArtist
///               class which is responsible for painting spans to the screen.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLESPANARTIST_H_
#define _CPUZZLESPANARTIST_H_

#include <QPaintDevice>
#include <QPainter>
#include <QBitmap>
#include "../CPuzzleGraph.h"
#include "CPuzzleGraphModel.h"
#include "../../CStyleToolkit.h"

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleSpanArtist
///
/// @description  This class is responsible for painting spans to the canvas on
///               the screen.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleSpanArtist
{
public:
    CPuzzleSpanArtist( QPaintDevice *canvas );
    ~CPuzzleSpanArtist();

    void paintMask( const SPuzzleSpan * span, int fromNode );
    void paintModel();
    void paintModel( QPainter *painter );
    void paintSpan( const SPuzzleSpan * span, QPainter *painter );

    void setModel( const CPuzzleGraphModel *model );

private:
    void paintStraightEdge( QPoint nodeFromPos, QPoint nodeToPos,
                            QPainter *painter );
    void paintStraightArrow( QPoint nodeFromPos, QPoint nodeToPos,
                             QPainter *painter );
    void paintStraightNumber( QPoint nodeFromPos, QPoint nodeToPos, qreal num,
                              QPainter *painter);
    void paintCurvedEdge( QPoint nodeFromPos, QPoint nodeToPos,
                          QPainter *painter );
    void paintCurvedArrow( QPoint nodeFromPos, QPoint nodeToPos,
                           QPainter *painter );
    void paintCurvedNumber( QPoint nodeFromPos, QPoint nodeToPos, qreal num,
                            QPainter *painter);

    qreal angleNorthOfEast( QPoint nodeFromPos, QPoint nodeToPos );

    QPaintDevice               *m_canvas;
    const CPuzzleGraphModel    *m_model;
    CStyleToolkit              *m_style;
};

#endif /* _CPUZZLESPANARTIST_H_ */
