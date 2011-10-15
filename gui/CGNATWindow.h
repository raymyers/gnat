////////////////////////////////////////////////////////////////////////////////
/// @file         CGNATWindow.h
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CGNATWindow class
///               which is the primary GUI class controlling the menubar,
///               tool bars, and contents.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CGNATWINDOW_H_
#define _CGNATWINDOW_H_

#include "../gnat.h"
#include "IGNATView.h"
#include <QMainWindow>

class IGNATView;
class QAction;
class QActionGroup;
class QMenuBar;
class QStackedWidget;
class QFrame;
class QPushButton;
class QToolButton;
class QScrollArea;

////////////////////////////////////////////////////////////////////////////////
/// CGNATWindow
///
/// @description  This class is the main dialog. It manages the menubars,
///               toolbars, and contents. The contents are stored in a
///               QStackedWidget showing different widgets as the user switches
///               between views.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CGNATWindow : public QMainWindow
{
    Q_OBJECT

public:

    CGNATWindow( QWidget *parent=0, Qt::WFlags flags=0 );
    virtual ~CGNATWindow();

	void signalRepaint(void)
		{ if(m_view) m_view->signalRepaint(); }

protected:

    void loadFile(QString fileName);
    void saveFile(QString fileName);

private slots:

    void newClicked();
    void openClicked();
    void openRecentClicked();
    void saveClicked();
    void saveAsClicked();
    void saveImageClicked();
	void preferencesClicked();
    void aboutClicked();

private:
    void setupActions();
    void setupMenuBar();
    void setupToolBar();
    void setupContent();
    void setupDefaultView();
    void setupSettings();

    void loadSettings();
    void saveSettings();

    void enableFileDependantActions( bool on = true );
    void launchWebTool( const QString& url );

    // Actions
    QAction    *m_newAction;
    QAction    *m_openAction;
    QAction    *m_saveAction;
    QAction    *m_saveAsAction;
    QAction    *m_saveImageAction;
	QAction    *m_preferencesAction;
    QAction    *m_exitAction;
    QAction    *m_aboutAction;

    // Drop Down Menus
    QMenuBar   *m_menuBar;
    QMenu      *m_fileMenu;
	QMenu      *m_settingsMenu;
    QMenu      *m_helpMenu;

    // Toolbar
    QToolBar   *m_mainToolBar;
    QToolBar   *m_traceToolBar;

    // Contents
    QFrame         *m_default;
    QToolButton    *m_defNew;
    QToolButton    *m_defOpen;
    QToolButton    *m_defRecent;

    IGNATView      *m_view;

    // Attributes
    QString         m_curFileName;
    QString         m_fileFilters;
    QString         m_filePath;
};

#endif /*_CGNATWINDOW_H_*/
