////////////////////////////////////////////////////////////////////////////////
/// @file         CStyleToolkit.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CStyleToolkit class
///               which stores the style information for how to display nodes,
///               such as font, size, etc.
///
/// comment
////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include "CStyleToolkit.h"

using namespace std;
////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::CStyleToolkit
///
/// @description  This is the default constructor which intializes style
///               variables to the default values.
/// @pre          None
/// @post         Member variables have been initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CStyleToolkit::CStyleToolkit()
{
    m_pen.setCapStyle(Qt::SquareCap);
    m_pen.setJoinStyle(Qt::MiterJoin);
	m_brush.setColor(Qt::white);
    m_brush.setStyle(Qt::SolidPattern);
	loadDefaults();
}

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::~CStyleToolkit
///
/// @description  This is the class destructor and is provided for maintenance
///               purposes.
///
/// @pre          None
/// @post         None
/// @limitations  None
////////////////////////////////////////////////////////////////////////////////
CStyleToolkit::~CStyleToolkit() { }


////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::laodDefaults
///
/// @description     This function sets various parameters of the singleton
///                  instance.
///
/// @pre             None
/// @post            If the preferences.ini file is found the information in
///                  that file is used to set the initial parameters of the
///                  singleton instance.  If the file is not found hard coded
///                  default values are used.
///
/// @return void:    No return information is given
///
/// @limitations     If the hard coded defaults here are changed so must the
///                  the hard coded defaults of the CEditStyleToolkit class.
///
////////////////////////////////////////////////////////////////////////////////
void CStyleToolkit::loadDefaults()
{
	ifstream in("preferences.ini");
	QFont    font;
	QPen     pen;

	if(in.is_open() == 0)
	{
		pen.setWidth(4);
		pen.setStyle(Qt::SolidLine);
		pen.setColor(QColor(Qt::black));
		m_newColor   = Qt::gray;
		m_nodeRadius = 30;
		m_boldWidth  = 4;
		font.setFamily("Arial");
		font.setPointSize(14);
		font.setWeight(QFont::DemiBold);
	}
	else
	{
		int nr, lw, bw;
		int r1, g1, b1;
		int r2, g2, b2;
		char buffer[64];

		in >> nr;
		in >> lw;
		in >> bw;
		in >> r1 >> g1 >> b1;
		in >> r2 >> g2 >> b2;
		in.read(buffer, 64);

		pen.setWidth(lw);
		m_nodeRadius = nr;
		m_boldWidth  = bw;
		pen.setStyle(Qt::SolidLine);
		pen.setColor(QColor(r1,g1,b1));
		m_newColor = QColor(r2,g2,b2);
		font.fromString(buffer);
	}

	m_pen  = pen;
	m_font = font;
}

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::getBoldWidth
///
/// @description  This function returns the extra width of a bold line or edge
///               in pixels.
/// @pre          None
/// @post         None
///
/// @return int:  This is the radius of each node, in pixels.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
int CStyleToolkit::getBoldWidth() const { return m_boldWidth; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::getBrush
///
/// @description     This function returns a copy of the QBrush used in painting
///                  the nodes and spans/edges to the screen.
/// @pre             None
/// @post            None
///
/// @return QBrush:  This is a copy of the QBrush used to paint the nodes and
///                  spans/edges.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
QBrush CStyleToolkit::getBrush() const { return m_brush; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::getFont
///
/// @description     This function returns a copy of the QFont used in painting
///                  the nodes and spans/edges to the screen.
/// @pre             None
/// @post            None
///
/// @return QFont:   This is a copy of the QFont used to paint the nodes and
///                  spans/edges.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
QFont CStyleToolkit::getFont() const { return m_font; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::getNewColor
///
/// @description     This function returns a copy of the QColor used in painting
///                  new objects to the screen.
/// @pre             None
/// @post            None
///
/// @return QColor:  This is a copy of the QColor used to paint new objects to
///                  the screen.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
QColor CStyleToolkit::getNewColor() const { return m_newColor; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::getNodeRadius
///
/// @description  This function returns the radius of each node, in pixels.
/// @pre          None
/// @post         None
///
/// @return int:  This is the radius of each node, in pixels.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
int CStyleToolkit::getNodeRadius() const { return m_nodeRadius; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::getPen
///
/// @description     This function returns a copy of the QPen used in painting
///                  the nodes and spans/edges to the screen.
/// @pre             None
/// @post            None
///
/// @return QPen:    This is a copy of the QPen used to paint the nodes and
///                  spans/edges.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
QPen CStyleToolkit::getPen() const { return m_pen; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::instance
///
/// @description              This function returns a pointer to the only
///                           existing object of this class.  It allows for the
///                           class to be a singleton class.
/// @pre                      None
/// @post                     None
///
/// @return CStyleToolkit *:  This is a pointer to the only object of this
///                           class.
///
/// @limitations              None
///
////////////////////////////////////////////////////////////////////////////////
CStyleToolkit * CStyleToolkit::instance()
{
    static CStyleToolkit toolkit;
    return &toolkit;
}

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::setBoldWidth
///
/// @description    This function sets the extra width used when painting bold
///                 edges or lines.
/// @pre            None
/// @post           The nodeRadius is updated.
///
/// @param width:   This is the new radius to use for painting nodes.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CStyleToolkit::setBoldWidth( int width ) { m_boldWidth = width; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::setBrush
///
/// @description    This function sets the brush to be used to paint the nodes
///                 to the indicated brush.
/// @pre            None
/// @post           The QBrush is updated.
///
/// @param brush:   This is a reference to the new brush to use.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CStyleToolkit::setBrush( QBrush &brush ) { m_brush = brush; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::setFont
///
/// @description    This function sets the font to be used to paint the nodes
///                 to the indicated font.
/// @pre            None
/// @post           The QFont is updated.
///
/// @param font:    This is a reference to the new font to use.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CStyleToolkit::setFont( QFont &font ) { m_font = font; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::setNewColor
///
/// @description    This function sets the color to be used to paint new objects
///                 to the screen before they are fully created.
/// @pre            None
/// @post           The QColor is updated.
///
/// @param color:   This is a reference to the new color to use.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CStyleToolkit::setNewColor( QColor &color ) { m_newColor = color; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::setNodeRadius
///
/// @description     This function sets the node radius used to paint the nodes
///                  to the indicated size.
/// @pre             None
/// @post            The nodeRadius is updated.
///
/// @param radius:   This is the new radius to use for painting nodes.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CStyleToolkit::setNodeRadius( int radius ) { m_nodeRadius = radius; }

////////////////////////////////////////////////////////////////////////////////
/// CStyleToolkit::setPen
///
/// @description    This function sets the pen to be used to paint the nodes
///                 to the indicated pen.
/// @pre            None
/// @post           The QPen is updated.
///
/// @param pen:     This is a reference to the new pen to use.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CStyleToolkit::setPen( QPen &pen ) { m_pen = pen; }
