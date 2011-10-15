////////////////////////////////////////////////////////////////////////////////
/// @file         CGeneralMTracer.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the CGeneralMTracer
///               class. This is the class that executes all adversarial traces.
///               The "M" stands for "MiniMax".
///
////////////////////////////////////////////////////////////////////////////////

#include "CGeneralMTracer.h"
#define INF 10000

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::CGeneralMTracer
///
/// @description    This is the constructor of the CGeneralMTracer class.
/// @pre            None
/// @post           Object is created.
///
/// @param          None.
///
/// @return         None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
CGeneralMTracer::CGeneralMTracer()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::generateHistoryTable
///
/// @description    Adds history table entries of node to internal history table,
///                 and recurses on all children.
/// @pre            "node" points to an STreeNode object.
/// @post           History table entries of tree with "node" as the root have
///                 been added to m_historyTable.
///
/// @param node:    Pointer to a node.
///
/// @return         None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CGeneralMTracer::generateHistoryTable( const STreeNode *node ) const
{
    m_historyTable.insert( node, node->s_history );

    const STreeNode *i;
    foreach( i, node->s_childList )
        generateHistoryTable( i );
}

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::moveName
///
/// @description    Get the move required to reach a given node.
/// @pre            node points to a node.
/// @post           Return value indicates the move from from its parent, or
///                 is an empty string if node is the root.
///
/// @param node:    The node in question.
///
/// @return         None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString CGeneralMTracer::moveName( const STreeNode *node ) const
{
    if( node == NULL )
        return QString();
    if( node->s_parent == NULL )
        return QString();

    QString n = node->s_name;
    QString p = node->s_parent->s_name;

    if( n.count() > 1 || p.count() > 1 )
        return QString( "%1-%2" ).arg(p).arg(n);
    return QString( "%1%2" ).arg(p).arg(n);
}

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::sortNodes
///
/// @description    Sort given list of nodes in descending order.
/// @pre
/// @post           nodes is now sorted in order of descending history table
///                 entries.
///
/// @param nodes:   List of pointers to STreeNode object.
///
/// @return         None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CGeneralMTracer::sortNodes( QList<STreeNode *> &nodes) const
{
    if(!m_conf.s_isHT)
        return;
    int nodeCount = nodes.count();
    int i, j, h1, h2;
    for( i = 0; i < nodeCount-1; ++i )
    {
        for( j = 0; j < nodeCount-1-i; ++j )
        {
            h1 = m_historyTable[nodes.at(j)];
            h2 = m_historyTable[nodes.at(j+1)];
            if( h2 > h1 )
                nodes.swap(j,j+1);
        }
    }

}

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::sssHistoryUpdates
///
/// @description    Descends an SSS chain, updating history table accordingly.
/// @pre            node is a pointer to a tree node.
/// @post           String list of history table updates is returned.
///
/// @param node:    Node to begin from.
///        depth:   depth limit to descend.
///        qsDepth: Quiessence Search Depth
///        top:     Indicates weather this is being called from outside.
///                 false is this is a recursive call.
///
/// @return         String list of history table updates.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QStringList CGeneralMTracer::sssHistoryUpdates( const STreeNode *node,
                                           int depth, int qsDepth,
                                           bool top) const
{
    QStringList ret;
    if( !top )
    {
        m_historyTable[node]++;
        ret += moveName( node )
             + ':' + QString::number( m_historyTable[node] );
    }
    if( node->s_childList.count() == 0 )
        return ret;

    if( depth == 0 )
    {
        if( (!node->s_quiescent) || qsDepth == 0 )
            return ret;
        qsDepth--;
    }
    else
        depth--;

    return sssHistoryUpdates( node->s_childList.first(), depth, qsDepth, false )
           + ret;
}

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::checkSSS
///
/// @description    Determines if node can do single successor shortcut.
/// @pre            node points to an STreeNode.
/// @post
///
/// @param node:    Node to start from.
///        depth:   Depth to search to.
///        qsDepth: Quiessance depth.
///
/// @return bool:   true if SSS is apropriate, false otherwise.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
bool CGeneralMTracer::checkSSS( const STreeNode *node,
                                 int depth, int qsDepth ) const
{
    if(!m_conf.s_allowSSS)
        return false;

    if( node->s_childList.count() == 0 )
        return true;

    if( depth == 0 )
    {
        if( (!node->s_quiescent) || qsDepth == 0 )
            return true;
        qsDepth--;
    }
    else
        depth--;

    if( node->s_childList.count() == 1 )
        return checkSSS( node->s_childList.first(), depth, qsDepth );

    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::sssValue
///
/// @description    Performs single successor shortcut and retrieves bottom value.
/// @pre
/// @post
///
/// @param node:    Node to start from.
///        depth:   Depth to search to.
///        qsDepth: Quiessance depth.
///
/// @return float:  value of bottom child node.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
float CGeneralMTracer::sssValue( const STreeNode *node,
                                  int depth, int qsDepth ) const
{
    if( node->s_childList.count() == 0 )
        return node->s_value;

    if( depth == 0 )
    {
        if( (!node->s_quiescent) || qsDepth == 0 )
            return node->s_value;
        qsDepth--;
    }
    else
        depth--;

    return sssValue( node->s_childList.first(), depth, qsDepth );
}

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::trace
///
/// @description    Executes a trace on the root node.
/// @pre            rootNode points to node trace from.
/// @post           returns trace.
///
/// @param rootNode: Node to trace from.
///        depthLimit: Depth Limit.
///        qsDepth: Quiessance depth.
///        traceConf: struct with trace parameters.
///
/// @return STreeTrace *: Pointer to tree trace.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
STreeTrace * CGeneralMTracer::trace( const STreeNode *rootNode,
    int depthLimit, int qsDepth, const STraceConfig &traceConf) const
{
    m_conf = traceConf;

    QString header;

    // Setup collumn headers based on which collumns will be used.

    header = "call|open|eval|value";
    if(m_conf.s_isAB)
        header += "|a,B";

    header += "|best action,value";

    if(!m_conf.s_isQS)
    {
            qsDepth = 0;
    }

    STreeTrace *newTrace = new STreeTrace;
    //newTrace->s_aiName = name();
    newTrace->s_columnNames =
        header.split('|');

    if( rootNode == NULL )
        return newTrace;
    if( rootNode->s_childList.isEmpty() )
        return newTrace;
    if(m_conf.s_isHT)
    {
        m_historyTable.clear();
        generateHistoryTable( rootNode );
    }

    int depth;
    int d = depth = INF;
    STreeCall *curDepth;
    if(m_conf.s_isDL) d = depth = depthLimit;
    if(m_conf.s_isID) d = 1;
    for(; d <= depth; ++d )
    {
        QString call;
        curDepth = new STreeCall;

        if(m_conf.s_isDL)
        {
            call = "DLM";
        }
        else
        {
            call = "Minimax";
        }
        call += QString("(%1")
                .arg(rootNode->s_name);
        if(m_conf.s_isDL)
        {
            call += QString(",%1")
                .arg(d);
        }
        if(m_conf.s_isQS)
        {
            call += QString(",%1")
                .arg(qsDepth);
        }
        if(m_conf.s_isAB)
        {
            ABtraceCall( curDepth, rootNode, d, qsDepth, -INF, INF, true );

            call += ",-INF,INF";
        }
        else
        {
            traceCall( curDepth, rootNode, d, qsDepth, true );
        }
        call += ")";
        ((SGeneralMLine*)(curDepth->s_lines.last()))->s_boxAction = true;
        ((SGeneralMLine*)(curDepth->s_lines.last()))->s_boxValue = false;

        ((SGeneralMLine*)(*curDepth)[0])->s_call = call;

        newTrace->s_depths.append( curDepth );

    }

    return newTrace;
}

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::traceCall
///
/// @description    Executes a trace call on a given node.
/// @pre            rootNode points to node trace from.
/// @post           call is filled with current trace call.
///
/// @param max: true if this is a max node, false for a min.
///        node: Node to trace from.
///        depthLimit: Depth Limit.
///        qsDepth: Quiessance depth.
///        traceConf: struct with trace parameters.
///
/// @return         None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CGeneralMTracer::traceCall( STreeCall *call, const STreeNode *node,
                               int depth, int qsDepth, bool max ) const
{
    if( call == NULL || node == NULL )
        return;
    QList<STreeNode*> curChildren = node->s_childList;
    sortNodes( curChildren );

    QString depthString;
    QString open;
    STreeNode *curNode;
    foreach( curNode, curChildren )
    {
        open += curNode->s_name;
        if(m_conf.s_isHT)
        {
            open += QString::number(m_historyTable[curNode]);
        }
        open += " ";
    }
    open = open.simplified();

    SGeneralMLine *curLine;
    STreeNode *bestAction = NULL;
    float bestValue = -1;
    bool better;
    int curDepth, curQSDepth;
    bool isEndPoint;
    foreach( curNode, curChildren )
    {
        curDepth = depth;
        curQSDepth = qsDepth;

        curLine = new SGeneralMLine;
        curLine->s_conf = m_conf;

        curLine->s_open = open;
        if( m_conf.s_isHT )
            curLine->s_open.remove(' ');
        curLine->s_eval = curNode->s_name;
        open = open.section( ' ', 1, -1 );


        if( curDepth > 0 )
            curDepth--;
        else
            curQSDepth--;
        isEndPoint = false;
        if( (curNode->s_childList.count() == 0)
              ||(curDepth == 0 && (!curNode->s_quiescent || curQSDepth == 0)) )
            isEndPoint = true;

        depthString = "";
        if( m_conf.s_isDL )
        {
            depthString = QString(",%1").arg(curDepth);
        }
        if( m_conf.s_isQS )
        {
            depthString += QString(",%1").arg(curQSDepth);
        }

        curLine->s_valueCall = max?"Min":"Max";
        curLine->s_valueCall += QString("V(%1%2)")
                                .arg(curNode->s_name)
                                .arg(depthString);
        if( isEndPoint )
        {
                curLine->s_valueValue = curNode->s_value;
                curLine->s_sss = false;
        }
        else if( checkSSS( curNode, curDepth, curQSDepth ) )
        {
            curLine->s_valueValue = sssValue( curNode, curDepth, curQSDepth );
            curLine->s_sss = true;
            if( m_conf.s_isHT )
                curLine->s_historyUpdates = sssHistoryUpdates( curNode,
                                                    curDepth, curQSDepth );
        }
        else
        {
            call->s_children.append( new STreeCall() );
            traceCall( call->s_children.last(), curNode, curDepth, curQSDepth, !max );
            curLine->s_valueValue = call->s_children.last()->s_returnValue;
        }
        curLine->s_qs = (curDepth == 0 && curQSDepth > 0
                         && curNode->s_quiescent);

        if( bestAction == NULL )
            better = true;
        else
        {
            if( max )   better = (curLine->s_valueValue > bestValue );
            else        better = (curLine->s_valueValue < bestValue );
        }
        if( better )
        {
            bestAction = curNode;
            bestValue = curLine->s_valueValue;
        }
        curLine->s_bestAction = node->s_name + bestAction->s_name;
        curLine->s_bestValue = bestValue;

        call->s_lines.append( curLine );
    }

    call->s_returnValue = bestValue;
    depthString = "";
    if( m_conf.s_isDL )
    {
        depthString = QString(",%1").arg(depth);
    }
    if( m_conf.s_isQS )
    {
        depthString += QString(",%1").arg(qsDepth);
    }
    ((SGeneralMLine*)call->s_lines.first())->s_call =
        QString(max?"MaxV(%1%2)":"MinV(%1%2)")
        .arg(node->s_name)
        .arg(depthString);
    ((SGeneralMLine*)call->s_lines.last())->s_boxValue = true;
}

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer::ABtraceCall
///
/// @description    Executes an Alpha Beta trace call on a given node.
/// @pre            rootNode points to node trace from.
/// @post           call is filled with current trace call.
///
/// @param max: true if this is a max node, false for a min.
///        node: Node to trace from.
///        depthLimit: Depth Limit.
///        qsDepth: Quiessance depth.
///        alpha: Alpha value.
///        beta: Beta value.
///        traceConf: struct with trace parameters.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CGeneralMTracer::ABtraceCall( STreeCall *call, const STreeNode *node,
                     int depth, int qsDepth, float alpha, float beta, bool max ) const
{
    if( call == NULL || node == NULL )
        return;

    QList<STreeNode*> curChildren = node->s_childList;
    sortNodes( curChildren );

    QString depthString;
    QString open;
    STreeNode *curNode;
    foreach( curNode, curChildren )
    {
        open += curNode->s_name;
        if( m_conf.s_isHT )
        {
            open += QString::number(m_historyTable[curNode]);
        }
        open += " ";
    }
    open = open.simplified();

    SGeneralMLine *curLine;
    STreeNode *bestAction = NULL;
    float bestValue = -1;
    int curDepth, curQSDepth;
    float curAlpha = alpha, curBeta = beta;

    bool better;
    bool isEndPoint;
    foreach( curNode, curChildren )
    {
        bool sssPrune = false;

        curDepth = depth;
        curQSDepth = qsDepth;
        curLine = new SGeneralMLine;

        curLine->s_conf = m_conf;

        curLine->s_open = open;


        if( m_conf.s_isHT )
            curLine->s_open.remove(' ');
        curLine->s_eval = curNode->s_name;
        open = open.section( ' ', 1, -1 );

        if( curDepth > 0 )
            curDepth--;
        else
            curQSDepth--;

        depthString = "";
        isEndPoint = false;
        if( (curNode->s_childList.count() == 0)
                  ||(curDepth == 0 && (!curNode->s_quiescent || curQSDepth == 0)) )
            isEndPoint = true;

        if( m_conf.s_isDL )
        {
            depthString = QString(",%1").arg(curDepth);
        }
        if( m_conf.s_isQS )
        {
            depthString += QString(",%1").arg(curQSDepth);
        }
        curLine->s_valueCall = max?"Min":"Max";
        curLine->s_valueCall += QString("V(%1%2,%3,%4)")
                                .arg(curNode->s_name)
                                .arg(depthString)
                                .arg(SGeneralMLine::numberToString(curAlpha))
                                .arg(SGeneralMLine::numberToString(curBeta));
        if( isEndPoint )
        {
            curLine->s_valueValue = curNode->s_value;
            curLine->s_sss = false;
        }
        else if( checkSSS( curNode, curDepth, curQSDepth ) )
        {
            curLine->s_valueValue =
                sssValue( curNode, curDepth, curQSDepth );

            curLine->s_sss = true;
            if(m_conf.s_isHT)
                curLine->s_historyUpdates =
                    sssHistoryUpdates( curNode, curDepth, curQSDepth );

        }
        else
        {
            call->s_children.append( new STreeCall() );
            ABtraceCall( call->s_children.last(), curNode,
                       curDepth, curQSDepth,
                       curAlpha, curBeta, !max );
            curLine->s_valueValue = call->s_children.last()->s_returnValue;
        }
        curLine->s_qs = (curDepth == 0 && curQSDepth > 0
                         && curNode->s_quiescent);
        if( curLine->s_sss )
        {
            if( curLine->s_valueValue <= curAlpha )
                sssPrune = true;
            else if( curLine->s_valueValue >= curBeta )
                sssPrune = true;
        }

        if( bestAction == NULL )
            better = true;
        else
        {
            if( max )   better = (curLine->s_valueValue > bestValue );
            else        better = (curLine->s_valueValue < bestValue );
        }
        if( better )
        {
            bestAction = curNode;
            bestValue = curLine->s_valueValue;

            if( max )
            {
                if( bestValue >= curBeta )
                    curLine->s_prune = true;
                else if( bestValue > curAlpha )
                    curAlpha = bestValue;
            }
            else
            {
                if( bestValue <= curAlpha )
                    curLine->s_prune = true;
                else if( bestValue < curBeta )
                    curBeta = bestValue;
            }
        }

        curLine->s_alpha = curAlpha;
        curLine->s_beta = curBeta;

        curLine->s_bestAction = node->s_name + bestAction->s_name;
        curLine->s_bestValue = bestValue;

        call->s_lines.append( curLine );

        if( curLine->s_prune )
        {
            break;
        }
        if( sssPrune )
        curLine->s_prune = true;
    }
    if( m_conf.s_isHT )
    {
        m_historyTable[bestAction]++;
        ((SGeneralMLine*)call->s_lines.last())->s_historyUpdates
            += moveName( bestAction )
               + ':' + QString::number( m_historyTable[bestAction] );
    }

    call->s_returnValue = bestValue;
    depthString = "";
    if( m_conf.s_isDL )
    {
        depthString = QString(",%1").arg(depth);
    }
    if( m_conf.s_isQS )
    {
        depthString += QString(",%1").arg(qsDepth);
    }
    ((SGeneralMLine*)call->s_lines.first())->s_call =
        QString(max?"MaxV(%1%2,%3,%4)":"MinV(%1%2,%3,%4)")
        .arg(node->s_name)
        .arg(depthString)
        .arg(SGeneralMLine::numberToString(alpha))
        .arg(SGeneralMLine::numberToString(beta));
    ((SGeneralMLine*)call->s_lines.last())->s_boxValue = true;
}

