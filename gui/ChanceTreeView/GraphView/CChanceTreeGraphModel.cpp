////////////////////////////////////////////////////////////////////////////////
/// @file         CChanceTreeGraphModel.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CChanceTreeGraphModel
///               class which is a wrapper for the CChanceTreeGraph class.  It is
///               designed to allow for notification of data modification by
///               requiring that all data modification be done through this
///               class.
///
////////////////////////////////////////////////////////////////////////////////

#include "CChanceTreeGraphModel.h"

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::CChanceTreeGraphModel
///
/// @description    This is the default constructor for the class and
///                 initializes member variables.
/// @pre            None
/// @post           Member variables are initialized.
///
/// @param parent:  This is a pointer to the parent QObject.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeGraphModel::CChanceTreeGraphModel( QObject *parent )
: QObject( parent )
{
    m_data = NULL;
    m_style = CStyleToolkit::instance();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::~CChanceTreeGraphModel
///
/// @description   This is the class destructor which frees up dynamic memory
///                used by the class.
/// @pre           None
/// @post          None
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
CChanceTreeGraphModel::~CChanceTreeGraphModel()
{
    //Check if the data structure exists.  If it does, delete it.
    if ( m_data != NULL )
    {
        delete m_data;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::addChildNode
///
/// @description   This function adds a new child node to the given node and
///                places it at the given location.
/// @pre           None
/// @post          A new child node is added to the given node and placed at
///                the given location.
///
/// @param node:   This is a pointer to the parent node for the new child node.
/// @param point:  This is the point on the screen where the user wants to place
///                the new child node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::addChildNode( SChanceTreeNode *node, QPoint point )
{
    //Create the new node and set the point and parent to the indicated values.
    if(node == NULL) return;
    SChanceTreeNode *newNode = new SChanceTreeNode;
    newNode->s_position = point;
    newNode->s_parent = node;
    if(node->s_chance)
    {
         newNode->s_chance = false;
         newNode->s_max = !node->s_max;
         if(node->s_childList.size() == 0)
             newNode->s_probability = 100.0;
    }
    else
    {
        newNode->s_chance = true;
        newNode->s_max = node->s_max;
    }

    //Add the new node to the child list of the parent node.
    node->s_childList.append( newNode );

    //Resort the parent node's child list and signal the canvas to redraw
    //the model.
    sortChildNodes( node );
    emit repaint();
    emit retrace();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::addChildNodeBasic
///
/// @description   This function adds a new child node to the given node and
///                places it at the origin, without repaint or retrace.
///                Used for adding many children at once.
/// @pre           None
/// @post          A new child node is added to the given node and placed at
///                QPoint(0,0). Canvas and trace are not updated.
///                New child node is returned.
///
/// @param node:   This is a pointer to the parent node for the new child node.
///
/// @return SChanceTreeNode *:  Returns a pointer to the new child node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeNode * CChanceTreeGraphModel::addChildNodeBasic( SChanceTreeNode *node )
{
    //Create the new node and set the parent appropriately.
    SChanceTreeNode *newNode = NULL;
    if(node != NULL)
    {
        newNode = new SChanceTreeNode;
        newNode->s_position = QPoint(0,0);
        newNode->s_parent = node;
        if(node->s_chance)
        {
            newNode->s_chance = false;
            newNode->s_max = !node->s_max;
            //if(node->s_childList.size() == 0)
            //   newNode->s_probability = 100.0;
        }
        else
        {
            newNode->s_chance = true;
            newNode->s_max = node->s_max;
        }
        //Add the new node to the child list of the parent node.
        node->s_childList.append( newNode );
    }

    return newNode;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::addParentNode
///
/// @description  This function inserts a new parent node above the given node.
///               If the given node is the root node, the new parent node
///               becomes the new root node.  Otherwise, the new parent node
///               becomes a child node of the former parent node.
/// @pre          None
/// @post         A node has been inserted as the parent of the indicated node.
///
/// @param node:  This is a pointer to the node to which a new parent node is
///               being added.
///
/// @limitations  Disabled for chance tree.
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::addParentNode( SChanceTreeNode *node )
{
    // This will need a rewrite if it is even needed in the chance tree.
    return;
 
    //Create the new parent node.
    SChanceTreeNode *newNode = new SChanceTreeNode;

    //Check if the indicated node was the root node or not.
    if ( node->s_parent != NULL )
    {
        //The indicated node was not the root node, so insert the new parent
        //node between the old parent and the indicated node.
        int nodeAt = node->s_parent->s_childList.indexOf( node );
        node->s_parent->s_childList.removeAt( nodeAt );
        node->s_parent->s_childList.insert( nodeAt, newNode );
    }
    else
        //Otherwise, set the new parent node as the new root node.
        setRootNode( newNode );

    //Set the new parent's node to the indicated node's old parent.
    newNode->s_parent = node->s_parent;

    //Add the indicated node to the new parent's child list.
    newNode->s_childList.append( node );

    //Set the indicated node's parent to the new parent node.
    node->s_parent = newNode;

    //Place the new parent node where the indicated node was.
    newNode->s_position = node->s_position;

    //Shift the indicated node down to make room for the new parent node.
    QPoint point( 0, m_style->getNodeRadius() * 3 );
    shiftSubTree( node, point );

    //Signal the canvas to redraw the model.
    emit repaint();
    emit retrace();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::autoLayout
///
/// @description  This function moves all the nodes to form a nicely spaced out
///               tree.
/// @pre          None
/// @post         The tree has been arranged.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::autoLayout()
{
    //This variable is used as the x value of the left most node in the tree.
    int x = m_style->getNodeRadius() + m_style->getBoldWidth() +
            m_style->getPen().width();

    //Align the x values of all the nodes in the tree.
    autoLayoutX( m_data->getRootNode(), x );

    //Set the Y value of the root node to be on the screen.
    m_data->getRootNode()->s_position.setY( m_style->getNodeRadius() +
                                            m_style->getBoldWidth() +
                                            m_style->getPen().width() );

    //Align the y values of all the nodes in the tree.
    autoLayoutY( m_data->getRootNode() );

    emit repaint();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::autoLayoutX
///
/// @description  This function arranges the given node and all its children
///               such that the left most child (or the given node, if it has
///               no children), uses the x value indicated.  Any node that has
///               children will use the average x value of its children to
///               determine its own x value.  The value x is passed by reference
///               so it may be continually used to properly space the whole
///               tree.
/// @pre          X must be the value where the left most node will be placed.
/// @post         The x values for the position of the given node and all its
///               children have been set.
///
/// @param node:  This is a pointer to the node which needs to be arranged.
/// @param x:     This is a reference to the position to use for the next node.
///               When a node uses this value, it increments it for the next
///               leaf node to use.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::autoLayoutX( SChanceTreeNode *node, int &x )
{
    //This constant is static so as not to redeclare it during the recursive
    //funtion call.  It indicates how far apart each node should be.
    static const int nodeSpacing = m_style->getNodeRadius() / 2 * 5;

    //Make sure a valid node is passed in.
    if ( node == NULL )
        return;

    //Check if the given node is a leaf node.
    if ( node->s_childList.isEmpty() )
    {
        //Set the leaf node's x value to the given value.
        node->s_position.setX( x );

        //Increment the x value for the next leaf node and return.
        x += nodeSpacing;
        return;
    }

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop recursively calls this function on
    //                the children of the given node to properly
    //                set their x values so the given node can set
    //                its x value.
    //
    // Precondition:  The given node must have at least 1 child.
    // Postcondition: The children of the given node have had their
    //                x values set.
    //
    //////////////////////////////////////////////////////////////////
    for ( int i = 0; i < node->s_childList.size(); i++ )
    {
        autoLayoutX( node->s_childList[ i ], x );
    }

    //Set the x value of the given node to the average of the child nodes so
    //as to center it above its children.
    node->s_position.setX( ( node->s_childList.first()->s_position.x() +
                             node->s_childList.last()->s_position.x() ) / 2 );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::autoLayoutY
///
/// @description  This function sets the y value of the given node and its
///               children for the autoLayout functionality.
/// @pre          The given node must have its y value set to the desired value.
/// @post         The y value of the node and its children have been set.
///
/// @param node:  This is a pointer to the node which needs to have its
///               children's y values set.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::autoLayoutY( SChanceTreeNode *node )
{
    //This constant is static so as not to redeclare it during the recursive
    //funtion call.  It indicates how far apart each node should be.
    static const int nodeSpacing = m_style->getNodeRadius() * 3;
    static const int chanceNodeSpacing = m_style->getNodeRadius() * 4;

    //Make sure the node passed in is valid.
    if ( node == NULL )
        return;

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop recursively calls this function on
    //                the children of the given node to properly
    //                set their y values.
    //
    // Precondition:  The y value of the given node is set.
    // Postcondition: The children of the given node have had their
    //                y values set.
    //
    //////////////////////////////////////////////////////////////////
    int space = (node->s_chance) ? chanceNodeSpacing : nodeSpacing;
    for ( int i = 0; i < node->s_childList.size(); i++ )
    {
        //Set the y value of the current child node and then call this function
        //to arrange the child's children properly.
        
        node->s_childList[ i ]->s_position.setY( node->s_position.y() + space );
        autoLayoutY( node->s_childList[ i ] );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::autoName
///
/// @description  This function names all the nodes alphabetically, starting
///               with A in a breadth first manner.
/// @pre          None
/// @post         All the nodes are named alphabetically, breadth-first.
///
/// @limitations  This function can only autoname up to 702 nodes.
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::autoName()
{
    //This variable is used as a loop variable.
    int i;

    //These variables are used to store the letters used to make the names for
    //the nodes.
    char firstLetter = ( 'A' - 1 ), secondLetter = 'A';

    //This list is used for traversing through the tree breadth first.
    QList<SChanceTreeNode *> nodeList;

    //THis variable is used to store the name of the next node.
    QString name;

    //Place the root node in the list to begin traversing the tree.
    nodeList.append( m_data->getRootNode() );

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop traverses through the tree breadth
    //                first by using a queue.  It names each node
    //                successively.
    //
    // Precondition:  The root node of the tree is in the queue.
    // Postcondition: Each node in the tree is named alphabetically.
    //
    //////////////////////////////////////////////////////////////////
    while ( !nodeList.empty() )
    {
        //Clear the name variable for the next name.
        name.clear();

        //Check if the name should be 2 letters long.  If it should, start with
        //the first letter.
        if ( firstLetter >= 'A' )
            name.append( QChar( firstLetter ) );

        //Append the second letter.
        name.append( QChar( secondLetter ) );

        //Check if the second letter is still before 'Z' in the alphabet.  If it
        //is, increment by 1 letter.
        if ( secondLetter < 'Z' )
            secondLetter++;
        //Otherwise, set the second letter back to 'A' and increment the first
        //letter by one.
        else
        {
            secondLetter = 'A';
            firstLetter++;
        }

        //Set the name of the node.
        nodeList.front()->s_name = name;

        //////////////////////////////////////////////////////////////
        //
        // Description:   This loop adds all the immediate children of
        //                the current node to the queue.
        //
        // Precondition:  None.
        // Postcondition: All the immediate child nodes of the current
        //                node are appended to the queue.
        //
        //////////////////////////////////////////////////////////////
        for ( i = 0; i < nodeList.front()->s_childList.size(); i++ )
            nodeList.append( nodeList.front()->s_childList[ i ] );

        //Pop the node at the front of the queue.
        nodeList.removeFirst();
    }

    emit repaint();
    emit retrace();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::autoNumber
///
/// @description  This function automatically numbers all the nodes with a
///               random integer between the min and max values, inclusive.
/// @pre          None
/// @post         All the nodes have had a value randomly assigned to them.
///
/// @param min:   This is the minimum value a node may have.
/// @param max:   This is the maximum value a node may have.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::autoNumber( int min, int max )
{
    //Seed the random number generator.
    srand( time( NULL ) );
    autoNumberHelper( min, max, m_data->getRootNode() );
    emit repaint();
    emit retrace();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::autoNumberHelper
///
/// @description  This function generates random values for the given node and
///               all its child nodes.
/// @pre          None
/// @post         The given node and all its children have a randomly generated
///               value within the given boundaries.
///
/// @param min:   This is the minimum value a node may have.
/// @param max:   This is the maximum value a node may have.
/// @param node:  This is a pointer to the root of a subtree to generate random
///               values for the tree, iterating depth first.
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::autoNumberHelper( int min, int max, SChanceTreeNode *node )
{
    if ( node == NULL )
        return;

    node->s_value = (double)( min + ( rand() % ( max - min + 1 ) ) );
    for ( int i = 0; i < node->s_childList.size(); i++ )
        autoNumberHelper( min, max, node->s_childList[ i ] );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::createNewData
///
/// @description  This function creates a new data set, deleting any old data.
/// @pre          None
/// @post         Any old data is deleted and a new data set is created.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::createNewData()
{
    //Check if there is an existing data set already and delete it if there is
    //one.
    if ( m_data != NULL )
    {
        delete m_data;
    }
    m_data = new CChanceTreeGraph;
    realignData();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::deleteNode
///
/// @description  This function deletes the indicated node.
/// @pre          None
/// @post         The indicated node and all its children are deleted.
///
/// @param node:  This is a pointer to the node that is to be deleted.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::deleteNode( SChanceTreeNode *node )
{
    if ( node == NULL )
        return;

    //Check if the indicated node is not the root node.  If not, remove the
    //indicated node from its parent's child list.
    if ( node->s_parent != NULL )
    {
        editNodeProb( node, 0.0 );
        node->s_parent->s_childList.removeAt(
            node->s_parent->s_childList.indexOf( node ) );
    }
    //Otherwise, create a new root node.
    else
    {
        setRootNode( new SChanceTreeNode );
        realignData();
    }

    //Delete the indicated node.
    delete node;

    //Signal the canvas to redraw the model.
    emit quickEditOff();
    emit repaint();
    emit retrace();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::editHistoryPosition
///
/// @description  This function updates the position of the history table.
/// @pre          None
/// @post         The history table position is updated.
///
/// @param point: This is the new position.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::editHistoryPosition( QPoint point )
{
    m_historyPosition = point;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::editNodeHistory
///
/// @description  This function updates the history table value of the given
////              node to the given value.
/// @pre          None
/// @post         The history table value is updated.
///
/// @param node:  This is a pointer to the node which is having its name
///               updated.
/// @param name:  This is the new history table value of the given node.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::editNodeHistory( SChanceTreeNode *node, int history )
{
    //Check if the node is not NULL and update its history value if it's not.
    if ( node != NULL )
    {
        node->s_history = history;
        emit repaint();
        emit retrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::editNodeName
///
/// @description  This function updates the name of the given node to the given
///               name.
/// @pre          None
/// @post         The name of the given node is updated.
///
/// @param node:  This is a pointer to the node which is having its name
///               updated.
/// @param name:  This is the new name of the given node.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::editNodeName( SChanceTreeNode *node, QString name )
{
    //Check if the node is not NULL and update its name if it's not.
    if ( node != NULL )
    {
        node->s_name = name;
        emit repaint();
        emit retrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::editNodePosition
///
/// @description   This function updates the given node's position to the given
///                point.
/// @pre           None
/// @post          The position of the given node is updated.
///
/// @param node:   This is a pointer to the node which is being moved.
/// @param point:  This is the new position of the node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::editNodePosition( SChanceTreeNode *node, QPoint point )
{
    //Check if the node is not NULL and update its position if it's not.
    if ( node != NULL )
    {
        node->s_position = point;

        //Check if the node has a parent node and resort the parent node's
        //child list if there is a parent.
        if ( node->s_parent != NULL )
            if ( sortChildNodes( node->s_parent ) )
                emit retrace();
        emit repaint();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::editNodeQuiescentFlag
///
/// @description       This function updates the quiescent flag of the given
///                    node.
/// @pre               None
/// @post              The given node's quiescent flag is updated.
///
/// @param node:       This is a pointer to the node whose quiescent flag is
///                    being updated.
/// @param quiescent:  This is the new value for the quiescent flag.
///
/// @limitations       None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::editNodeQuiescentFlag( SChanceTreeNode *node, bool quiescent )
{
    //Check if the node is not NULL and update the quiescent flag if it's not.
    if ( node != NULL )
    {
        node->s_quiescent = quiescent;
        emit repaint();
        emit retrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::editNodeValue
///
/// @description   This function updates the value of the given node.
/// @pre           None
/// @post          The given node's value is updated.
///
/// @param node:   This is a pointer to the ndoe whose value is being updated.
/// @param value:  This is the new value for the node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::editNodeValue( SChanceTreeNode *node, qreal value )
{
    //Check if the node is not NULL and update the value if it's not.
    if ( node != NULL )
    {
        node->s_value = value;
        emit repaint();
        emit retrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::editNodeProb
///
/// @description   This function updates the value of the given node.
/// @pre           None
/// @post          The given node's value is updated.
///
/// @param node:   This is a pointer to the ndoe whose value is being updated.
/// @param value:  This is the new value for the node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::editNodeProb( SChanceTreeNode *node, qreal value )
{
    // Check if the node is not NULL and update the value if it's not.
    // Siblings will have their values updated to help add up to 1.0.
    // Only nodes to the right will change if possible.
    // Nodes to the left will change only if neccessary.
    if ( node != NULL &&               // Node exist AND
         node->s_parent != NULL &&     // Node isn't root node AND
         value >= 0.0 && value <= 1.0) // 
    {
        value = qRound(value*100);
        node->s_probability = value;
        SChanceTreeNode * parent = node->s_parent;
        qreal sum = 0.0;
        int i = 0;
        int size = parent->s_childList.size();
        if(size == 1) return;
        for(int j = 0; j < size; j++)
        {
            SChanceTreeNode * n = parent->s_childList[j];
            sum += n->s_probability;
            if(node == n) i=j;
        }
        if(sum != 100.0)
        {
            QList<SChanceTreeNode *> leftSiblings;
            QList<SChanceTreeNode *> rightSiblings;
            for(int j = 0; j < i; j++) leftSiblings.append(parent->s_childList[j]);
            for(int j = i+1; j < size; j++) rightSiblings.append(parent->s_childList[j]);
            qreal diff = sum - 100;
            SChanceTreeNode * n;
            int remaining = rightSiblings.size();
            foreach(n, rightSiblings)
            {

                // frac = (overflow / remaining) rounded to two decimal places.
                qreal frac = qRound(diff / remaining);
                if((n->s_probability - frac) >= 0.0)
                {
                    diff -= frac;
                    n->s_probability -= frac;
                }
                else
                {
                   diff -= n->s_probability;
                   n->s_probability = 0.0;
                }
                remaining--;
            }
            remaining = leftSiblings.size();
            foreach(n, leftSiblings)
            {
                qreal frac = qRound(diff / remaining);
                if((n->s_probability - frac) >= 0.0)
                {
                    diff -= frac;
                    n->s_probability -= frac;
                }
                else
                {
                   diff -= n->s_probability;
                   n->s_probability = 0.0;
                }
                remaining--;
            }
        }
        emit repaint();
        emit retrace();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::editSubTreePosition
///
/// @description    This function is called when a user is moving a node along
///                 with the subtree beneath it.
/// @pre            None
/// @post           The given node and its subtree have been shifted to the new
///                 point with all the children having the same relative
///                 position.
///
/// @param parent:  This is a pointer to the node which is being shifted.
/// @param point:   This is the new position of the given node.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::editSubTreePosition( SChanceTreeNode *parent, QPoint point )
{
    if ( parent == NULL )
        return;

    //Move the given node and its subtree.
    shiftSubTree( parent, point - parent->s_position );

    //Check if the node has a parent node and resort the parent node's
    //child list if there is a parent.
    if ( parent->s_parent != NULL )
        if ( sortChildNodes( parent->s_parent ) )
            emit retrace();
    emit repaint();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::findMaxDepth
///
/// @description  This function finds the maximum depth from the given node.
///               The given node is considered at a depth of 1.  Calling this
///               function on the root node gets the depth of the entire tree.
/// @pre          None
/// @post         None
///
/// @param node:  This is a pointer to the node of a subtree which the depth
///               is being found.
///
/// @return int:  Returns the depth below the given node, and 1 if the given
///               node is a leaf node.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
int CChanceTreeGraphModel::findMaxDepth( SChanceTreeNode *node )
{
    if ( node == NULL )
        return 0;

    int maxDepth = 0, tempDepth = 0;
    for ( int i = 0; i < node->s_childList.size(); i++ )
    {
        tempDepth = findMaxDepth( node->s_childList[ i ] );
        if ( tempDepth > maxDepth )
            maxDepth = tempDepth;
    }
    return ( maxDepth + 1 );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::findNode
///
/// @description          This function attempts to find any node located at the
///                       given point.  If the point exists inside a node, then
///                       a pointer to the node is returned.  If the point does
///                       not lie within a node, NULL is returned.
/// @pre                  None
/// @post                 None
///
/// @param point:         This is the point where the user clicked.
///
/// @return SChanceTreeNode *:  Returns a pointer to the node which contains the given
///                       point and NULL if no node contains the point.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeNode * CChanceTreeGraphModel::findNode( QPoint point )
{
	SChanceTreeNode *node = NULL;

	int nodeRadius = m_style->getNodeRadius();

    //To save unnecessary computations, create a QRect around the point equal to
    //the size of a node and look for any node center points in the "virtual
    //node".
	QRect rect( 0, 0, nodeRadius * 2, nodeRadius * 2 );
	rect.moveCenter( point );

    //Recurse through the tree and find any nodes which contain the point.
	node = findNodeHelper( getRootNode(), rect );

	return node;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::findNodeHelper
///
/// @description          This function recursively calls itself so that the
///                       given node and all children nodes beneath it are
///                       searched for the center point of the given rect.
/// @pre                  None
/// @post                 None
///
/// @param node:          This is a pointer to the node which is being checked
///                       for the given point.
/// @param rect:          This is the rect around the point where the user
///                       clicked.  If the center of a node exists inside the
///                       rect, the point where the user clicked would be inside
///                       the node.
///
/// @return SChanceTreeNode *:  Returns a pointer to the node which contains the given
///                       point and NULL if no node contains the point.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeNode * CChanceTreeGraphModel::findNodeHelper( SChanceTreeNode *node, QRect &rect )
{
    if ( node == NULL )
        return NULL;

	SChanceTreeNode *returnNode = NULL;

    //Check if the rect contains the center of the given node.  If it does,
    //go through the computations to see if the center of the rect is actually
    //within the circle representing the node, and not just the bounding rect.
	if ( rect.contains( node->s_position ) )
	{
    	//These variables are used to compute if the point is within the
    	//node.
        quint32 dX, dY, radius;

        //Grab the X and Y distances from the point to the center of the
        //circle to compute 2 legs of the triangle.
        dX = abs(node->s_position.x() - rect.center().x());
        dY = abs(node->s_position.y() - rect.center().y());

		//Calculate the radius of the node.
        radius = rect.height() / 2;

		//Square the radius, X, and Y values.
        dX = dX * dX;
        dY = dY * dY;
        radius = radius * radius;

		//Check if the hypotenouse of the triangle formed by (0,0), (x,0), (0,y)
		//is less than radius.  Since squaring both the radius and hypotenouse
		//does not change which is greater than the other, do not take the
		//squareroot of each as this will cause additional unneeded computation.
        if (dX + dY <= radius)
        {
        	//The point is in the node.
            returnNode = node;
        }

	}

    //Check if a node was found that contains the center of the rect or not.
	if ( returnNode == NULL )
	{
        //////////////////////////////////////////////////////////////
        //
        // Description:   This loop calls this function recursively
        //                for all children nodes of the current node.
        //
        // Precondition:  None
        // Postcondition: A node was found, if the loop was broken.
        //
        //////////////////////////////////////////////////////////////
		for ( int i = 0; i < node->s_childList.size(); i++ )
		{
			returnNode = findNodeHelper( node->s_childList[i], rect );

			if ( returnNode != NULL )
				break;
		}
	}

	return returnNode;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::findProb
///
/// @description          This function attempts to find any node located at the
///                       given point.  If the point exists inside a node, then
///                       a pointer to the node is returned.  If the point does
///                       not lie within a node, NULL is returned.
/// @pre                  None
/// @post                 None
///
/// @param point:         This is the point where the user clicked.
///
/// @return SChanceTreeNode *:  Returns a pointer to the node which contains the given
///                       point and NULL if no node contains the point.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeNode * CChanceTreeGraphModel::findProb( QPoint point )
{
	SChanceTreeNode *node = NULL;

    //To save unnecessary computations, create a QRect around the point equal to
    //the size of a node and look for any node center points in the "virtual
    //node".
	//QRect rect( 0, 0, nodeRadius * 2, nodeRadius * 2 );
        QRect rect( 0, 0, 25*2, 25 );
	rect.moveCenter( point );

    //Recurse through the tree and find any nodes which contain the point.
	node = findProbHelper( getRootNode(), rect );

	return node;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::findProbHelper
///
/// @description          This function recursively calls itself so that the
///                       given node and all children nodes beneath it are
///                       searched for the center point of the given rect.
/// @pre                  None
/// @post                 None
///
/// @param node:          This is a pointer to the node which is being checked
///                       for the given point.
/// @param rect:          This is the rect around the point where the user
///                       clicked.  If the center of a node exists inside the
///                       rect, the point where the user clicked would be inside
///                       the node.
///
/// @return SChanceTreeNode *:  Returns a pointer to the node which contains the given
///                       point and NULL if no node contains the point.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeNode * CChanceTreeGraphModel::findProbHelper( SChanceTreeNode *node, QRect &rect )
{
    if ( node == NULL )
        return NULL;

	SChanceTreeNode *returnNode = NULL;

    //Check if the rect contains the center of the given node.  If it does,
    //go through the computations to see if the center of the rect is actually
    //within the circle representing the node, and not just the bounding rect.
	if ( (!node->s_chance) && rect.contains( node->s_probPosition ) )
	{
            returnNode = node;
    	// Deemed unnecessary.
        /*
        //These variables are used to compute if the point is within the
    	//node.
        quint32 dX, dY, radius;

        //Grab the X and Y distances from the point to the center of the
        //circle to compute 2 legs of the triangle.
        dX = abs(node->s_position.x() - rect.center().x());
        dY = abs(node->s_position.y() - rect.center().y());

		//Calculate the radius of the node.
        radius = rect.height() / 2;

		//Square the radius, X, and Y values.
        dX = dX * dX;
        dY = dY * dY;
        radius = radius * radius;

		//Check if the hypotenouse of the triangle formed by (0,0), (x,0), (0,y)
		//is less than radius.  Since squaring both the radius and hypotenouse
		//does not change which is greater than the other, do not take the
		//squareroot of each as this will cause additional unneeded computation.
        if (dX + dY <= radius)
        {
        	//The point is in the node.
            returnNode = node;
        }*/

	}

    //Check if a node was found that contains the center of the rect or not.
	if ( returnNode == NULL )
	{
        //////////////////////////////////////////////////////////////
        //
        // Description:   This loop calls this function recursively
        //                for all children nodes of the current node.
        //
        // Precondition:  None
        // Postcondition: A node was found, if the loop was broken.
        //
        //////////////////////////////////////////////////////////////
		for ( int i = 0; i < node->s_childList.size(); i++ )
		{
			returnNode = findProbHelper( node->s_childList[i], rect );

			if ( returnNode != NULL )
				break;
		}
	}

	return returnNode;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::getContainingRect
///
/// @description    This function returns a QRect which contains all the nodes
///                 currently in the graph.
/// @pre            None
/// @post           None
///
/// @return QRect:  Returns a QRect which contains all the nodes.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
QRect CChanceTreeGraphModel::getContainingRect() const
{
    //Start the rect at the root node.
    QRect ret = getContainingRectHelper( m_data->getRootNode() );

    //Adjust the containing rect to take into account how the nodes are actually
    //drawn to the screen.
    int adjust = m_style->getNodeRadius() + m_style->getBoldWidth() +
                 m_style->getPen().width();
    ret.adjust( -adjust, -adjust, adjust, adjust );
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::getContainingRectHelper
///
/// @description    This function returns a rect that contains the given node
///                 and all its children.  The rect does not take into account
///                 how the nodes are drawn to the screen, so it only contains
///                 the center points of the nodes.
/// @pre            None
/// @post           None
///
/// @param parent:  This is a pointer to the node with which the function will
///                 return a containing rect for it and all its children.
///
/// @return QRect:  Returns a rect containing the given node and all its
///                 children.
///
/// @limitations    The returned rectangle does not encompass the history table.
///
////////////////////////////////////////////////////////////////////////////////
QRect CChanceTreeGraphModel::getContainingRectHelper( SChanceTreeNode *parent ) const
{
    if ( parent == NULL )
        return QRect();
    QRect ret( parent->s_position, QSize( 1,1 ) );

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop recursively calls this function on
    //                all the children of the given node to obtain
    //                a containing rect of the given node and its
    //                children.
    //
    // Precondition:  None
    // Postcondition: The containing rect includes all the children
    //                of the given node.
    //
    //////////////////////////////////////////////////////////////////
    for ( int i = 0; i < parent->s_childList.size(); i++ )
        ret |= getContainingRectHelper( parent->s_childList[ i ] );

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::getHistoryPosition
///
/// @description    This function returns a QPoint representint the upper left
///                 corner of the history table.
/// @pre            None
/// @post           None
///
/// @return QPoint: Returns a QPoint representint the upper left corner of the
///                 history table.
///
/// @limitations    Current hack to return the bottom left corner of the tree
///                 rectangle. This is in place because moving the history
///                 table is currently not implemented and using the bottom
///                 right corner ensures that the tree is never written over.
///
////////////////////////////////////////////////////////////////////////////////
QPoint CChanceTreeGraphModel::getHistoryPosition() const
{
    return getContainingRect().bottomLeft();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::getHistoryTable
///
/// @description    This function returns a QMap which contains all the nodes
///                 currently in the graph as keys and their history table
///                 history table values as values.
/// @pre            None
/// @post           None
///
/// @return QMap:   Returns a QMap which contains all the nodes currently in
///                 the graph as keys and their history table
///                 history table values as values.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
HistoryTable CChanceTreeGraphModel::getHistoryTable() const
{
    HistoryTable ret;
    
    QList<SChanceTreeNode *> openList;
    openList = m_data->getRootNode()->s_childList;
    SChanceTreeNode *cur;
    SChanceTreeNode *i;
    
    while( openList.count() > 0 )
    {
        cur = openList.takeFirst();
        foreach( i, cur->s_childList )
            openList.append( i );
        if( cur->s_history != 0 )
            ret.append( HistoryValue(cur, cur->s_history) );
    }
    
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::getHistoryTableHelper
///
/// @description    This function recursively does a DFS on the tree filling
///                 the table with all nodes and their values.
/// @pre            None
/// @post           None
///
/// @param node:    This is a pointer to the current node in the depth first
///                 search. It will be added to the table and then the function
///                 will be recursively called on its children.
/// @param table:   This is the history table in its current state as it is 
///                 filled.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::getHistoryTableHelper( const SChanceTreeNode *node,
                      HistoryTable &table ) const
{
/*    if( node->s_history != 0 )
        table.insert( node, node->s_history );
    
    SChanceTreeNode *i;
    foreach( i, node->s_childList )
        getHistoryTableHelper( i, table );*/
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::getRootNode
///
/// @description          This function returns a pointer to the root node of
///                       the tree, if it exists.
/// @pre                  None
/// @post                 None
///
/// @return SChanceTreeNode *:  This is a pointer to the root node of the tree.
///
/// @limitations          None
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeNode * CChanceTreeGraphModel::getRootNode() const
{
    SChanceTreeNode *node = NULL;
    if ( m_data != NULL )
        node = m_data->getRootNode();

    return node;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::load
///
/// @description      This function loads the data from the given file.
/// @pre              None
/// @post             The given file is loaded into the program.
///
/// @param filename:  This is the filename to be loaded.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::load( const QString &filename )
{
    if ( m_data == NULL )
        createNewData();

    m_data->load( filename );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::realignData
///
/// @description      This function shifts the nodes so that all nodes are
///                   are painted in positive coordinates.
/// @pre              None
/// @post             The top left corner of every node is >= (0,0)
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::realignData()
{
    QPoint shiftOffset = getContainingRect().topLeft();
    if( shiftOffset.x() >= 0 && shiftOffset.y() >= 0 )
        return;

    if( shiftOffset.x() > 0 )
        shiftOffset.setX( 0 );
    if( shiftOffset.y() > 0 )
        shiftOffset.setY( 0 );

    shiftSubTree( m_data->getRootNode(), -shiftOffset );

    emit repaint();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::rehook
///
/// @description    This function attaches the second node to tree as a child
///                 node of the first parameter.
/// @pre            None
/// @post           The node is attached to the parent.
///
/// @param parent:  This is a pointer to the node which the other node is being
///                 attached to as the other node's parent.
/// @param node:    This is a pointer to the node which is being reattached to
///                 the tree.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::rehook( SChanceTreeNode *parent, SChanceTreeNode *node )
{
    if ( parent == NULL || node == NULL )
        return;

    parent->s_childList.append( node );
    node->s_parent = parent;
    sortChildNodes( parent );

    emit repaint();
    emit retrace();
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::save
///
/// @description      This function saves the current data to the given file.
/// @pre              None
/// @post             The data is saved to the given file.
///
/// @param filename:  This is the filename to save the data to.
///
/// @limitations      None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::save( const QString &filename ) const
{
    if ( m_data != NULL )
        m_data->save( filename );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::setRootNode
///
/// @description  This function sets the root node of the tree to the indicated
///               node.
/// @pre          None
/// @post         The root node of the tree has been updated if a data set
///               exists.
///
/// @param node:  This is a pointer to the new root node.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::setRootNode( SChanceTreeNode *node )
{
    if ( m_data != NULL )
        m_data->setRootNode( node );
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::shiftSubTree
///
/// @description    This function shifts the given node and all its children by
///                 the given point.
/// @pre            None
/// @post           The given node and all its children are shifted by the given
///                 amount.
///
/// @param parent:  This is a pointer to the parent node which is being shifted.
/// @param point:   This point contains the offset to shift the given node and
///                 all its children by.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::shiftSubTree( SChanceTreeNode *parent, QPoint point )
{
    if ( parent == NULL )
        return;

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop recursively calls this function for
    //                each child node of the given node in order to
    //                shift the entire sub tree.
    //
    // Precondition:  None
    // Postcondition: All child nodes below the given node are
    //                shifted by the given amount.
    //
    //////////////////////////////////////////////////////////////////
    for ( int i = 0; i < parent->s_childList.size(); i++ )
    {
        shiftSubTree( parent->s_childList[i], point );
    }
    parent->s_position = parent->s_position + point;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::sortChildNodes
///
/// @description    This function sorts the child node list of the given node
///                 so that the order of the list is the order the nodes appear
///                 on the screen, from left to right.
/// @pre            None
/// @post           The given node's child node list is sorted.
///
/// @param parent:  This is a pointer to the parent node whose child list needs
///                 to be sorted.
///
/// @return bool:   Returns true if the child list was resorted and false if the
///                 list was already sorted.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
bool CChanceTreeGraphModel::sortChildNodes( SChanceTreeNode *parent )
{
    bool listChanged = false;
    //Check to make sure the given node is not NULL, and return if it is.
    if ( parent == NULL )
        return false;

    int minIndex;

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop sorts through the child list of the
    //                given node and sorts them from left to right,
    //                based on their position.  This is done by
    //                iterating through the list and finding the
    //                smallest X position and placing it at the start
    //                of the list.  Then the 2nd smallest value is
    //                found, and so on and so forth.
    //
    // Precondition:  None
    // Postcondition: The child nodes are sorted.
    //
    //////////////////////////////////////////////////////////////////
    for ( int i = 0; i < parent->s_childList.size(); i++ )
    {
        minIndex = i;
        for ( int j = i + 1; j < parent->s_childList.size(); j++ )
        {
            if ( parent->s_childList[j]->s_position.x() <
                 parent->s_childList[minIndex]->s_position.x() )
            {
                minIndex = j;
            }
        }

        //Only swap if the value isn't at the proper spot in the list already.
        if ( minIndex != i )
        {
            parent->s_childList.swap( minIndex, i );
            listChanged = true;
        }
    }
    return listChanged;
}

////////////////////////////////////////////////////////////////////////////////
/// CChanceTreeGraphModel::unhook
///
/// @description  This function unhooks the given node from the tree.
/// @pre          None
/// @post         The given node is removed from the tree.
///
/// @param node:  This is a pointer to the node which is to be unhooked from the
///               tree.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CChanceTreeGraphModel::unhook( SChanceTreeNode *node )
{
    if ( node == NULL )
        return;

    node->s_parent->s_childList.removeAt(
        node->s_parent->s_childList.indexOf( node ) );
    node->s_parent = NULL;

    emit repaint();
    emit retrace();
}
