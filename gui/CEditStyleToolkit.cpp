////////////////////////////////////////////////////////////////////////////////
/// @file         CEditStyleToolkit.cpp
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
/// comment
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include <QLabel>
#include <QDialog>
#include <fstream>
#include "CStyleToolkit.h"
#include "CEditStyleToolkit.h"
#include "CGNATWindow.h"

using namespace std;

int CEditStyleToolkit::m_refCount = 0;

////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit::CEditStyleToolkit
///
/// @description     Constructor
///
/// @pre             None
/// @post            The class is initialized and the instance counter is
///                  incremented
///
/// @param parent:   Points to a QWidget that is to become the dialogs parent
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
CEditStyleToolkit::CEditStyleToolkit(QWidget *parent) : QDialog(parent)
	{ initialize(); m_refCount++; }


////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit::CEditStyleToolkit
///
/// @description     Destructor
///
/// @pre             None
/// @post            The instance counter is decremented
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
CEditStyleToolkit::~CEditStyleToolkit()
	{ m_refCount--; }


////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit::intialize
///
/// @description     This function is used to initialize the widgets used by the
///                  dialog
///
/// @pre             None
/// @post            The dialog is ready to become active
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CEditStyleToolkit::initialize(void)
{
	QPushButton *button1, *button2;
	QPushButton *button3, *button4;
	QLabel      *label1,  *label2;
	QLabel      *label3;
	QGridLayout *layout;
	QGroupBox   *box;
	QVBoxLayout *main = new QVBoxLayout;

	// Set dialog window title
	setWindowTitle(tr("Preferences"));

	//---------------------------------------------------------------//
	//-- Graphics group box -----------------------------------------//
	box     = new QGroupBox("Nodes and Edges ");
	layout  = new QGridLayout;
	label1  = new QLabel(tr("Node Radius  1 to 100"));
	label2  = new QLabel(tr("Line Width  1 to 50"));
	label3  = new QLabel(tr("Bold Width  1 to 50"));
	button1 = new QPushButton("Standard Color");
	button2 = new QPushButton("Create Color");
	m_sbNodeRadius   = new QSpinBox; m_sbNodeRadius->setRange(1, 100);
	m_sbLineWidth    = new QSpinBox; m_sbLineWidth->setRange (1, 100);
	m_sbBoldWidth    = new QSpinBox; m_sbBoldWidth->setRange (1, 100);
	m_fStandardColor = new QFrame;
	m_fCreateColor   = new QFrame;

	m_fStandardColor->setFrameRect (QRect(0,0,0,0));
	m_fCreateColor->setFrameRect   (QRect(0,0,0,0));
	m_fCreateColor->setFrameStyle  (QFrame::Panel|QFrame::Sunken);
	m_fStandardColor->setFrameStyle(QFrame::Panel|QFrame::Sunken);
	m_fCreateColor->setAutoFillBackground  (true);
	m_fStandardColor->setAutoFillBackground(true);

	layout->addWidget(label1,  0,0); layout->addWidget(m_sbNodeRadius,   0,2);
	layout->addWidget(label2,  1,0); layout->addWidget(m_sbLineWidth,    1,2);
	layout->addWidget(label3,  2,0); layout->addWidget(m_sbBoldWidth,    2,2);
	layout->addWidget(button1, 3,0); layout->addWidget(m_fStandardColor, 3,2);
	layout->addWidget(button2, 4,0); layout->addWidget(m_fCreateColor,   4,2);
	box->setLayout(layout); main->addWidget(box);

	connect(button1, SIGNAL(clicked()), this, SLOT(standardColorClicked()));
	connect(button2, SIGNAL(clicked()), this, SLOT(createColorClicked()));
	//---------------------------------------------------------------//

	//---------------------------------------------------------------//
	//-- Text group box ---------------------------------------------//
	box           = new QGroupBox("Text ");
	layout        = new QGridLayout;
	m_lFontSample = new QLabel(tr("AaBbYyZz"));
	button1       = new QPushButton("Change Font");
	layout->addWidget(m_lFontSample, 0,0);
	layout->addWidget(button1,       1,0);
	box->setLayout(layout); main->addWidget(box);

	connect(button1, SIGNAL(clicked()), this, SLOT(changeFontClicked()));
	//---------------------------------------------------------------//

	//---------------------------------------------------------------//
	//-- Changes group box ------------------------------------------//
	box     = new QGroupBox("Changes ");
	layout  = new QGridLayout;
	button1 = new QPushButton("Apply");	   layout->addWidget(button1, 0, 0);
	button2 = new QPushButton("OK");	   layout->addWidget(button2, 1, 0);
	button3 = new QPushButton("Reset"); layout->addWidget(button3, 0, 1);
	button4 = new QPushButton("Cancel");   layout->addWidget(button4, 1, 1);
	box->setLayout(layout); main->addWidget(box);

	connect(button1, SIGNAL(clicked()), this, SLOT(applyClicked()));
	connect(button2, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(button3, SIGNAL(clicked()), this, SLOT(defaultsClicked()));
    connect(button4, SIGNAL(clicked()), this, SLOT(close()));
	//---------------------------------------------------------------//

	setAttribute(Qt::WA_DeleteOnClose);	// Tell Qt to delete class instance when
	                                    // the user closes the dialog
	getStyleToolkit();                  // Initialize widges to current values
	setLayout(main);                    //
}


////////////////////////////////////////////////////////////////////////////////
/// CEditTyleToolkit::getStyleToolkit
///
/// @description     This function collects information from the CStyleToolkit
///                  class and uses that information to set the dialog's widgets
///                  to the appropriate values
///
/// @pre             None
/// @post            The state of the dialog's widgets and CStyleToolkit
///                  parameters are the same.
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CEditStyleToolkit::getStyleToolkit()
{
	QPalette       pal;
	CStyleToolkit *kit = CStyleToolkit::instance();

	// Fetch style parameters
	m_iNodeRadius    = kit->getNodeRadius();
	m_iLineWidth     = kit->getPen().width();
	m_iBoldWidth     = kit->getBoldWidth();
	m_cStandardColor = kit->getPen().color();
	m_cCreateColor   = kit->getNewColor();
	m_fFont          = kit->getFont();

	// Set dialog widgets
	m_sbNodeRadius->setValue(m_iNodeRadius);
	m_sbLineWidth->setValue(m_iLineWidth);
	m_sbBoldWidth->setValue(m_iBoldWidth);
	pal = m_fStandardColor->palette();
	pal.setColor(QPalette::Window, m_cStandardColor);
	m_fStandardColor->setPalette(pal);
	pal = m_fCreateColor->palette();
	pal.setColor(QPalette::Window, m_cCreateColor);
	m_fCreateColor->setPalette(pal);
	m_lFontSample->setFont(m_fFont);
}


////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit::standardColorClicked
///
/// @description     This function is called when the user clicks on the
///                  "Standard Color" push button.
///
/// @pre             None
/// @post            A stock Qt dialog window appears allowing the user to
///                  select a color.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CEditStyleToolkit::standardColorClicked(void)
{
	QColor color = QColorDialog::getColor(m_cStandardColor, this);
	if(color.isValid() == true)
	{
		m_cStandardColor = color;
		QPalette pal = m_fStandardColor->palette();
		pal.setColor(QPalette::Window, m_cStandardColor);
		m_fStandardColor->setPalette(pal);
	}
} //---------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit::createColorClicked
///
/// @description     This function is called when the user clicks on the
///                  "Create Color" push button.
///
/// @pre             None
/// @post            A stock Qt dialog window appears allowing the user to
///                  select a color.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CEditStyleToolkit::createColorClicked(void)
{
	QColor color = QColorDialog::getColor(m_cCreateColor, this);
	if(color.isValid() == true)
	{
		m_cCreateColor = color;
		QPalette pal = m_fCreateColor->palette();
		pal.setColor(QPalette::Window, m_cCreateColor);
		m_fCreateColor->setPalette(pal);
	}
}


////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit::changeFontClicked
///
/// @description     This function is called when the user clicks on the
///                  "Change Font" push button.
///
/// @pre             None
/// @post            A stock Qt dialog window appears allowing the user to
///                  select a font.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CEditStyleToolkit::changeFontClicked(void)
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, m_fFont, this);
	if(ok == true)
	{
		m_fFont = font;
		CStyleToolkit::instance()->setFont(m_fFont);
		m_lFontSample->setFont(m_fFont);
	}
} //---------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit::applyClicked
///
/// @description     This funtion is called when the user clicks the "apply"
///                  push button.
///
/// @pre             None
/// @post            The current state of the dialog's widgets are used to set
///                  the corresponding parameters of the CStyleToolkit class.
///                  Additionaly, the set parameters are written to a file
///                  (preferences.ini) which is used by the CStyleToolkit class
///                  on application startup to load the users preferences.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CEditStyleToolkit::applyClicked(void)
{
	CStyleToolkit *kit = CStyleToolkit::instance();
	QPen           pen = kit->getPen();

	// Fetch settings
	m_iNodeRadius = m_sbNodeRadius->value();
	m_iLineWidth  = m_sbLineWidth->value();
	m_iBoldWidth  = m_sbBoldWidth->value();
	// m_cStanderColor:  Does not need to be fetched
	// m_cCreateColor:   Does not need to be fetched
	// m_fFont:          Does not need to be fetched

	// Set settings
	pen.setWidth(m_iLineWidth);
	pen.setColor(m_cStandardColor);
	kit->setNodeRadius(m_iNodeRadius);
	kit->setBoldWidth (m_iBoldWidth);
	kit->setNewColor  (m_cCreateColor);
	kit->setPen       (pen);
	kit->setFont      (m_fFont);

	// Write setting to the preferences file
	ofstream out("preferences.ini");
	out << m_iNodeRadius << "\n";
	out << m_iLineWidth  << "\n";
	out << m_iBoldWidth  << "\n";
	out << m_cStandardColor.red()   << " ";
	out << m_cStandardColor.green() << " ";
	out	<< m_cStandardColor.blue()  << "\n";
	out << m_cCreateColor.red()   << " ";
	out << m_cCreateColor.green() << " ";
	out << m_cCreateColor.blue()  << "\n";
	out << (const char*)m_fFont.key().toAscii() << "\n";
	out.close();

	// Tell parent window to repaint itself
	parentWidget()->repaint(0,0,-1,-1);
	CGNATWindow *ptr = (CGNATWindow*)parentWidget();
	ptr->signalRepaint();

} //---------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit::okClicked
///
/// @description     This function is called when the user clicks the "OK" push
///                  button.
///
/// @pre             None
/// @post            applyClicked() is called and then the dialog is closed
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CEditStyleToolkit::okClicked(void)
{
	applyClicked();
	close();
}