////////////////////////////////////////////////////////////////////////////////
/// SGeneralMLine::SGeneralMLine
///
/// @description    This is the constructor of the SGeneralMLine class.
/// @pre            None
/// @post           Object is created.
///
/// @param          None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
SGeneralMLine::SGeneralMLine()
{
    s_call = QString();
    s_open = QString();
    s_eval = QString();

    s_valueCall = QString();
    s_valueValue = 0.0;
    s_qs = false;
    s_sss = false;
    s_prune = false;

    s_alpha = -INF;
    s_beta = INF;

    s_bestAction = QString();
    s_boxAction = false;
    s_bestValue = 0.0;
    s_boxValue = false;
}

////////////////////////////////////////////////////////////////////////////////
/// SGeneralMLine::~SGeneralMLine
///
/// @description    This is the destructor of the SGeneralMLine class.
/// @pre            None
/// @post           Object is destroyed.
///
/// @param          None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
SGeneralMLine::~SGeneralMLine()
{
}

////////////////////////////////////////////////////////////////////////////////
/// SGeneralMLine::numberToString
///
/// @description    Converts number to string.
/// @pre            None
/// @post           returns string that is either the number, "INF", or "-INF"
///
/// @param val:     number to convert.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString SGeneralMLine::numberToString( float val )
{
    if( val == INF )
        return QString("INF");
    if( val == -INF )
        return QString("-INF");
    return QString::number(val);
}

