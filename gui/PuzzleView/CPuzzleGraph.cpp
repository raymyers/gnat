////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleGraph.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CPuzzleGraph class,
///               which stores the actual data of a puzzle, and the structures
///               which represent the puzzle nodes and spans.
///
////////////////////////////////////////////////////////////////////////////////

#include <QFile>
#include <QMessageBox>
#include "CPuzzleGraph.h"
#include "../../gnat.h"

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::CPuzzleGraph
///
/// @description  This is the default class constructor which initializes class
///               member variables.
/// @pre          None
/// @post         Member variables are initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleGraph::CPuzzleGraph()
{
    m_startNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::~CPuzzleGraph
///
/// @description     This is the destructor which frees up dynamic memory used
///                  by the class.  Only the node list must be deleted because
///                  the destructor of the nodes will delete the associated
///                  spans along with it.
/// @pre             None
/// @post            None
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleGraph::~CPuzzleGraph()
{
    while( !m_nodeList.isEmpty() )
        delete m_nodeList.takeFirst();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::getFileName
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
QString CPuzzleGraph::getFileName()
{
    return m_fileName;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::getNodeList
///
/// @description                     This function returns a reference to the
///                                  list of nodes in the puzzle.
/// @pre                             None
/// @post                            None
///
/// @return QList<SPuzzleNode *> &:  This is a reference to the QList holding
///                                  all the nodes in the puzzle.
///
/// @limitations                     None
///
////////////////////////////////////////////////////////////////////////////////
QList<SPuzzleNode *> &CPuzzleGraph::getNodeList()
{
    return m_nodeList;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::getSpanList
///
/// @description                     This function returns a reference to the
///                                  list of spans in the puzzle.
/// @pre                             None
/// @post                            None
///
/// @return QList<SPuzzleSpan *> &:  This is a reference to the QList holding
///                                  all the spans in the puzzle.
///
/// @limitations                     None
///
////////////////////////////////////////////////////////////////////////////////
QList<SPuzzleSpan *> &CPuzzleGraph::getSpanList()
{
    return m_spanList;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::getStartNode
///
/// @description            This function gets the pointer to the start node of
///                         the puzzle and returns NULL if no start node is set.
/// @pre                    None
/// @post                   None
///
/// @return SPuzzleNode *:  This is a pointer to the current start node of the
///                         puzzle.
///
/// @limitations            None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleNode * CPuzzleGraph::getStartNode() const
{
    return m_startNode;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::load
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
void CPuzzleGraph::load( const QString &fileName )
{
    if ( fileName.isNull() && m_fileName.isNull() )
        return;

    if ( fileName.isNull() == false )
        setFileName( fileName );

    QFile iFile(m_fileName);
    iFile.open(QIODevice::ReadOnly);

    if ( !iFile.isOpen() )
    {
        QMessageBox::critical( NULL, "File Error!", "There was an error "
                               "opening the requested file, so the operation "
                               "has been terminated.", QMessageBox::Ok,
                               QMessageBox::NoButton );
        return;
    }

    QDataStream iStream(&iFile);

    quint32 magicNum;
    int     size, i, j;

    iStream >> magicNum;
    iStream >> size;
    iStream >> size;

    for (i = 0; i < size; i++)
        m_nodeList.append(new SPuzzleNode());

    iStream >> size;

    for (i = 0; i < size; i++)
        m_spanList.append(new SPuzzleSpan());

    iStream >> i;

    if (i == -1)
        m_startNode = NULL;
    else
        m_startNode = m_nodeList[i];

    for (i = 0; i < m_nodeList.size(); i++)
    {
        iStream >> m_nodeList[i]->s_name;
        iStream >> m_nodeList[i]->s_position;
        iStream >> m_nodeList[i]->s_heuristic;
        iStream >> j;
        m_nodeList[i]->s_flag = (SPuzzleNode::NodeFlag)j;
    }

    for (i = 0; i < m_spanList.size(); i++)
    {
        iStream >> j;
        m_spanList[i]->s_node1 = m_nodeList[j];
        iStream >> j;
        m_spanList[i]->s_node2 = m_nodeList[j];
        iStream >> m_spanList[i]->s_weightFrom1;
        iStream >> m_spanList[i]->s_weightFrom2;
        iStream >> j;
        m_spanList[i]->s_flag = (SPuzzleSpan::SpanFlag)j;

        m_nodeList[m_nodeList.indexOf( m_spanList[i]->s_node1
                )]->s_spanList.append( m_spanList[i] );
        m_nodeList[m_nodeList.indexOf( m_spanList[i]->s_node2
                )]->s_spanList.append( m_spanList[i] );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::save
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
void CPuzzleGraph::save( const QString &fileName )
{
    if ( fileName.isNull() && m_fileName.isNull() )
        return;

    if ( fileName.isNull() == false )
        setFileName( fileName );

    QFile oFile(m_fileName);
    oFile.open(QIODevice::WriteOnly);
    if ( !oFile.isOpen() )
    {
        QMessageBox::critical( NULL, "File Error!", "Unable to open the "
                               "selected file for writing.  The operation has "
                               "been terminated.", QMessageBox::Ok,
                               QMessageBox::NoButton );
        return;
    }

    QDataStream oStream(&oFile);

    oStream << SAVEFILE_MAGICNUM;
    oStream << (int)MODE_PUZZLE;
    oStream << m_nodeList.size();
    oStream << m_spanList.size();
    if (m_startNode == NULL)
        oStream << (int)-1;
    else
        oStream << m_nodeList.indexOf(m_startNode);

    int i;
    for (i = 0; i < m_nodeList.size(); i++)
    {
        oStream << m_nodeList[i]->s_name;
        oStream << m_nodeList[i]->s_position;
        oStream << m_nodeList[i]->s_heuristic;
        oStream << (int)m_nodeList[i]->s_flag;
    }

    for (i = 0; i < m_spanList.size(); i++)
    {
        oStream << m_nodeList.indexOf(m_spanList[i]->s_node1);
        oStream << m_nodeList.indexOf(m_spanList[i]->s_node2);
        oStream << m_spanList[i]->s_weightFrom1;
        oStream << m_spanList[i]->s_weightFrom2;
        oStream << (int)m_spanList[i]->s_flag;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::setFileName
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
void CPuzzleGraph::setFileName( const QString &fileName )
{
    m_fileName = fileName;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraph::setStartNode
///
/// @description  This function sets the start node of the puzzle.
/// @pre          None
/// @post         The start node of the puzzle has been set.
///
/// @param node:  This is a pointer to the new start node of the puzzle.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraph::setStartNode( SPuzzleNode *node )
{
    m_startNode = node;
}

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleNode::SPuzzleNode
///
/// @description  This is the default constructor for the SPuzzleNode struct.
///               It initializes struct member variables.
/// @pre          None
/// @post         Struct member variables are initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleNode::SPuzzleNode()
{
    s_flag = MidNode;
    s_heuristic = 0.0F;
}

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleNode::~SPuzzleNode
///
/// @description  This is the destructor for the SPuzzleNode struct.  It frees
///               dynamic memory used by any spans associated with the given
///               node.
/// @pre          None
/// @post         None
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleNode::~SPuzzleNode()
{
    while( !s_spanList.isEmpty() )
        delete s_spanList.first();
}

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleSpan::SPuzzleSpan
///
/// @description  This is the default constructor for the SPuzzleSpan struct.
///               It initializes struct member variables.
/// @pre          None
/// @post         Struct member variables are initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleSpan::SPuzzleSpan()
{
    s_node1 = NULL;
    s_node2 = NULL;

    s_weightFrom1 = 0.0;
    s_weightFrom2 = 0.0;

    s_flag = EmptySpan;
}

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleSpan::SPuzzleSpan
///
/// @description  This is the default constructor for the SPuzzleSpan struct.
///               It initializes struct member variables.
/// @pre          None
/// @post         Struct member variables are initialized.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleSpan::SPuzzleSpan( SPuzzleNode* node1, SPuzzleNode* node2 )
{
    s_node1 = node1;
    s_node2 = node2;

    s_weightFrom1 = 0.0;
    s_weightFrom2 = 0.0;

    s_flag = EmptySpan;
}

////////////////////////////////////////////////////////////////////////////////
/// SPuzzleSpan::~SPuzzleSpan
///
/// @description  This is the destructor for the span struct.  It removes
///               itself from the list of spans contained within both of
///               its node connections.
/// @pre          None
/// @post         The span has been removed from both node's span lists.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleSpan::~SPuzzleSpan()
{
    if( s_node1 )
        s_node1->s_spanList.removeAt(
            s_node1->s_spanList.indexOf(this) );

    if( s_node2 )
        s_node2->s_spanList.removeAt(
            s_node2->s_spanList.indexOf(this) );
}
