///////////////////////////////////////////////////////////////////////////////
/// @file         IOpenList.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the ITreeTracer
///               interface class and all derived classes.
///
////////////////////////////////////////////////////////////////////////////////
#include "IOpenList.h"

////////////////////////////////////////////////////////////////////////////////
/// CDFOpenList::getString
///
/// @description    Get string representation of open list.
/// @pre            None.
/// @post           String representation of open list is returned.
///
/// @return QString : String representation of open list.
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString CDFOpenList::getString() const
{
    QString ret;
    CSATraceNode i;
    foreach(i, m_data)
        ret = ret + i.getString() + " ";
    return ret.simplified();
}

////////////////////////////////////////////////////////////////////////////////
/// CDFOpenList::getEvalString
///
/// @description    Get string representation of eval node.
/// @pre            None.
/// @post           String representation of eval node is returned.
///
/// @return QString : String representation of eval node.
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString CDFOpenList::getEvalString() const
{
    return m_data.front().getString();
}

////////////////////////////////////////////////////////////////////////////////
/// CDFOpenList::push
///
/// @description    Adds node to open list.
/// @pre            None.
/// @post           node is prepended to m_data, unless m_data allready contains it.
///
/// @param node:    Node to add.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CDFOpenList::push( CSATraceNode node )
{
    CSATraceNode i;
    //if(!m_data.contains( node ));
    m_data.prepend( node );
}

////////////////////////////////////////////////////////////////////////////////
/// CDFOpenList::push
///
/// @description    Adds nodes to open list.
/// @pre            None.
/// @post           Each node supplied that m_data doesn't already contain is
///                 prepended to m_data, preserving order.
///
/// @param nodes:    Nodes to add.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CDFOpenList::push( QList<CSATraceNode> nodes )
{
    CSATraceNode i;
    //foreach( i, m_data )
    //    nodes.removeAll(i);
    m_data = nodes + m_data;
}

////////////////////////////////////////////////////////////////////////////////
/// CBFOpenList::getString
///
/// @description    Get string representation of open list.
/// @pre            None.
/// @post           String representation of open list is returned.
///
/// @return QString : String representation of open list.
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString CBFOpenList::getString() const
{
    QString ret;
    CSATraceNode i;
    foreach(i, m_data)
        ret = ret + i.getString() + " ";
    return ret.simplified();
}

////////////////////////////////////////////////////////////////////////////////
/// CBFOpenList::getEvalString
///
/// @description    Get string representation of eval node.
/// @pre            None.
/// @post           String representation of eval node is returned.
///
/// @return QString : String representation of eval node.
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString CBFOpenList::getEvalString() const
{
    return m_data.front().getString();
}

////////////////////////////////////////////////////////////////////////////////
/// CBFOpenList::push
///
/// @description    Adds node to open list.
/// @pre            None.
/// @post           node is appended to m_data, unless m_data allready contains it.
///
/// @param node:    Node to add.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CBFOpenList::push( CSATraceNode node )
{
    CSATraceNode i;
    //if(!m_data.contains( node ));
        m_data.append( node );
}

////////////////////////////////////////////////////////////////////////////////
/// CBFOpenList::push
///
/// @description    Adds nodes to open list.
/// @pre            None.
/// @post           Each node supplied that m_data doesn't already contain is
///                 appended to m_data, in order.
///
/// @param nodes:    Nodes to add.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CBFOpenList::push( QList<CSATraceNode> nodes )
{
    CSATraceNode i;
    //foreach( i, m_data )
      //  nodes.removeAll(i);
    m_data += nodes;
}


////////////////////////////////////////////////////////////////////////////////
/// IInformedSearchOpenList::getString
///
/// @description    Get string representation of open list.
/// @pre            None.
/// @post           String representation of open list is returned.
///
/// @return QString : String representation of open list.
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString IInformedSearchOpenList::getString() const
{
    QString ret;
    CSATraceNode i;
    foreach(i, m_data)
        ret = ret + i.getString() + QString::number( nodeVal(i) ) + " ";
    return ret.simplified();
}

////////////////////////////////////////////////////////////////////////////////
/// IInformedSearchOpenList::getEvalString
///
/// @description    Get string representation of eval node.
/// @pre            None.
/// @post           String representation of eval node is returned.
///
/// @return QString : String representation of eval node.
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString IInformedSearchOpenList::getEvalString() const
{
    return m_data.front().getString() +
           QString::number( nodeVal( m_data.front() ) );
}

////////////////////////////////////////////////////////////////////////////////
/// IInformedSearchOpenList::push
///
/// @description    Adds node to open list.
/// @pre            None.
/// @post           node is inserted to m_data, maintaining sorted order
///                 unless m_data allready contains it. If m_data contains it,
///                 but with a higher nodeVal, new better node will relace it.
///
/// @param node:    Node to add.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void IInformedSearchOpenList::push( CSATraceNode node )
{
    int i;
    //foreach(i, m_data)
    //    if( i == node )
    for( i=0; i < m_data.size(); i++ )
        if( m_data[i] == node ) {
            if( nodeVal( node ) < nodeVal( m_data[i] ) ) 
                m_data.replace(i, node);
            return;
        }
    
    for( i=0; i < m_data.size(); i++ ) 
        if( nodeVal( node ) <  nodeVal( m_data[i] ) )
            break;
    m_data.insert( i, node );

}

////////////////////////////////////////////////////////////////////////////////
/// IInformedSearchOpenList::push
///
/// @description    Adds nodes to open list.
/// @pre            None.
/// @post           Attempts to add each node to list, via the single node
///                 push method.
///
/// @param nodes:    Nodes to add.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void IInformedSearchOpenList::push( QList<CSATraceNode> nodes )
{
    CSATraceNode i;//,j;
    foreach( i, nodes )
        push(i);
}

////////////////////////////////////////////////////////////////////////////////
/// CClosedList::getString
///
/// @description    Get string representation of closed list.
/// @pre            None.
/// @post           String representation of closed list is returned.
///
/// @return QString : String representation of closed list.
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString CClosedList::getString()
{
    QString ret;
    CSATraceNode i;
    foreach(i, m_data)
        ret = ret + i.getString() + " ";
    return ret.simplified();
}

////////////////////////////////////////////////////////////////////////////////
/// CClosedList::push
///
/// @description    Adds node to closed list.
/// @pre            None.
/// @post           Node is appended to closed list.
///
/// @param node:    Node to add.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CClosedList::push( CSATraceNode node ) { m_data.append( node ); }

////////////////////////////////////////////////////////////////////////////////
/// CClosedList::strip
///
/// @description
/// @pre
/// @post
///
/// @param nodes:
///
/// @return STreeTrace *: This is the trace generated by the ITreeTracer
///                       subclass.
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QList<CSATraceNode> CClosedList::strip( QList<CSATraceNode> nodes ) const
{
    QList<CSATraceNode> ret = nodes;
    CSATraceNode i;
    foreach( i, m_data )
        ret.removeAll(i);
    return ret;
}