////////////////////////////////////////////////////////////////////////////////
/// SGeneralMLine::operator []
///
/// @description
/// @pre            None
/// @post
///
/// @param index:   index of collumn.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QString SGeneralMLine::operator [](int index)
{
    QString ret;

    switch( index )
    {
    case 0: //  CALL
        ret = s_call;
        break;
    case 1: //  OPEN
        ret = s_open;
        break;
    case 2: //  EVAL
        ret = s_eval;
        break;
    case 3: //  VALUE
        ret = s_valueCall;
        ret += "=" + QString::number( s_valueValue );
        if( s_qs || s_sss || s_prune)
        {
            ret += " (";
            if( s_qs )
            {
                ret += "QS";
                if( s_sss || s_prune)
                    ret += ",";
            }
            if( s_sss )
            {
                ret += "SSS";
                if( s_prune )
                    ret += ",";
            }
            if( s_prune )
                ret += "Prune";
            ret += ")";
        }
        break;
    case 4: //  a,b
        ret += numberToString(s_alpha);
        ret += ", ";
        ret += numberToString(s_beta);
        break;
    case 5: //  BEST ACTION,VALUE
        if( s_boxAction )
            ret = QString("[%1], ").arg(s_bestAction);
        else
            ret = s_bestAction + ", ";
        if( s_boxValue )
            ret += QString("[%1]").arg(s_bestValue);
        else
            ret += QString::number(s_bestValue);
            if( !s_historyUpdates.isEmpty() )
                ret += QString(" {%1}").arg(s_historyUpdates.join(","));
        break;
    default:
        break;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// SGeneralMLine::toStringList
///
/// @description
/// @pre            None
/// @post
///
/// @param          None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
QStringList SGeneralMLine::toStringList()
{
    QStringList ret;

    for( int i = 0; i < 6; ++i )
    {
        if(i == 4 && !s_conf.s_isAB) continue;

        ret += (*this)[i];
    }
    return ret;
}
