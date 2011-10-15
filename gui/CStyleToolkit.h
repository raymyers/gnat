////////////////////////////////////////////////////////////////////////////////
/// @file         CStyleToolkit.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CStyleToolkit class
///               which stores the style information for how to display nodes,
///               such as font, size, etc.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CSTYLETOOLKIT_H_
#define _CSTYLETOOLKIT_H_


#include <QPen>
#include <QBrush>
#include <QFont>

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit
///
/// @description  This class is a singleton class which stores the style
///               information for how to display nodes, such as font, size,
///               etc.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CStyleToolkit
{
public:
    ~CStyleToolkit();
    static CStyleToolkit * instance();
    int    getBoldWidth() const;
    QBrush getBrush() const;
    QFont  getFont() const;
    QColor getNewColor() const;
    int    getNodeRadius() const;
    QPen   getPen() const;

	void loadDefaults();

    void setBoldWidth( int width );
    void setBrush( QBrush &brush );
    void setFont( QFont &font );
    void setNewColor( QColor &color );
    void setNodeRadius( int radius );
    void setPen( QPen &pen );

private:

    //The constructor is made private to ensure that only a single instance
    //of the class can be created through the instance function.
    CStyleToolkit();

    //This variable stores the pen used to paint objects to the screen.
    QPen   m_pen;

    //This variable stores the color to be used while still creating objects.
    QColor m_newColor;

    //This variable stores the brush used to paint objects to the screen.
    QBrush m_brush;

    //This variable stores the font used to write text to the screen.
    QFont  m_font;

    //This variable stores the radius of the nodes.
    int    m_nodeRadius;

    //This variable stores the additional pen width for making nodes bold.
    int    m_boldWidth;
};

#endif /*_CSTYLETOOLKIT_H_*/
