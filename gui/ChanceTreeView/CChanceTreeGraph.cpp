////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeGraph.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CChanceTreeGraph class,
///               which stores the actual data of a tree, and the structure
///               which represents the tree nodes.
///
////////////////////////////////////////////////////////////////////////////////

#include <QFile>
#include <QMessageBox>
#include "CChanceTreeGraph.h"
#include "../../gnat.h"

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::CChanceTreeGraph
///
/// @description  This is the default class constructor which initializes class
///               member variables.
/// @pre          None
/// @post         Member variables are initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeGraph::CChanceTreeGraph()
{
    m_rootNode = NULL;
    m_rootNode = new SChanceTreeNode;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::~CChanceTreeGraph
///
/// @description     This is the destructor which frees up dynamic memory used
///                  by the class.  Only the start node must be deleted because
///                  the destructor of the nodes will delete the associated
///                  child nodes.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeGraph::~CChanceTreeGraph()
{
    if ( m_rootNode != NULL )
        delete m_rootNode;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::getFileName
///
/// @description      This function returns the current filename.
/// @pre              None
/// @post             None
///
/// @return QString:  This is the current filename.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
QString CChanceTreeGraph::getFileName()
{
    return m_fileName;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::getRootNode
///
/// @description          This function gets the pointer to the root node of
///                       the tree and returns NULL if no root node is set.
/// @pre                  None
/// @post                 None
///
/// @return SChanceTreeNode *:  This is a pointer to the current root node of the
///                       tree.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeNode * CChanceTreeGraph::getRootNode() const
{
    return m_rootNode;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::load
///
/// @description      This function loads data from the indicated file.
/// @pre              None
/// @post             The data is loaded from the file, if it is a valid file.
///
/// @param fileName:  This is the filename to load the data from.  If no
///                   filename is specified, the name stored in the class will
///                   be used.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraph::load( const QString &fileName )
{
    //Check that either the given filename or the one stored within the class
    //are valid.  If both are NULL, return without loading a file.
    if ( fileName.isNull() && m_fileName.isNull() )
        return;

    //Check if the given filename is not NULL.  As long as it's not NULL,
    //set the name stored in the class to the given name.
    if ( fileName.isNull() == false )
        setFileName( fileName );

    //Open the indicated file in read only mode.
    QFile iFile( m_fileName );
    iFile.open( QIODevice::ReadOnly );
    if ( !iFile.isOpen() )
    {
        QMessageBox::critical( NULL, "File Error!", "There was an error "
                               "opening the requested file, so the operation "
                               "has been terminated.", QMessageBox::Ok,
                               QMessageBox::NoButton );
        return;
    }
    QDataStream iStream(&iFile);

    //These variables are used for reading in the program magic number and the
    //program mode, respectively.
    quint32 magicNum;
    int     i;

    //Read the magic number of program mode from the data stream to get to the
    //data stored in the file.  These values were checked previously before
    //this load function was called.
    iStream >> magicNum;
    iStream >> i;

    //Check if there is currently a tree loaded in the program.  If there is,
    //delete it.
    if ( m_rootNode != NULL )
        delete m_rootNode;

    //Create a new root node and load the data from the save file.
    m_rootNode = new SChanceTreeNode;
    loadHelper( m_rootNode, iStream );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::loadHelper
///
/// @description    This function loads a single node from the given data stream
///                 and then recursively calls itself to load the tree depth
///                 first.
/// @pre            None
/// @post           The given node is loaded along with all its children.
///
/// @param node:    This is a pointer to the node in which to load the data to.
/// @param stream:  This is a reference to the data stream which contains the
///                 node's data which needs to be loaded.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraph::loadHelper( SChanceTreeNode *node, QDataStream &stream )
{
    //This variable is used to store how many children the current node had.
    int size;

    //Read in the data for this node and load it to the current node.
    stream >> node->s_name;
    stream >> node->s_max;
    stream >> node->s_chance;
    if( !node->s_chance )
    {
        stream >> node->s_value;
        stream >> node->s_quiescent;
        stream >> node->s_probability;
    }
    stream >> node->s_history;
    stream >> node->s_position;

    //Read in how many children nodes this node had.
    stream >> size;

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop adds the proper number of child nodes
    //                and then recursively calls this function to
    //                load them in the proper order.
    //
    // Precondition:  None
    // Postcondition: All the current node's children are loaded from
    //                the save file.
    //
    //////////////////////////////////////////////////////////////////
    for (int i = 0; i < size; i++)
    {
        //Create the next child node.
        node->s_childList.append( new SChanceTreeNode );
        node->s_childList[i]->s_parent = node;

        //Load the child node's data.
        loadHelper( node->s_childList[i], stream );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::save
///
/// @description      This function writes out the current data to the indicated
///                   file.
/// @pre              None
/// @post             The data is saved to the file.
///
/// @param fileName:  This is the filename to save the file to.  If no filename
///                   is specified, the name stored in the class will be used.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraph::save( const QString &fileName )
{
    //Check that either the given filename or the one stored within the class
    //are valid.  If both are NULL, return without saving the file.
    if ( fileName.isNull() && m_fileName.isNull() )
        return;

    //Check if the given filename is not NULL.  As long as it's not NULL,
    //set the name stored in the class to the given name.
    if ( fileName.isNull() == false )
        setFileName( fileName );

    //Open the indicated file in write only mode.
    QFile oFile( m_fileName );
    oFile.open( QIODevice::WriteOnly );
    if ( !oFile.isOpen() )
    {
        QMessageBox::critical( NULL, "File Error!", "Unable to open the "
                               "selected file for writing.  The operation has "
                               "been terminated.", QMessageBox::Ok,
                               QMessageBox::NoButton );
        return;
    }
    QDataStream oStream( &oFile );

    //Write out the save file magic number to indicate this file is used by this
    //program.  Also write out the current program mode so the program knows
    //when later loading that the file is a tree save file.
    oStream << SAVEFILE_MAGICNUM;
    oStream << (int)MODE_CHANCE;

    //Call the recursive function which writes out the tree information.
    saveHelper( m_rootNode, oStream );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::saveHelper
///
/// @description    This function saves a single node to the given data stream
///                 and then recursively calls itself to save the tree depth
///                 first.
/// @pre            None
/// @post           The given node is saved along with all its children.
///
/// @param node:    This is a pointer to the node in which to save the data
///                 from.
/// @param stream:  This is a reference to the data stream which the node's data
///                 will be written to.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraph::saveHelper( const SChanceTreeNode *node, QDataStream &stream )
{
    //Write out the current node's data to the save file.
    stream << node->s_name;
    stream << node->s_max;
    stream << node->s_chance;
    if( !node->s_chance )
    {
        stream << node->s_value;
        stream << node->s_quiescent;
        stream << node->s_probability;
    }
    stream << node->s_history;
    stream << node->s_position;

    //Write out the number of children nodes the current node has.
    stream << node->s_childList.size();

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop recursively calls this function to
    //                write out all the child nodes of the current
    //                node.
    //
    // Precondition:  None
    // Postcondition: All the current node's children are written to
    //                the save file.
    //
    //////////////////////////////////////////////////////////////////
    for (int i = 0; i < node->s_childList.size(); i++)
    {
        saveHelper( node->s_childList[i], stream );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::setFileName
///
/// @description      This function sets the filename for the current data to
///                   the given name.
/// @pre              None
/// @post             The filename has been set.
///
/// @param fileName:  This is the file name to use for the current data.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraph::setFileName( const QString &fileName )
{
    m_fileName = fileName;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraph::setRootNode
///
/// @description  This function sets the root node of the puzzle.
/// @pre          None
/// @post         The root node of the puzzle has been set.
///
/// @param node:  This is a pointer to the new root node of the tree.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraph::setRootNode( SChanceTreeNode *node )
{
    m_rootNode = node;
}

////////////////////////////////////////////////////////////////////////////////
/// SChanceTreeNode::SChanceTreeNode
///
/// @description  This is the default constructor for the SChanceTreeNode struct.
///               It initializes struct member variables.
/// @pre          None
/// @post         Struct member variables are initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeNode::SChanceTreeNode() : s_probPosition(-100,-100)
{
    s_quiescent = false;
    s_value     = 0.0F;
    s_history   = 0;
    s_parent    = NULL;
    s_chance    = false;
    s_max       = true;
    s_probability = 0.0F;
}

////////////////////////////////////////////////////////////////////////////////
/// SChanceTreeNode::~SChanceTreeNode
///
/// @description  This is the destructor for the SChanceTreeNode struct.  It frees
///               dynamic memory used by any children of the given node.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeNode::~SChanceTreeNode()
{
    while( !s_childList.isEmpty() )
        delete s_childList.takeFirst();
}
