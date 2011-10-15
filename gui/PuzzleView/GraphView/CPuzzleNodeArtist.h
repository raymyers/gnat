////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleNodeArtist.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CPuzzleNodeArtist
///               class which is responsible for painting nodes to the screen.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUZZLENODEARTIST_H_
#define _CPUZZLENODEARTIST_H_

#include <QPaintDevice>
#include <QPainter>
#include "../CPuzzleGraph.h"
#include "CPuzzleGraphModel.h"
#include "../../CStyleToolkit.h"

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleNodeArtist
///
/// @description  This class is responsible for painting nodes to the canvas on
///               the screen.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CPuzzleNodeArtist
{
public:
    CPuzzleNodeArtist( QPaintDevice *canvas );
    ~CPuzzleNodeArtist();

    void drawArrow( QPainter *painter, QRect &boundingRect,
                    qreal angleNorthOfEast = 135.0 );
    void paintModel();

    void paintModel( QPainter *painter );

    void paintNode( const SPuzzleNode * node,
                    QPainter *painter );
    void setModel( CPuzzleGraphModel *model );

	void toggleHeuristics(bool b);

private:
    QPaintDevice       *m_canvas;
    CPuzzleGraphModel  *m_model;
    CStyleToolkit      *m_style;

	bool m_showHeuristics;
};

#endif /* _CPUZZLENODEARTIST_H_ */
