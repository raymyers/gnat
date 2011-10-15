////////////////////////////////////////////////////////////////////////////////
/// @file         CSATraceNode.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the ITreeTracer
///               interface class. This is the base class for all tracer
///               algorithms.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _CSATRACENODE_H_
#define _CSATRACENODE_H_

#include "../../CPuzzleGraph.h"
#include <cmath>
#include <QList>
#include <QString>

////////////////////////////////////////////////////////////////////////////////
/// CSATraceNode
///
/// @description  Wrapper class for addressing graph nodes during a single
///               agent trace. In addition to normal node information, it also
///               contains search depth and path cost. A method is provided for
///               expanding into a list of all children in apropriate
///               counterclockwise order.
///
/// @limitations  Children list is computed when node is created and not altered.
///               Presumably the user will not alter the tree in the middle of the trace.
///               If this ever becomes a problem, simply uncomment the first line of the
///               expand method.
///
///               Also, the depth limit reached notification has no counterpart in
///               the backwards expansion.
///
///
////////////////////////////////////////////////////////////////////////////////
class CSATraceNode
{
public:
    CSATraceNode() { m_node = NULL; m_depth = 0; m_cost = 0; }
    CSATraceNode(const SPuzzleNode *node) 
    {
        m_node = node;
        m_depth = 0;
        m_cost = 0;
    //    updateChildrenList();
    }
    CSATraceNode(const SPuzzleNode *node, const int depth, const double cost)
    {
        m_node = node;
        m_depth = depth;
        m_cost = cost;
    //    updateChildrenList();
    }
    ~CSATraceNode() {}

    void updateChildrenList()
    {
        SPuzzleSpan * span;

        // For each edge attached to node.
        foreach( span, m_node->s_spanList )
        {
            // If edge is from node 1 to 2 and this node is
            // node 1, add node 2 to child list.
            if(( span->s_flag & SPuzzleSpan::From1 ) &&
               ( span->s_node1 == m_node ))
            {
                m_children.append(
                    CSATraceNode( span->s_node2,
                                  // Add 1 to depth
                                  m_depth + 1,
                                  // Add edge weight to path cost
                                  m_cost + span->s_weightFrom1 ) );
            }

            // If edge is from node 2 to 1 and this node is
            // node 2, add node 1 to child list.
            if(( span->s_flag & SPuzzleSpan::From2 ) &&
               ( span->s_node2 == m_node ))
            {
                m_children.append(
                    CSATraceNode( span->s_node1,
                                  // Add 1 to depth
                                  m_depth + 1,
                                  // Add edge weight to path cost
                                  m_cost + span->s_weightFrom2 ) );
            }
        }
        int nodeCount = m_children.count();
        QPoint center = m_node->s_position;
        // Bubble sort by relative angle.
        // TODO: Find a way of making QT do this. This is not strait 
        //       forward because the < operator is already being used for
        //       something else. Perhaps a functor, a functoid, or some 
        //       other gawdawfull basterization of a Lisp concept can be used.
        for( int i = 0; i < nodeCount-1; ++i )
        {
            for( int j = 0; j < nodeCount-1-i; ++j )
            {
                if( angle( center, m_children[j].getPoint() ) >
                    angle( center, m_children[j+1].getPoint() ) )
                    m_children.swap(j,j+1);
            }
        }
    } 
    bool goalTest() const { return (m_node->s_flag & SPuzzleNode::EndNode); }
    
    double getCost() const { return m_cost; }
    double getH() const { return m_node->s_heuristic; }

    int getDepth() const { return m_depth; }

    QString getString() const {return m_node->s_name;}

    QPoint getPoint() const {return m_node->s_position;}

    QList<CSATraceNode> expand(const int maxDepth) //const
    {
        bool ignore=true;
        return expand(maxDepth, ignore);
    }
    
    QList<CSATraceNode> expand(const int maxDepth, bool &isDepthlimitReached) //const
    {
        updateChildrenList();
        // If depth limit reached return empty list.
        if( maxDepth >= 0 && maxDepth <= m_depth )
        {
            if(!m_children.empty())
                isDepthlimitReached = true;
            return QList<CSATraceNode>();
        }

        return m_children;
    }

    QList<CSATraceNode> expandBackward(const int maxDepth) const
    {
        QList<CSATraceNode> nodes;
        // If depth limit reached return empty list.
        if( maxDepth >= 0 && maxDepth <= m_depth )
            return nodes;

        SPuzzleSpan * span;

        // For each edge attached to node.
        foreach( span, m_node->s_spanList )
        {
            // If edge is from node 2 to 1 and this node is
            // node 1, add node 2 to child list.
            if(( span->s_flag & SPuzzleSpan::From2 ) &&
               ( span->s_node1 == m_node ))
            {
                nodes.append(
                    CSATraceNode( span->s_node2,
                                  // Add 1 to depth
                                  m_depth + 1,
                                  // Add edge weight to path cost
                                  m_cost + span->s_weightFrom2 ) );
            }

            // If edge is from node 1 to 2 and this node is
            // node 2, add node 1 to child list.
            if(( span->s_flag & SPuzzleSpan::From1 ) &&
               ( span->s_node2 == m_node ))
            {
                nodes.append(
                    CSATraceNode( span->s_node1,
                                  // Add 1 to depth
                                  m_depth + 1,
                                  // Add edge weight to path cost
                                  m_cost + span->s_weightFrom1 ) );
            }
        }
        int nodeCount = nodes.count();
        QPoint center = m_node->s_position;
        // Bubble sort by relative angle.
        // TODO: Find a way of making QT do this. This is not strait 
        //       forward because the < operator is already being used for
        //       something else. Perhaps a functor, a functoid, or some 
        //       other gawdawfull basterization of a Lisp concept can be used.
        for( int i = 0; i < nodeCount-1; ++i )
        {
            for( int j = 0; j < nodeCount-1-i; ++j )
            {
                if( angle( center, nodes[j].getPoint() ) >
                    angle( center, nodes[j+1].getPoint() ) )
                    nodes.swap(j,j+1);
            }
        }
        return nodes;
    }

    friend bool operator==(const CSATraceNode &a, const CSATraceNode &b)
    {
        return a.m_node == b.m_node;
    }

private:
    double angle(const QPoint a, const QPoint b) const
    {
        double pi = 3.1415927;
        double dx = a.x() - b.x();
        double dy = b.y() - a.y();
        double theta;
        if(dx == 0) theta = pi/2;
        else theta = atan(fabs(dy/dx));


        // Modify theta according to quadrant.
        if(dx <= 0 && dy > 0 )
            return pi - theta;
        if(dx <= 0 && dy <= 0 )
            return pi + theta;
        if(dx >= 0 && dy <= 0 )
            return 2*pi - theta;
        return theta;
    }
    QList<CSATraceNode> m_children;
    int m_depth;
    double m_cost;
    const SPuzzleNode *m_node;

};

#endif