////////////////////////////////////////////////////////////////////////////////
/// CEditStyleToolkit::defaultsClicked
///
/// @description     This function is called when the user clicks the defaults
///                  push button.
///
/// @pre             None
/// @post            Default values are applied to the dialog's widgets
///
/// @limitations     The defaults hard coded here are duplicated in the
///                  CStyleToolkit.  A change in the defaults of one class will
///                  not be reflected in the other class.  If you change the
///                  defaults here you must change the defaults in the
///                  CStyleToolkit class (the reverse holds as well).
///
////////////////////////////////////////////////////////////////////////////////
void CEditStyleToolkit::defaultsClicked()
{
	QPalette pal;
	QFont    font;

	// Reconstruct defaults
	m_iNodeRadius    = 30;
	m_iLineWidth     = 4;
	m_iBoldWidth     = 4;
	m_cStandardColor = QColor(Qt::black);
	m_cCreateColor   = QColor(Qt::gray);
    font.setFamily("Arial");
    font.setPointSize(14);
    font.setWeight(QFont::DemiBold);
	m_fFont          = font;

	// Set dialog widgets
	m_sbNodeRadius->setValue(m_iNodeRadius);
	m_sbLineWidth->setValue(m_iLineWidth);
	m_sbBoldWidth->setValue(m_iBoldWidth);
	pal = m_fStandardColor->palette();
	pal.setColor(QPalette::Window, m_cStandardColor);
	m_fStandardColor->setPalette(pal);
	pal = m_fCreateColor->palette();
	pal.setColor(QPalette::Window, m_cCreateColor);
	m_fCreateColor->setPalette(pal);
	m_lFontSample->setFont(m_fFont);
}

