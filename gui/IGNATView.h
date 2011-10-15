////////////////////////////////////////////////////////////////////////////////
/// @file         IGNATView.h
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the IGNATView class
///               which is an interface class for combining the graph and trace
///               views.
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#ifndef _IGNATVIEW_H_
#define _IGNATVIEW_H_

#include "../gnat.h"
#include <QSplitter>
#include <QString>
#include <QToolBar>

////////////////////////////////////////////////////////////////////////////////
/// IGNATView
///
/// @description  The IGNATView class is a interface for the combination Graph
///               and Trace view. It is based off of the QSplitter.
///
/// @limitations  This is a pure virtual class and no objects of this class
///               may directly be created.
///
////////////////////////////////////////////////////////////////////////////////
class IGNATView : public QSplitter
{
    Q_OBJECT

public:
    IGNATView( QWidget *parent = 0 );
    virtual ~IGNATView();

    virtual QToolBar *getToolBar();
    virtual void loadFromFile( const QString &fileName ) = 0;
    virtual void saveImage( const QString &fileName ) = 0;
    virtual void saveToFile( const QString &fileName ) = 0;
	virtual void signalRepaint(void) = 0;

protected:
    virtual void setupToolBar() = 0;
    QToolBar *m_toolBar;
};

////////////////////////////////////////////////////////////////////////////////
/// CGNATViewFactory
///
/// @description  The CGNATViewFactory class is a factory that returns a
///               IGNATView loaded with the filename passed.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CGNATViewFactory
{
public:
    static IGNATView *createGNATView( const QString &fileName,
                                      QWidget *parent = 0 );
    static IGNATView *createGNATView( PROGRAM_MODE type,
                                      const QString &fileName,
                                      QWidget *parent = 0 );
};

#endif /*_CGNATVIEWFACTORY_H_*/
