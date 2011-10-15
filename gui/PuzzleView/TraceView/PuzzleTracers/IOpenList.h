///////////////////////////////////////////////////////////////////////////////
/// @file         IOpenList.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration for all open and closed list
///               classes used in single agent searches.
///
///               This includes the interface classes:
///               IOpenList, and IInformedSearchOpenList
///
///               and the classes:
///               CDFOpenList (Depth First Search)
///               CDFOpenList (Breadth First Search)
///               CUCOpenList (Uniform Cost Search)
///               CGBFOpenList (Greedy Best First Search)
///               CAStarOpenList (A*)
///
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _IOPENLIST_H_
#define _IOPENLIST_H_

#include "CSATraceNode.h"
#include <QString>
#include <QList>

////////////////////////////////////////////////////////////////////////////////
/// IOpenList
///
/// @description  This is the interface class to all the Single Agent trace open lists.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class IOpenList
{
public:
    IOpenList() {}

    virtual ~IOpenList() {}

    virtual QString getString() const = 0;

    virtual CSATraceNode top() const { return m_data.front(); }

    virtual void pop() { m_data.pop_front(); }

    virtual void push( CSATraceNode node ) = 0;

    virtual void push( QList<CSATraceNode> node ) = 0;

    virtual bool empty() {return m_data.empty();}

    virtual void removeDuplicates() 
    {
        QList<CSATraceNode> unique;
        for(int i = m_data.size()-1; i>=0; i--) {
            CSATraceNode node = m_data[i];
            if(! unique.contains(node)) unique.prepend(node);
        }
        m_data = unique;
    }

protected:
    QList<CSATraceNode> m_data;
};

////////////////////////////////////////////////////////////////////////////////
/// CDFOpenList
///
/// @description  Open list for Depth First search. New nodes are pushed onto the
///               beginning of the list.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CDFOpenList : public IOpenList
{
public:
    CDFOpenList() {}

    ~CDFOpenList() {}

    QString getString() const;

    QString getEvalString() const;

    void push( CSATraceNode node );

    void push( QList<CSATraceNode> nodes );
};

////////////////////////////////////////////////////////////////////////////////
/// CBFOpenList
///
/// @description  Open list for Breadth First search. New nodes are pushed onto the
///               end of the list.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CBFOpenList : public IOpenList
{
public:
    CBFOpenList() {}

    ~CBFOpenList() {}

    QString getString() const;

    QString getEvalString() const;

    void push( CSATraceNode node );

    void push( QList<CSATraceNode> nodes );
};

////////////////////////////////////////////////////////////////////////////////
/// IInformedSearchOpenList
///
/// @description  Interface class for the open list of informed searches. List is
///               sorted by node values, as defined by the derived classes.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class IInformedSearchOpenList : public IOpenList
{
public:
    IInformedSearchOpenList() {}

    ~IInformedSearchOpenList() {}

    QString getString() const;

    QString getEvalString() const;

    void push( CSATraceNode node );

    void push( QList<CSATraceNode> nodes );

protected:

    virtual double nodeVal( const CSATraceNode &n) const = 0;
};

////////////////////////////////////////////////////////////////////////////////
/// CUCOpenList
///
/// @description  Open list for Uniform Cost search. List is sorted by path costs.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CUCOpenList : public IInformedSearchOpenList
{
public:
    CUCOpenList() {}

    ~CUCOpenList() {}

protected:
    double nodeVal( const CSATraceNode &n) const
    {
        return n.getCost();
    }
};

////////////////////////////////////////////////////////////////////////////////
/// CGBFOpenList
///
/// @description  Open list for Greedy Best First search. List is sorted by
///               heuristic values.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CGBFOpenList : public IInformedSearchOpenList
{
public:
    CGBFOpenList() {}

    ~CGBFOpenList() {}

protected:
    double nodeVal( const CSATraceNode &n) const
    {
        return n.getH();
    }
};

////////////////////////////////////////////////////////////////////////////////
/// CAStarOpenList
///
/// @description  Open list for A* search. List is sorted by sum of path costs and
///               heuristic values.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CAStarOpenList : public IInformedSearchOpenList
{
public:
    CAStarOpenList() {}

    ~CAStarOpenList() {}

protected:
    double nodeVal( const CSATraceNode &n) const
    {
        return n.getH() + n.getCost();
    }
};


////////////////////////////////////////////////////////////////////////////////
/// CClosedList
///
/// @description  Closed list for graph searches.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class CClosedList
{
public:
    CClosedList() {}

    ~CClosedList() {}

    QString getString();

    void push( CSATraceNode node );

    //void push( QList<CSATraceNode> nodes ) { m_data = nodes + m_data; }

    QList<CSATraceNode> strip( QList<CSATraceNode> nodes ) const;

private:
    QList<CSATraceNode> m_data;
};
#endif
