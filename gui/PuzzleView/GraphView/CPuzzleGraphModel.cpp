////////////////////////////////////////////////////////////////////////////////
/// @file         CPuzzleGraphModel.cpp
///
/// @author       Bob Buehler, Matt Dissinger, Kyle Owen, Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the definition of the CPuzzleGraphModel
///               class which is a wrapper for the CPuzzleGraph class.  It is
///               designed to allow for notification of data modification by
///               requiring that all data modification be done through this
///               class.
///
/// comment
////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "CPuzzleGraphModel.h"
#include "CPuzzleSpanArtist.h"
#include <QBitmap>

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::CPuzzleGraphModel
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
CPuzzleGraphModel::CPuzzleGraphModel( QObject *parent )
: QObject( parent )
{
    m_data = NULL;
    m_style = CStyleToolkit::instance();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::~CPuzzleGraphModel
///
/// @description   This is the class destructor which frees up dynamic memory
///                used by the class.
/// @pre           None
/// @post          None
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
CPuzzleGraphModel::~CPuzzleGraphModel()
{
    if (m_data != NULL)
    {
        delete m_data;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::addEdge
///
/// @description           This function adds an edge between the two indicated
///                        nodes.  If the bidirectional flag is marked true,
///                        the new edge is bidirectional, otherwise, it is
///                        directional from node 1 to node 2.
/// @pre                   None
/// @post                  A new edge is created according to the given
///                        parameters.
///
/// @param node1:          This is the index value of the first node the edge
///                        is connected to.
/// @param node2:          This is the index value of the second node the edge
///                        is connected to.
/// @param bidirectional:  This variable is used to indicate if the edge is
///                        bidirectional (true) or directional (false).
///
/// @limitations           None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::addEdge( int node1, int node2, bool bidirectional )
{
    //Find any existing span for the 2 nodes, as the edge is added to the span.
    int span = findSpan( node1, node2 );
    if ( span == -1 )
        span = newSpan( node1, node2 );

    SPuzzleSpan *pSpan = m_data->getSpanList()[ span ];

    //If the new edge is bidirectional, change the span to indicate a
    //bidirectional edge.
    if ( bidirectional )
        pSpan->s_flag = SPuzzleSpan::Bidirectional;
    else
    {
        //Initially indicate the edge is coming from node 1 and going to node 2.
        bool from1 = true;

        //Check if the span is stored with the given nodes in reverse order and
        //flip the edge direction if they are.
        if ( m_data->getNodeList().indexOf( pSpan->s_node1 ) != node1 )
            from1 = false;

        if ( from1 )
            pSpan->s_flag = SPuzzleSpan::SpanFlag( pSpan->s_flag |
                                                   SPuzzleSpan::From1 );
        else
            pSpan->s_flag = SPuzzleSpan::SpanFlag( pSpan->s_flag |
                                                   SPuzzleSpan::From2 );

        //If a bidirectional edge previously existed, overwrite it with a split
        //span.
        if ( pSpan->s_flag == SPuzzleSpan::Bidirectional )
            pSpan->s_flag = SPuzzleSpan::Split;
    }

	emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::addNode
///
/// @description  This function adds the given node to the data model.
/// @pre          None
/// @post         The node has been added to the current data.
///
/// @param node:  This is a pointer to the node to be added.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::addNode(SPuzzleNode *node)
{
    if (m_data == NULL)
    {
        createNewData();
    }

    m_data->getNodeList().append(node);

	emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::addSpan
///
/// @description  This function adds the given node to the data model.
/// @pre          None
/// @post         The span has been added to the current data.
///
/// @param span:  This is a pointer to the span to be added.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::addSpan(SPuzzleSpan *span)
{
    if (m_data == NULL)
    {
        createNewData();
    }

    m_data->getSpanList().append(span);

	emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::createNewData
///
/// @description  This function creates a new data set, deleting any old data.
/// @pre          None
/// @post         Any old data is deleted and a new data set is created.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::createNewData()
{
    if (m_data != NULL)
    {
        delete m_data;
    }
    m_data = new CPuzzleGraph;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::deleteNodeAt
///
/// @description   This function deletes the node located at the given position
///                in the QList.
/// @pre           None
/// @post          The indicated node is deleted, if present.
///
/// @param index:  This is the index into the QList of the node to be deleted.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::deleteNodeAt(int index)
{
    if (m_data != NULL && m_data->getNodeList().size() > index && index >= 0)
    {
        SPuzzleNode *node = m_data->getNodeList().takeAt(index);
        SPuzzleSpan *span;
        foreach(span, node->s_spanList)
        {
            //m_data->getSpanList().removeAt(m_data->getSpanList().indexOf(span));
            m_data->getSpanList().removeAll(span);
        }

        // I, Ray Myers, take credit for finding this special case.
        if(node == m_data->getStartNode())
            m_data->setStartNode(NULL);

        delete node;
        
    }

    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::deleteSpanAt
///
/// @description   This function deletes the span located at the given position
///                in the QList.
/// @pre           None
/// @post          The indicated span is deleted, if present.
///
/// @param index:  This is the index into the QList of the span to be deleted.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::deleteSpanAt(int index)
{
    if (m_data != NULL && m_data->getSpanList().size() > index && index >= 0)
    {
        delete m_data->getSpanList().takeAt(index);
    }

    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::editNodeEndFlag
///
/// @description   This function sets the end node flag of the node at the
///                given index into the QList.
/// @pre           None
/// @post          The flag of the given node is updated.
///
/// @param index:  This is the index into the QList of the node to be edited.
/// @param flag:   This is the value used to indicate if the node is an end
///                node or not.  True indicates the node is an end node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::editNodeEndFlag(int index, bool flag)
{
    if (m_data != NULL && m_data->getNodeList().size() > index && index >= 0)
    {
        SPuzzleNode *node = m_data->getNodeList().at(index);
        if (flag)
            node->s_flag =
                SPuzzleNode::NodeFlag(node->s_flag | SPuzzleNode::EndNode);
        else
            node->s_flag =
                SPuzzleNode::NodeFlag(node->s_flag ^ SPuzzleNode::EndNode);
    }

    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::editNodeName
///
/// @description   This function sets the name of the node at the given index
///                into the QList.
/// @pre           None
/// @post          The name of the given node is updated.
///
/// @param index:  This is the index into the QList of the node to be edited.
/// @param name:   This is the new name of the node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::editNodeName(int index, QString name)
{
    if (m_data != NULL && m_data->getNodeList().size() > index && index >= 0)
    {
        m_data->getNodeList().at(index)->s_name = name;
    }

    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::editNodeHeuristic
///
/// @description   This function sets the heuristic value of the node at the
///                given index into the QList.
/// @pre           None
/// @post          The heuristic value of the given node is updated.
///
/// @param index:  This is the index into the QList of the node to be edited.
/// @param name:   This is the heuristic value of the node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::editNodeHeuristic(int index, qreal value)
{
    if (m_data != NULL && m_data->getNodeList().size() > index && index >= 0)
    {
        m_data->getNodeList().at(index)->s_heuristic = value;
    }

    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::editNodePosition
///
/// @description   This function sets the position of the node at the given
///                index into the QList.
/// @pre           None
/// @post          The position of the given node is updated.
///
/// @param index:  This is the index into the QList of the node to be edited.
/// @param point:  This is the new position of the node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::editNodePosition(int index, QPoint point)
{
    if (m_data != NULL && m_data->getNodeList().size() > index && index >= 0)
    {
        m_data->getNodeList().at(index)->s_position = point;
    }

    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::editNodeStartFlag
///
/// @description   This function sets the start node flag of the node at the
///                given index into the QList.
/// @pre           None
/// @post          The flag of the given node is updated.
///
/// @param index:  This is the index into the QList of the node to be edited.
/// @param flag:   This is the value used to indicate if the node is an start
///                node or not.  True indicates the node is an start node.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::editNodeStartFlag(int index, bool flag)
{
    if (m_data != NULL)
    {
        SPuzzleNode *node = m_data->getNodeList().at(index);
        if (flag)
        {
            if (m_data->getStartNode() == NULL)
            {
                node->s_flag =
                    SPuzzleNode::NodeFlag(
                        node->s_flag | SPuzzleNode::StartNode);
                m_data->setStartNode(node);
            }
        }
        else
        {
            if (m_data->getStartNode() == node)
            {
                node->s_flag =
                    SPuzzleNode::NodeFlag(
                        node->s_flag ^ SPuzzleNode::StartNode);
                m_data->setStartNode(NULL);
            }
        }
    }

    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::editSpanFlag
///
/// @description     This function is used the change the flag value of a span
///
/// @pre             The indexed span must exist
/// @post            The flag of the indexed span is changed to "flag"
///
/// @param index:    The index of the span
/// @param flag:     The flag value to set
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::editSpanFlag(int index, SPuzzleSpan::SpanFlag flag)
{
    SPuzzleSpan *span = m_data->getSpanList().at(index);
    span->s_flag = flag;
    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::editSpanWeightFrom1
///
/// @description     This function is used to change the weight associated
///                  from traversing from node1 to node2 where node 1 and 2
///                  are at the end points of the indexed span
///
/// @pre             The indexed span must exist
/// @post            The weight associated with moving from node 1 to 2 is
///                  changed in the indexed span
///
/// @param index:    The index of the span
/// @param weight:   The weight to be set
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::editSpanWeightFrom1(int index, qreal weight)
{
    SPuzzleSpan *span = m_data->getSpanList().at(index);
    span->s_weightFrom1 = weight;
    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::editSpanWeightFrom2
///
/// @description     This function is used to change the weight associated
///                  from traversing from node1 to node2 where node 2 and 1
///                  are at the end points of the indexed span
///
/// @pre             The indexed span must exist
/// @post            The weight associated with moving from node 2 to 1 is
///                  changed in the indexed span
///
/// @param index:    The index of the span
/// @param weight:   The weight to be set
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::editSpanWeightFrom2(int index, qreal weight)
{
    SPuzzleSpan *span = m_data->getSpanList().at(index);
    span->s_weightFrom2 = weight;
    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::findEdge
///
/// @description     This function is used to detect if the user clicked on a
///                  span and if so the index of that span
///
/// @pre             None
/// @post            None
///
/// @param p:        The mouse click point
/// @param span:     Is set to the index of the clicked span or -1 is no span
///                  was clicked.
/// @param type:     Is set to the type of span clicked...
///                  1  span from node1 to node2 was clicked
///                  2  span from node2 to node1 was clicked
///                  3  span is bidirectional
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::findEdge(QPoint p, int &span, int &type) const
{
    int index = 0;

    span = -1;
    type = -1;

    while(true)
    {
        if(index >= getSpanCount()) break;

        if(getSpanAt(index)->s_flag != SPuzzleSpan::Split)
        {
            if(testEdgeNormal(p, index, type) == true) { span = index; break; }
        }
        else
        {
            if(testEdgeSplit(p, index, type) == true)  { span = index; break; }
        }

        index++;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::findWeight
///
/// @description     This function is used to detect if the user clicked on a
///                  span and if so the index of that span
///
/// @pre             None
/// @post            None
///
/// @param p:        The mouse click point
/// @param span:     Is set to the index of the clicked span or -1 is no span
///                  was clicked.
/// @param type:     Is set to the type of span clicked...
///                  1  weight from node1 to node2 was clicked
///                  2  weight from node2 to node1 was clicked
///                  3  weight from node1 to and from node2
///
/// @return void:    No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::findWeight(QPoint p, int &span, int &type) const
{
    int index = 0;
    span = -1;
    type = -1;
    while(true)
    {
        if(index >= getSpanCount()) break;
        if(testEdgeWeight(p, index, type) == true)
            { span = index; break; }
        index++;
    }
}


////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::testEdgeWeight
///
/// @description    This function is used to test if the user clicked on an edge
///                 weight (that is the text used to indicate the edge weight)
///
/// @pre            None
/// @post           Collision case is returned
///
/// @param p:       The mouse click point
/// @param index:   The index of the span to check
/// @param type:    The type of span clicked...
///                 1 edge weight from node1 to node2 was clicked
///                 2 edge weight from node2 to node1 was clicked
///                 3 edge was bidirectional
///
/// @return bool:   True if p is on the edge weight / ow false
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleGraphModel::testEdgeWeight(QPoint p, int index, int &type) const
{
    QFontMetrics          metrics(m_style->getFont());
    QRect                 rect;
    SPuzzleSpan::SpanFlag flag = getSpanAt(index)->s_flag;
    QString               w1   = QString::number(getSpanAt(index)->s_weightFrom1);
    QString               w2   = QString::number(getSpanAt(index)->s_weightFrom2);

    float  a1, b1, c1;  // Plane equation
    float  x0, y0;      // Node1 position
    float  x1, y1;      // Node2 position

    // Construct plane equation
    x0 = getSpanAt(index)->s_node1->s_position.x();
    y0 = getSpanAt(index)->s_node1->s_position.y();
    x1 = getSpanAt(index)->s_node2->s_position.x();
    y1 = getSpanAt(index)->s_node2->s_position.y();
    getPlaneEquation(x0, y0, x1, y1, a1, b1, c1);
    a1 *= -1.0f; b1 *= -1.0f; c1 *= -1.0f;

    if(flag != SPuzzleSpan::Split)
    {
        if(flag == SPuzzleSpan::From1) { rect = metrics.boundingRect(w1); type = 1; }
        if(flag == SPuzzleSpan::From2) { rect = metrics.boundingRect(w2); type = 2; }
        else                           { rect = metrics.boundingRect(w1); type = 3; }

        // Calc midpoint of text box
        x0 = 0.5f * (x0+x1) + 5.0f * a1;
        y0 = 0.5f * (y0+y1) + 5.0f * b1;
        x0 += 0.5f * (float)rect.width()  * a1;
        y0 += 0.5f * (float)rect.height() * b1;

        // Calc upper left corner of text box
        x0 -= 0.5f * (float)rect.width();
        y0 -= 0.5f * (float)rect.height();

        // Calc bottom right coordinate of text box
        x1 = x0 + rect.width();
        y1 = y0 + rect.height();

        // Bounding box test
        if((x0<p.x())&&(x1>p.x()) && (y0<p.y())&&(y1>p.y()))
            return true;
        else
            return false;
    }

    // If click is in the pos half space of plane we should check the node1 -> node2 curve
    // If click in in the neg half space of plane we should check the node2 -> node1 curve
    if((a1*p.x() + b1*p.y() + c1) > 0.0f)
    {
        type = 1;
        rect = metrics.boundingRect(w1);
    }
    else
    {
        type = 2;
        rect = metrics.boundingRect(w2);
        a1 *= -1.0f;
        b1 *= -1.0f;
    }

    // Calc midpoint of text box on
    x0 = 0.5f * (x0+x1) + (5.0f + m_style->getNodeRadius()) * a1;
    y0 = 0.5f * (y0+y1) + (5.0f + m_style->getNodeRadius()) * b1;
    x0 += 0.5f * (float)rect.width()  * a1;
    y0 += 0.5f * (float)rect.height() * b1;

    // Calc upper left corner of text box
    x0 -= 0.5f * (float)rect.width();
    y0 -= 0.5f * (float)rect.height();

    // Calc bottom right coordinate of text box
    x1 = x0 + rect.width();
    y1 = y0 + rect.height();

    // Bounding box test
    if((x0<p.x())&&(x1>p.x()) && (y0<p.y())&&(y1>p.y())) {           return true;  }
    else                                                 { type = 0; return false; }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::testEdgeSplit
///
/// @description    This function is used to test if the user clicked on an edge
///                 between two nodes that is drawn as a cubic benzier spline.
///
/// @pre            None
/// @post           Collision case is returned
///
/// @param p:       The mouse click point
/// @param index:   The index of the span to check
/// @param type:    The type of span clicked...
///                 1 span from node1 to node2 was clicked
///                 2 span from node2 to node1 was clicked
///
/// @return bool:   True if p is on the curve / ow false
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleGraphModel::testEdgeSplit(QPoint p, int index, int &type) const
{
    static const float cost   = cosf(0.392699f);
    static const float sint   = sinf(0.392699f);
    const float        r      = (float)(m_style->getNodeRadius());
    float              delta  = (float)(m_style->getPen().width()+2) / 2.0f;
                       delta *= delta;

    int    count;           // Bisection iteration cound
    float  a1, b1, c1;      // Plane equation 1
    float  a2, b2, c2;      // Plane equation 2
    float  a3, b3, c3;      // Plane equation 3
    float  d1, d2, d3;      // Distance mouse click is from plane 1, 2, 3
    float  x0, y0, x1, y1;  // Points used to generate plane equations...
    float  x2, y2, x3, y3;  // ...
    float  ax, bx, cx;      // X-axis Benzier coefficeints
    float  ay, by, cy;      // Y-axis Benzier coefficeints
    float  u,  u0, u1;      // Misc variables
    float  m;               // Dist from node1 to node2

    // Construct plane 1
    x0 = getSpanAt(index)->s_node1->s_position.x();
    y0 = getSpanAt(index)->s_node1->s_position.y();
    x3 = getSpanAt(index)->s_node2->s_position.x();
    y3 = getSpanAt(index)->s_node2->s_position.y();
    getPlaneEquation(x0, y0, x3, y3, a1, b1, c1);

    // Construct plane 2
    x1 = x0 + a1 * delta; y1 = y0 + b1 * delta;
    x2 = x0 - a1 * delta; y2 = y0 - b1 * delta;
    getPlaneEquation(x1, y1, x2, y2, a2, b2, c2);

    // Construct plane 3
    x1 = x3 - a1 * delta; y1 = y3 - b1 * delta;
    x2 = x3 + a1 * delta; y2 = y3 + b1 * delta;
    getPlaneEquation(x1, y1, x2, y2, a3, b3, c3);

    // Get the distance away from each plane
    d1 = a1*p.x() + b1*p.y() + c1;
    d2 = a2*p.x() + b2*p.y() + c2;
    d3 = a3*p.x() + b3*p.y() + c3;

    // Check for early out
    if(d2 < 0.0f) return false;
    if(d3 < 0.0f) return false;

    // Calc distance from node1 to node2
    m = sqrtf((x0-x3)*(x0-x3) + (y0-y2)*(y0-y3));

    // The current coordinate sytem is realative to node1
    // The origin is the center of node1
    // The x-axis points form node1 to node2
    // The y-axsi is 90 counter clockwise off the x-axis
    // The x-coord of the mouse click is d2
    // The y-coord of the mouse click is d1

    // Calculate benzier spline control points
    x0 = r * cost;          y0 = r * sint;          // Start point
    x3 = m - x0;            y3 = y0;                // End point
    x1 = x0 + 2.0f * x0;    y1 = y0 + 2.0f * y0;    // Control point 1
    x2 = x3 - 2.0f * x0;    y2 = y3 + 2.0f * y0;    // Control point 2

    // If the click is below the x-axis we want to check the bottom spline
    // curve instead of the top...so flip the control points about the y-axis
    if(d1 < 0.0f)
    {
        y0 *= -1.0f; y1 *= -1.0f;
        y2 *= -1.0f; y3 *= -1.0f;
    }

    // Calculate benzier spline constants
    cx = 3.0f * (x1-x0);        cy = 3.0f * (y1-y0);
    bx = 3.0f * (x2-x1)-cx;     by = 3.0f * (y2-y1)-cy;
    ax = x3-x0-cx-bx;           ay = y3-y0-cy-by;

    // Initialize bisection search parameters
    u0 = 0.0f; u1 = 1.0f; count = 10;

    // Begin search
    while(count > 0)
    {
        // Calc search parameter
        u  = (u0 + u1) / 2.0f;

        // Line from spline point to mouse click
        x1 = d2 - (ax*u*u*u + bx*u*u + cx*u + x0);
        y1 = d1 - (ay*u*u*u + by*u*u + cy*u + y0);

        // Tangent on spline
        x2 = 3*ax*u*u + 2*bx*u + cx;
        y2 = 3*ay*u*u + 2*by*u + cy;

        // Figure out how to adjust u
        d3 = x1*x2 + y1*y2;
        if     (d3 < 0.0f) { u1 = u; }
        else if(d3 > 0.0f) { u0 = u; }
        else               { break;  }
        count--;
    }

    // Calc approx distance from spline curve
    d3 = x1*x1 + y1*y1;

    // Return collision case
    if(d3 < delta)
    {
        if(d1 < 0.0f) { type = 1; return true; }
        else          { type = 2; return true; }
    }
    else return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::testEdgeNormal
///
/// @description    This function is used to test if the user clicked on an edge
///                 between two nodes that is drawn as a straight line
///
/// @pre            None
/// @post           Collision case is returned
///
/// @param p:        The mouse click point
/// @param index:    The index of the span to check
/// @param span:     Is set to the value of index if the indexed span was
///                  clicked on
/// @param type:     The type of span clicked...
///                  1   span from node1 to node2 was clicked
///                  2   span from node2 to node1 was clicked
///                  3   span was bidirectional
///
/// @return bool:   True if p is on the curve / ow false
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
bool CPuzzleGraphModel::testEdgeNormal(QPoint p, int index, int &type) const
{
    const float delta = (float)(m_style->getPen().width()+2) / 2.0f;

    float  a1, b1, c1;      // Plane equation 1
    float  a2, b2, c2;      // Plane equation 2
    float  a3, b3, c3;      // Plane equation 3
    float  d1, d2, d3;      // Distance mouse click is from plane 1, 2, 3
    float  x0, y0, x1, y1;  // Points used to generate plane equations...
    float  x2, y2, x3, y3;  // ...

    // Construct plane 1
    x0 = getSpanAt(index)->s_node1->s_position.x();
    y0 = getSpanAt(index)->s_node1->s_position.y();
    x3 = getSpanAt(index)->s_node2->s_position.x();
    y3 = getSpanAt(index)->s_node2->s_position.y();
    getPlaneEquation(x0, y0, x3, y3, a1, b1, c1);

    // Construct plane 2
    x1 = x0 + a1 * delta; y1 = y0 + b1 * delta;
    x2 = x0 - a1 * delta; y2 = y0 - b1 * delta;
    getPlaneEquation(x1, y1, x2, y2, a2, b2, c2);

    // Construct plane 3
    x1 = x3 - a1 * delta; y1 = y3 - b1 * delta;
    x2 = x3 + a1 * delta; y2 = y3 + b1 * delta;
    getPlaneEquation(x1, y1, x2, y2, a3, b3, c3);

    // Get the distance away from each plane
    d1 = a1*p.x() + b1*p.y() + c1;
    d2 = a2*p.x() + b2*p.y() + c2;
    d3 = a3*p.x() + b3*p.y() + c3;

    // Try to reject containment
    if(d2 < 0.0f)   return false;
    if(d3 < 0.0f)   return false;
    if(d1 < -delta) return false;
    if(d1 >  delta) return false;

    // An edge was clicked...
    if     (getSpanAt(index)->s_flag == SPuzzleSpan::From1)         type = 1;
    else if(getSpanAt(index)->s_flag == SPuzzleSpan::From2)         type = 2;
    else if(getSpanAt(index)->s_flag == SPuzzleSpan::Bidirectional) type = 3;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::getPlaneEquation
///
/// @description     This function is used to calculate the normalized plane
///                  equation from the two points (x1,y1) and (x2,y2)...
///                  The values return by a, b, and c are the parameters of the
///                  normalized plane equation derived from the line
///                  (x1,y1)----->(x2,x2) where:
///                  0 = ax + by + c
///                  holds for any (x,y) on the plane. Additionaly the
///                  perpendicular distance within either half space of the
///                  plane can be computed by:
///                  dist = ax + by + c
///                  Positive values indicate (x,y) is in the "upper" half space
///                  of the plane. The "upper" half is in the direction of the
///                  normal vector the line segment (x1,y1)---->(x2,y2).
///                  Negative value indicate the opposite. Finally the magnitude
///                  of dist is equal to the length of the line from (x,y) to a
///                  point on the plane such that angle of incidence is 90 degrees.
///
/// @pre             None
/// @post            None
///
/// @param x1:       x coordinate of point 1
/// @param y1:       y coordinate of point 1
/// @param x2:       x coordinate of point 2
/// @param y2:       y coordinate of point 2
/// @param a:        plane constant
/// @param b:        plane constant
/// @param c:        plane constant
///
/// @return void     No return information is given
///
/// @limitations     None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::getPlaneEquation(float x1, float y1, float x2, float y2,
                                         float &a, float &b, float &c) const
{
    float m;
    a = y1 - y2;
    b = x2 - x1;
    c = -1.0f * (a*x1 + b*y1);
    m = 1.0f / sqrtf(a*a + b*b);
    a *= m;
    b *= m;
    c *= m;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::findNode
///
/// @description  This function attempts to find a node at the given point.  If
///               a node is found, the index of the node in the QList of nodes
///               is returned, otherwise, -1 is returned.
/// @pre          None
/// @post         None
///
/// @param p:     This is the point to check for a node at.
///
/// @return int:  Returns the index of the node if found, and -1 otherwise.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
int CPuzzleGraphModel::findNode( QPoint p ) const
{
    float radius = m_style->getNodeRadius();
    radius += m_style->getPen().width() / 2.0;
    int radiusSq = static_cast<int>(radius*radius);

    float bold = radius + m_style->getBoldWidth() / 2.0;
    int boldSq = static_cast<int>(bold*bold);

    QRect quickMid(0, 0, (int)radius*2, (int)radius*2);
    QRect quickEnd(0, 0, (int)bold*2, (int)bold*2);
    quickMid.moveCenter( p );
    quickEnd.moveCenter( p );

    int curRadiusSq, xSq, ySq;
    const SPuzzleNode *curNode;

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop goes through the list of nodes in
    //                reverse order so top most nodes are selected
    //                before lower nodes.  It checks each node for
    //                the given point and returns the first node found
    //                which contains the given point.
    //
    // Precondition:  None
    // Postcondition: If the loop completes without returning from the
    //                function, no node was found at the given point.
    //
    //////////////////////////////////////////////////////////////////
    for( int i = getNodeCount()-1; i >= 0; --i )
    {
        curRadiusSq = 0;
        curNode = getNodeAt( i );
        if( curNode->s_flag & SPuzzleNode::EndNode )
        {
            if( quickEnd.contains( curNode->s_position ) )
                curRadiusSq = boldSq;
        }
        else
        {
            if( quickMid.contains( curNode->s_position ) )
                curRadiusSq = radiusSq;
        }

        if( curRadiusSq > 0.0 )
        {
            xSq = abs(curNode->s_position.x() - p.x());
            xSq *= xSq;

            ySq = abs(curNode->s_position.y() - p.y());
            ySq *= ySq;

            if( (xSq + ySq) < curRadiusSq )
                return i;
        }
    }

    return -1;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::findSpan
///
/// @description   This function attempts to find a span which has the 2 given
///                nodes as end points.  It returns the index of the span if
///                one is found and -1 if no span is found.
/// @pre           None
/// @post          None
///
/// @param node1:  This is the index of one of the nodes for the span.
/// @param node2:  This is the index of the other node for the span.
///
/// @return int:   Returns the index of the span if it is found and -1
///                otherwise.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
int CPuzzleGraphModel::findSpan( int node1, int node2 ) const
{
    SPuzzleNode *pNode1 = m_data->getNodeList()[ node1 ];
    SPuzzleNode *pNode2 = m_data->getNodeList()[ node2 ];
    SPuzzleSpan *pSpan;

    //////////////////////////////////////////////////////////////////
    //
    // Description:   This loop iterates through the span list looking
    //                for a span with the 2 given node endpoints.
    //
    // Precondition:  None
    // Postcondition: If the loop completes without returning from the
    //                function, no span was found.
    //
    //////////////////////////////////////////////////////////////////
    for ( int i = 0; i < m_data->getSpanList().size(); i++ )
    {
        pSpan = m_data->getSpanList()[ i ];

        if ( ( pSpan->s_node1 == pNode1 && pSpan->s_node2 == pNode2 ) ||
             ( pSpan->s_node1 == pNode2 && pSpan->s_node2 == pNode1 ) )
             return i;
    }

    return -1;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::autoName
///
/// @description    Names all nodes. 
/// @pre            None
/// @post           Nodes are named, alphabetically by creation order.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::autoName()
{
    //These variables are used to store the letters used to make the names for
    //the nodes.
    char firstLetter = ( 'A' - 1 ), secondLetter = 'A';
    //This variable is used to store the name of the next node.
    QString name;
    SPuzzleNode * node;
    foreach(node, m_data->getNodeList())
    {
        //Clear the name variable for the next name.
        name.clear();
        //Check if the name should be 2 letters long.  If it should, start with
        //the first letter.
        if ( firstLetter >= 'A' )
            name.append( QChar( firstLetter ) );
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
        node->s_name = name;

    }
    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::autoLayout
///
/// @description    Evens out nodes. 
/// @pre            None
/// @post           Nodes are snapped to a grid.
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::autoLayout()
{
    snapToGrid(50,50);
    emit changed();
}


////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::snapToGrid
///
/// @description    
/// @pre            None
/// @post           None
///
/// @limitations    None
///
///////////////////////////////////////////////////////////////////////////////
void CPuzzleGraphModel::snapToGrid(int x, int y)
{
    if( m_data == NULL || m_data->getNodeList().size() < 2 ) return;
    SPuzzleNode * node;
    QList<SPuzzleNode *> &nodeList = m_data->getNodeList();
    foreach( node, nodeList )
    {
        QPoint p = node->s_position;
        p.setX( x*qRound( qreal( p.x() ) / qreal( x ) ) );
        p.setY( y*qRound( qreal( p.y() ) / qreal( y ) ) );
        node->s_position = p;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::getContainingRect
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
QRect CPuzzleGraphModel::getContainingRect() const
{
    if( m_data == NULL || m_data->getNodeList().size() == 0 )
        return QRect();

    SPuzzleNode *i = m_data->getNodeList()[0];
    QRect ret( i->s_position, QSize( 1,1 ) );

    foreach( i, m_data->getNodeList() )
        ret |= QRect( i->s_position, QSize( 1,1 ) );

    CStyleToolkit *style = CStyleToolkit::instance();
    int adjust = style->getNodeRadius() + style->getBoldWidth() +
                 style->getPen().width();

    ret.adjust( -adjust, -adjust, adjust, adjust );

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::getNodeAt
///
/// @description            This function returns a pointer to the node at the
///                         given index in the QList.
/// @pre                    None
/// @post                   None
///
/// @param index:           This is the index into the QList of the node to be
///                         retrieved.
///
/// @return SPuzzleNode *:  Returns a pointer to the node at the given index
///                         location in the node QList.
///
/// @limitations            None
///
////////////////////////////////////////////////////////////////////////////////
const SPuzzleNode * CPuzzleGraphModel::getNodeAt(int index) const
{
    SPuzzleNode *node = NULL;

    if (m_data != NULL && m_data->getNodeList().size() > index && index >= 0)
    {
        node = m_data->getNodeList().at(index);
    }

    return node;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::getNodeCount
///
/// @description  This function returns the size of the node QList.
/// @pre          None
/// @post         None
///
/// @return int:  Returns the number of nodes in the QList.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
int CPuzzleGraphModel::getNodeCount() const
{
    int size = 0;
    if (m_data != NULL)
    {
        size = m_data->getNodeList().size();
    }
    return size;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::getSpanAt
///
/// @description            This function returns a pointer to the span at the
///                         given index in the QList.
/// @pre                    None
/// @post                   None
///
/// @param index:           This is the index into the QList of the span to be
///                         retrieved.
///
/// @return SPuzzleSpan *:  Returns a pointer to the span at the given index
///                         location in the node QList.
///
/// @limitations            None
///
////////////////////////////////////////////////////////////////////////////////
const SPuzzleSpan * CPuzzleGraphModel::getSpanAt(int index) const
{
    SPuzzleSpan *span = NULL;

    if (m_data != NULL && m_data->getSpanList().size() > index && index >= 0)
    {
        span = m_data->getSpanList().at(index);
    }

    return span;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::getSpanCount
///
/// @description  This function returns the size of the span QList.
/// @pre          None
/// @post         None
///
/// @return int:  Returns the number of spans in the QList.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
int CPuzzleGraphModel::getSpanCount() const
{
    int size = 0;
    if (m_data != NULL)
    {
        size = m_data->getSpanList().size();
    }
    return size;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::getStartNode
///
/// @description            This function returns a pointer to the start node
///                         of the puzzle.
/// @pre                    None
/// @post                   None
///
/// @return SPuzzleNode *:  Returns a pointer to the start node of the puzzle.
///
/// @limitations            None
///
////////////////////////////////////////////////////////////////////////////////
SPuzzleNode * CPuzzleGraphModel::getStartNode() const
{
    SPuzzleNode *node = NULL;
    if ( m_data != NULL )
        node = m_data->getStartNode();

    return node;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::getGoalNodeList
///
/// @description            This function returns a list of pointer to every
///                         goal node of the puzzle.
/// @pre                    None
/// @post                   None
///
/// @return SPuzzleNode *:  List of pointers to all goal nodes in the puzzle.
///
/// @limitations            None
///
////////////////////////////////////////////////////////////////////////////////
QList<SPuzzleNode *> CPuzzleGraphModel::getGoalNodeList() const
{
    QList<SPuzzleNode *> goals;
    SPuzzleNode * node;
    foreach(node, m_data->getNodeList())
    {
        if((node->s_flag & SPuzzleNode::EndNode) != 0)
            goals.append(node);
    }
    return goals;
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::load
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
void CPuzzleGraphModel::load( const QString &filename )
{
    if (m_data == NULL)
    {
        createNewData();
    }
    m_data->load(filename);
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::newNode
///
/// @description  This function creates a new node and adds it to the graph.  It
///               returns the index to the newly created node once it has been
///               added to the graph.
/// @pre          None
/// @post         A new node is added to the graph.
///
/// @return int:  Returns index to the newly created node.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
int CPuzzleGraphModel::newNode()
{
    SPuzzleNode *node = new SPuzzleNode;

    addNode( node );

    return m_data->getNodeList().indexOf( node );
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::newSpan
///
/// @description   This function creates a new span with the given nodes as
///                endpoints and returns the index of the new span.
/// @pre           None
/// @post          A new span is created with the given nodes as endpoints.
///
/// @param node1:  This is the index of one of the nodes for the span.
/// @param node2:  This is the index of the other node for the span.
///
/// @return int:   Returns the index to the new span.
///
/// @limitations   None
///
////////////////////////////////////////////////////////////////////////////////
int CPuzzleGraphModel::newSpan( int node1, int node2 )
{
    SPuzzleSpan *span = new SPuzzleSpan;
    span->s_node1 = m_data->getNodeList()[node1];
    span->s_node2 = m_data->getNodeList()[node2];

    addSpan( span );
    span->s_node1->s_spanList.append(span);
    span->s_node2->s_spanList.append(span);

    return m_data->getSpanList().indexOf( span );
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
void CPuzzleGraphModel::realignData()
{
    QPoint shiftOffset = getContainingRect().topLeft();
    if( shiftOffset.x() >= 0 && shiftOffset.y() >= 0 )
        return;

    if( shiftOffset.x() > 0 )
        shiftOffset.setX( 0 );
    if( shiftOffset.y() > 0 )
        shiftOffset.setY( 0 );

    SPuzzleNode *i;
    foreach( i, m_data->getNodeList() )
    {
        i->s_position -= shiftOffset;
    }

    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
/// CPuzzleGraphModel::save
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
void CPuzzleGraphModel::save( const QString &filename ) const
{
    if (m_data != NULL)
    {
        m_data->save(filename);
    }
}
