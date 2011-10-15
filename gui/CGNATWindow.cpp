////////////////////////////////////////////////////////////////////////////////
/// @file         CGNATWindow.cpp
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CGNATWindow class,
///               which is the main window of the GNAT application.
//
/// Modified by Kyle Owen on March 04, 2006
//
//  Added "Setting" to the menu bar
//  Added "Preferences" to the setting menu
//
//  Settings --> Preferences launches the CEditStyleToolkit dialog
///
////////////////////////////////////////////////////////////////////////////////

#include "CGNATWindow.h"
#include "IGNATView.h"
#include "CStyleToolkit.h"
#include "CEditStyleToolkit.h"
#include <QAction>
#include <QActionGroup>
#include <QMenuBar>
#include <QToolBar>
#include <QStackedWidget>
#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QToolButton>
#include <QScrollArea>
#include <QMessageBox>
#include <QDialog>
#include <QFileDialog>
#include <QCoreApplication>
#include <QSettings>
#include <QProcess>

#include <iostream>
using std::cerr;

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::CGNATWindow
///
/// @description    This is the constructor for the CGNATWindow class and
///                 initializes class variables.
/// @pre            None
/// @post           Class variables are initialized.
///
/// @param parent:  This is a pointer to the parent object.
/// @param flags:   This is the set of flags so the main window knows what
///                 options to use.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CGNATWindow::CGNATWindow( QWidget *parent, Qt::WFlags flags )
: QMainWindow( parent, flags )
{
    setupActions();
    setupMenuBar();
    setupToolBar();
    setupContent();

    enableFileDependantActions( false );

    m_curFileName = "";
    m_fileFilters = tr("GNAT files (*.gnat)\n"
                       "All files (*.*)");

    setupSettings();
    loadSettings();
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::~CGNATWindow
///
/// @description  This is the destructor for the CGNATWindow class and saves
///               the program settings to the registery (or equivalent in UNIX
///               and Macintosh environments).
/// @pre          None
/// @post         Program settings are saved.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CGNATWindow::~CGNATWindow()
{
    saveSettings();
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::aboutClicked
///
/// @description  This function presents the user with the about box for the
///               GNAT application.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::aboutClicked()
{
    QString string("GNAT Version ");
    string.append(QString::number(PROGRAM_VERSIONNUM));
    string.append("\nGraphical AI Tracer (The 'N' stand for nothing!)\n"
                  "Program Written by and Copyright Matt Dissinger,"
                  "Bob Buehler, Kyle Owen, and Ray Myers.\n");
    QMessageBox::about( this, "About GNAT", string );
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::enableFileDependantActions
///
/// @description  This function enables or disables the QActions which are
///               dependant on having an open file.
/// @pre          None
/// @post         None
///
/// @param on:    This variable is used to indicate if the QActions are being
///               enabled or disabled.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::enableFileDependantActions( bool on )
{
    m_saveAction->setEnabled(on);
    m_saveAsAction->setEnabled(on);
    m_saveImageAction->setEnabled(on);
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::launchWebTool
///
/// @description  This function launches attempts to launch the default browser
///               on the user's current platform.
/// @pre          None
/// @post         None
///
/// @param url:   This is the address that the browser should be pointed at on
///               success.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::launchWebTool( const QString& url )
{
    QProcess* web = new QProcess( this );
    bool supportedPlatform = true;
    bool executeProcess = false;

#if defined(Q_OS_WIN32)
/*#include <windows.h>

   //Trolltech's Windows Version Independent Default Browser Launch
    QT_WA(
        {ShellExecute( winId(), 0, (TCHAR*)url.ucs2(), 0, 0, SW_SHOWNORMAL );},
        {ShellExecuteA( winId(), 0, url.local8Bit(), 0, 0, SW_SHOWNORMAL );} );
*/

    supportedPlatform = false;
    executeProcess = false;
#elif defined(Q_OS_LINUX)
#include <stdio.h>
#include <stdlib.h>
/*
    //get : delimited list of all perferred browsers
    QCString allBrowsers = getenv( "BROWSER" );

    if( allBrowsers.isEmpty() ) //if it is empty we're hosed
    {
        qDebug( "No $BROWSER environment varialbe set." );
        supportedPlatform = false;
        executeProcess = false;
    }
    else
    {
        //split the list
        QStringList browsers = QStringList::split( ":" , allBrowsers );

        //grab the first (default)
        QString browser = *browsers.begin();

        if( browser.contains( "kfmclient" ) )
        {
            web->addArgument( "kfmclient" );
            web->addArgument( "exec" );
        }
        else
        {
            web->addArgument( browser );
        }

        web->addArgument( url );

        executeProcess = true;
    }
*/
#elif defined(Q_OS_MACX)
    //Trolltech's Mac OSX Version Independent Default Browser Launch
    web->addArgument( "/usr/bin/open" );
    web->addArgument( url );
    executeProcess = true;
#else
    supportedPlatform = false;
    executeProcess = false;
#endif

    int returnValue = 0;
    QObject::connect( web, SIGNAL( processExited() ),
                      web, SLOT( deleteLater() ) );

    if( executeProcess )
        web->start( "webTool" );

    if( returnValue || !supportedPlatform )
        QMessageBox::information( this, "Mapimg",
                    QString("Unable to launch web browser to %1").arg( url ) );

    return;
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::loadFile
///
/// @description          This function opens the indicated file and opens the
///                       proper view associated with the file's view mode.
/// @pre                  None
/// @post                 The given file is opened, if it exists and is a valid
///                       file.
///
/// @param openFileName:  This is the name of the file to be opened.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::loadFile( QString openFileName )
{
    //Create the view needed for the given filename.
    IGNATView *newView =
        CGNATViewFactory::createGNATView( openFileName, this );

    //Make sure the file name was valid and contained a valid GNAT file.
    if( newView == NULL )
        return;

    //If there was a file open, remove the toolbar used by it.
    if( m_view != NULL )
        removeToolBar( m_view->getToolBar() );

    //Add the new view appropriate toolbar.
    addToolBar( newView->getToolBar() );

    //Delete the old view if it existed.
    if( m_view )
        delete m_view;
    m_view = newView;

    enableFileDependantActions();

//    m_default->setShown( false );
    m_view->setShown( true );
    setCentralWidget( m_view );

    m_curFileName = openFileName;
    m_filePath = openFileName.left( openFileName.lastIndexOf("/") );

    QSettings settings;
    settings.setValue( "recentfile", m_curFileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::loadSettings
///
/// @description  This function loads the program settings from the registry or
///               equivalent location for UNIX and Mac operating systems.
/// @pre          None
/// @post         The old program settings are loaded, if found.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::loadSettings()
{
    QSettings settings;

    resize( QSize(settings.value("size").toSize()).expandedTo(minimumSizeHint()) );
    if( settings.value("maximized").toBool() )
        showMaximized();

    m_filePath = settings.value( "filepath" ).toString();
    m_curFileName = settings.value( "recentfile" ).toString();
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::newClicked
///
/// @description  This function is called when the user clicks the New option
///               to create a new file.
/// @pre          None
/// @post         A new file of the specified type is created.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::newClicked()
{
    //Prompt the user for which type of file they would like to create.
    int choice = QMessageBox::question(this,
        "New...",
        "Please choose a style:",
        "Single Agent", "Adversarial","Chance Tree (Experimental)");
    
    choice++; // Conveniently the choice values + 1 equal the
              // corresponding PROGRAM_MODE values as of 10/3/05

    //Create the appropriate file view.
    IGNATView *newView =
        CGNATViewFactory::createGNATView( (PROGRAM_MODE) choice,
                                         QString(), this );
    //Make sure a proper view was created.
    if( newView == NULL )
        return;

    //Remove any current view that was open.
    //cerr << "1\n"; 

    if( m_view != NULL )
        removeToolBar( m_view->getToolBar() );
    addToolBar( newView->getToolBar() );

    //Delete any existing view that was currently open.
    //cerr << "2\n"; 

    if( m_view )
        delete m_view;
    //cerr << "3\n"; 

    m_view = newView;

    enableFileDependantActions();

//    m_default->setShown( false );
    m_view->setShown( true );
    setCentralWidget( m_view );

    m_curFileName = "";
    //cerr << "4\n"; 

}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::openClicked
///
/// @description  This function is run when the user clicks the Open option to
///               open an existing file.  It prompts the user to locate the file
///               they wish to open.
/// @pre          None
/// @post         The file is loaded, if a valid file was selected.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::openClicked()
{
    QString openFileName( QFileDialog::getOpenFileName(this, "Open...",
                                                       m_filePath,
                                                       m_fileFilters) );

    if (!openFileName.isEmpty() && QFile::exists(openFileName))
        loadFile( openFileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::openRecentClicked
///
/// @description  This function opens the most recently opened file, if one
///               exists.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::openRecentClicked()
{
    if( m_curFileName != "" )
        loadFile( m_curFileName );
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::saveAsClicked
///
/// @description  This function prompts the user for a name and location to save
///               the currently active file to.  It then saves the file to the
///               given location, if it is valid.
/// @pre          None
/// @post         The active file is saved to the given location, if valid.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::saveAsClicked()
{
    //Make sure a file is currently open.
    if( m_view == NULL )
        return;

    //Get the save file name from the user.
    QString saveFileName( QFileDialog::getSaveFileName(this, "Save...",
                                                       m_filePath,
                                                       m_fileFilters ) );

    //Make sure the file was valid.
    if (!saveFileName.isEmpty())
    {
        //Append the file extension if the user didn't add it themselves.
        if ( !saveFileName.endsWith(".gnat", Qt::CaseInsensitive ) )
            saveFileName.append( ".gnat" );
        m_curFileName = saveFileName;
        m_view->saveToFile(m_curFileName);
        m_filePath = saveFileName.left( saveFileName.lastIndexOf("/") );

        QSettings settings;
        settings.setValue( "recentfile", m_curFileName );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::saveClicked
///
/// @description  This function saves the open file and prompts the user with a
///               Save As box if the file has not been previously saved.
/// @pre          None
/// @post         The file is saved, unless it has not been previously saved and
///               the user selected to cancel.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::saveClicked()
{
    //Make sure there is a file open.
    if( m_view == NULL )
        return;

    //Prompt the user with the Save As box if the file has not been previously
    //saved.
    if (m_curFileName.isEmpty())
        saveAsClicked();
    else
        m_view->saveToFile(m_curFileName);
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::saveImageClicked
///
/// @description  This function is called when the user selects to save an image
///               of the current graph view.
/// @pre          None
/// @post         A PNG image is created of the graph view to the file specified
///               by the user, if valid.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::saveImageClicked()
{
    //Make sure a file is currently open.
    if ( m_view == NULL )
        return;

    //Get the name of the file the user wants to save the image to.
    QString saveFileName( QFileDialog::getSaveFileName( this, "Save Image...",
                          m_filePath, tr("PNG Files (*.png)") ) );

    //Make sure the file name given is valid.
    if ( !saveFileName.isEmpty() )
    {
        //Append the PNG extension if the user didn't add it if the user didn't.
        if ( !saveFileName.endsWith(".png", Qt::CaseInsensitive ) )
            saveFileName.append( ".png" );
        m_view->saveImage( saveFileName );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::preferencesClicked
///
/// @description     This function is called when the user selects Settings -->
///                  Preferences from the menu bar.
///
/// @pre             None
/// @post            The CEditStyleToolkit dialog is initialized and launched
///                  if an instance does not exist.  If an instance does exist
///                  that instance is made visible and active.
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::preferencesClicked()
{
    static CEditStyleToolkit *dialog;

    if(CEditStyleToolkit::refCount() == 0)
        dialog = new CEditStyleToolkit((QWidget*)this);

    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::saveSettings
///
/// @description  This function saves the current program settings to the
///               registry or equivalent in UNIX and Mac operating systems.
/// @pre          None
/// @post         The program settings are saved to the registry or equivalent
///               for UNIX and Mac operating systems.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::saveSettings()
{
    QSettings settings;

    if( isMaximized() )
        settings.setValue( "maximized", true );
    else
    {
        settings.setValue( "size", size() );
        settings.setValue( "maximized", false );
    }

    settings.setValue( "filepath", m_filePath );
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::setupActions
///
/// @description  This function creates the QActions used for the main menu
///               options.
/// @pre          None
/// @post         The QActions for the main menus are created.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::setupActions()
{
    // File Menu type Actions
    m_newAction = new QAction( QIcon(":/new.png"), "New...", this );
    connect( m_newAction, SIGNAL(activated()), this, SLOT(newClicked()) );

    m_openAction = new QAction( QIcon(":/open.png"), "Open...", this );
    connect( m_openAction, SIGNAL(activated()), this, SLOT(openClicked()) );

    m_saveAction = new QAction( QIcon(":/save.png"), "Save", this );
    connect( m_saveAction, SIGNAL(activated()), this, SLOT(saveClicked()) );

    m_saveAsAction = new QAction( "Save As...", this );
    connect( m_saveAsAction, SIGNAL(activated()), this, SLOT(saveAsClicked()) );

    m_saveImageAction = new QAction( "Save Image...", this );
    connect( m_saveImageAction, SIGNAL(activated()), this,
             SLOT(saveImageClicked()));

    m_exitAction = new QAction( QIcon(":/exit.png"), "Exit", this );
    connect( m_exitAction, SIGNAL(activated()), this, SLOT(close()) );

    m_preferencesAction = new QAction("Preferences...", this);
    connect(m_preferencesAction, SIGNAL(activated()), this, SLOT(preferencesClicked()));

    m_aboutAction = new QAction( "About...", this );
    connect( m_aboutAction, SIGNAL(activated()), this, SLOT(aboutClicked()));
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::setupContent
///
/// @description  This function sets up the main window to hold the main program
///               contents.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::setupContent()
{
    setupDefaultView();
    m_view = NULL;

    setContentsMargins(4,4,4,4);
    setCentralWidget( m_default );
}

////////////////////////////////////////////////////////////////////////////////
/// <function name>
///
/// @description  This function sets up the initial display which gives the user
///               the option to create a new file, open an existing one, or open
///               the most recent file.
/// @pre          None
/// @post         The default view is created.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::setupDefaultView()
{
    m_default = new QFrame(this);

    m_defNew = new QToolButton(m_default);
    m_defNew->setText("New");
    m_defNew->setIcon(QIcon(":/new_large.png"));
    m_defNew->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_defNew->setIconSize(QSize(70,70));
    m_defNew->setAutoRaise(true);
    connect( m_defNew, SIGNAL(clicked()), this, SLOT(newClicked()));

    m_defOpen = new QToolButton(m_default);
    m_defOpen->setText("Open");
    m_defOpen->setIcon(QIcon(":/open_large.png"));
    m_defOpen->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_defOpen->setIconSize(QSize(70,70));
    m_defOpen->setAutoRaise(true);
    connect( m_defOpen, SIGNAL(clicked()), this, SLOT(openClicked()));

    m_defRecent = new QToolButton(m_default);
    m_defRecent->setText("Most Recent");
    m_defRecent->setIcon(QIcon(":/recent_large.png"));
    m_defRecent->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_defRecent->setIconSize(QSize(70,70));
    m_defRecent->setAutoRaise(true);
    connect( m_defRecent, SIGNAL(clicked()), this, SLOT(openRecentClicked()));

    QGridLayout *defaultLayout = new QGridLayout;
    defaultLayout->addWidget(m_defNew, 0,0,1,1,
                             Qt::AlignRight | Qt::AlignVCenter);
    defaultLayout->addWidget(m_defOpen, 0,1,1,1,
                             Qt::AlignVCenter);
    defaultLayout->addWidget(m_defRecent, 0,2,1,1,
                             Qt::AlignLeft | Qt::AlignVCenter);

    m_default->setLayout(defaultLayout);
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::setupMenuBar
///
/// @description  This function sets up the the main menu bars.
/// @pre          None
/// @post         The main menus are created with the appropriate items on them.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::setupMenuBar()
{
    m_menuBar = menuBar();

    m_fileMenu = m_menuBar->addMenu("File");
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addAction(m_saveImageAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_settingsMenu = m_menuBar->addMenu("Settings");
    m_settingsMenu->addAction(m_preferencesAction);

    m_helpMenu = m_menuBar->addMenu("Help");
    m_helpMenu->addAction(m_aboutAction);
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::setupSettings
///
/// @description  This function sets the settings needed to store the
///               application settings to the registry or equivalent in UNIX
///               and Mac operating systems.
/// @pre          None
/// @post         The settings values are set.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::setupSettings()
{
    QCoreApplication::setOrganizationName("UMR");
    QCoreApplication::setOrganizationDomain("umr.edu");
    QCoreApplication::setApplicationName("GNAT");
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow::setupToolBar
///
/// @description  This function sets up the default toolbar which has the
///               new, open, and save options on it.
/// @pre          None
/// @post         The default toolbar is created.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CGNATWindow::setupToolBar()
{
    setIconSize( QSize( 25, 25 ) );

    m_mainToolBar = addToolBar("Main");
    m_mainToolBar->addAction(m_newAction);
    m_mainToolBar->addAction(m_openAction);
    m_mainToolBar->addAction(m_saveAction);
}
