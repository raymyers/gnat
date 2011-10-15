////////////////////////////////////////////////////////////////////////////////
/// @file         CEditStyleToolkit.h
///
/// @author       Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definitions of the CEditStyleToolkit
///               class which is a dialog box accessed by the end user through
///               Settings -> Preferences.  The class functions as a interface
///               between the end user and the CStyleToolkit singleton instance
///               allowing the user to change various parameters that effect
///               how nodes, edges and so on are displayed.
///               Qt Version: 4.1.0 Open Source
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _CEDITSTYLETOOLKIT_H_
#define _CEDITSTYLETOOLKIT_H_

#include <QtGui>
#include <QLabel>
#include <QDialog>
#include "CStyleToolkit.h"
#include "CEditStyleToolkit.h"

////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit
/// 
/// @description  This class encapsulates a dialog box accessed by the end user
///               through Settings -> Preferences.  The class functions as an
///               interface between the end user and the CStyleToolkit singleton
///               instance allowing the user to change various parameters that
///               effect how nodes, edges and so on are displayed.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CEditStyleToolkit : public QDialog
{
	Q_OBJECT

public:

	CEditStyleToolkit (QWidget *parent);
	~CEditStyleToolkit(void);

	static int refCount(void)
		{ return m_refCount; }

private slots:

	void standardColorClicked();    // Button actions...
	void createColorClicked();      // ...
	void changeFontClicked();       // ...
	void applyClicked();            // ...
	void okClicked();               // ...
    void defaultsClicked();	        // ...

private:

	void initialize     (void); // Create widgets
	void getStyleToolkit(void); // Sync widgets to CStyleToolkit values

	int    m_iNodeRadius;       // Node radius
	int    m_iLineWidth;        // Node / edge width
	int    m_iBoldWidth;        // Node bold width
	QColor m_cStandardColor;    // Current node / edge / text color
	QColor m_cCreateColor;	    // Current edge creation value
	QFont  m_fFont;	            // Current font

	QSpinBox *m_sbNodeRadius;   // Widgets...
	QSpinBox *m_sbLineWidth;    // ...
	QSpinBox *m_sbBoldWidth;    // ...
	QFrame   *m_fStandardColor; // ...
	QFrame   *m_fCreateColor;   // ...
	QLabel   *m_lFontSample;    // ...

	// This is used to track the number of class instances
	static int m_refCount;
}; //--------------------------------------------------------------------------//

#endif // _CEDITSTYLETOOLKIT_H

