////////////////////////////////////////////////////////////////////////////////
/// @file         IGNATView.cpp
///
/// @author       Bob Buehler and Matt Dissinger
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the IGNATView class which
///               is an interface class for combining the graph and trace views.
///
////////////////////////////////////////////////////////////////////////////////
#include "IGNATView.h"
#include "PuzzleView/CPuzzleView.h"
#include "PuzzleView/CPuzzleGraph.h"
#include "TreeView/CTreeView.h"
#include "TreeView/CTreeGraph.h"
#include "ChanceTreeView/CChanceTreeView.h"
#include "ChanceTreeView/CChanceTreeGraph.h"
#include <QFile>

////////////////////////////////////////////////////////////////////////////////
/// IGNATView::IGNATView
///
/// @description    This is the constructor for the IGNATView class and it
///                 initializes class variables.
/// @pre            None
/// @post           Class variables are initialized.
///
/// @param parent:  This is a pointer to the parent widget.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
IGNATView::IGNATView( QWidget *parent )
: QSplitter( parent )
{
    m_toolBar = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// IGNATView::~IGNATView
///
/// @description  This is the destructor for the IGNATView class and is provided
///               for maintenance purposes only.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
IGNATView::~IGNATView()
{
}

////////////////////////////////////////////////////////////////////////////////
/// IGNATView::getToolBar
///
/// @description         This function returns a pointer to the view specific
///                      toolbar.
/// @pre                 None
/// @post                None
///
/// @return QToolBar *:  This is a pointer to the toolbar specific to the view.
///
/// @limitations         None
///
////////////////////////////////////////////////////////////////////////////////
QToolBar * IGNATView::getToolBar()
{
    //Set up the toolbar if one has not yet been initialized.
    if( m_toolBar == NULL )
        setupToolBar();

    return m_toolBar;
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATViewFactory::createGNATView
///
/// @description          This function determines if the given file is a valid
///                       GNAT file and determines what view mode the file
///                       requires if it is.  It then creates the proper view
///                       and loads that file into it.
/// @pre                  None
/// @post                 If a proper file was given, the corresponding view
///                       is created and the file is loaded into that view.
///
/// @param fileName:      This is the name of the file to be loaded.
/// @param parent:        This is a pointer to the parent object the new view
///                       will go under.
///
/// @return IGNATView *:  This is a pointer to the newly created view.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
IGNATView * CGNATViewFactory::createGNATView( const QString &fileName,
                                              QWidget *parent )
{
    int fileType;
    QFile file( fileName );

    if( !file.open( QIODevice::ReadOnly ) )
        return NULL;

    QDataStream stream(&file);

    quint32 magicNum;
    stream >> magicNum;
    stream >> fileType;

    //Make sure the file is a valid GNAT file.
    if( magicNum != SAVEFILE_MAGICNUM )
        fileType = 0;

    file.close();

    return createGNATView( (PROGRAM_MODE)fileType, fileName, parent );
}

////////////////////////////////////////////////////////////////////////////////
/// CGNATViewFactory::createGNATView
///
/// @description          This function creates the given view mode and then
///                       loads the given file into it.
/// @pre                  None
/// @post                 If a proper file was given, the corresponding view
///                       is created and the file is loaded into that view.
///
/// @param type:          This is the view type to create.
/// @param fileName:      This is the name of the file to be loaded.
/// @param parent:        This is a pointer to the parent object the new view
///                       will go under.
///
/// @return IGNATView *:  This is a pointer to the newly created view.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
IGNATView *CGNATViewFactory::createGNATView( PROGRAM_MODE type,
                                             const QString &fileName,
                                             QWidget *parent )
{
    IGNATView *ret = NULL;

    switch( type )
    {
    case MODE_PUZZLE:
        ret = new CPuzzleView( parent );
        ret->loadFromFile( fileName );
        break;
    case MODE_TREE:
        ret = new CTreeView( parent );
        ret->loadFromFile( fileName );
        break;
    case MODE_CHANCE:
        ret = new CChanceTreeView( parent );
        ret->loadFromFile( fileName );
        break;
    case MODE_UNDEFINED:
    default:
        break;
    }

    return ret;
}
