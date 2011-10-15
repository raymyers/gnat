////////////////////////////////////////////////////////////////////////////////
/// @file         CExpectMTracer.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the CExpectMTracer
///               class. This is the class that executes all adversarial traces.
///               The "M" stands for "MiniMax".
///
////////////////////////////////////////////////////////////////////////////////

#include "CExpectMTracer.h"
#define INF 10000
#define MIN(a,b) ((a) <= (b) ? (a) : (b))
#define MAX(a,b) ((a) >= (b) ? (a) : (b))

////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer::CExpectMTracer
///
/// @description    This is the constructor of the CExpectMTracer class.
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
CExpectMTracer::CExpectMTracer()
{
}

////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer::generateHistoryTable
///
/// @description    Adds history table entries of node to internal history table,
///                 and recurses on all children.
/// @pre            "node" points to an SChanceTreeNode object.
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
void CExpectMTracer::generateHistoryTable( const SChanceTreeNode *node ) const
{
    m_historyTable.insert( node, node->s_history );

    const SChanceTreeNode *i;
    foreach( i, node->s_childList )
        generateHistoryTable( i );
}

////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer::moveName
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
QString CExpectMTracer::moveName( const SChanceTreeNode *node ) const
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
/// CExpectMTracer::sortNodes
///
/// @description    Sort given list of nodes in descending order.
/// @pre
/// @post           nodes is now sorted in order of descending history table
///                 entries.
///
/// @param nodes:   List of pointers to SChanceTreeNode object.
///
/// @return         None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
void CExpectMTracer::sortNodes( QList<SChanceTreeNode *> &nodes) const
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
/// CExpectMTracer::sssHistoryUpdates
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
QStringList CExpectMTracer::sssHistoryUpdates( const SChanceTreeNode *node,
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

    if( !node->s_chance )
    {
        if( depth == 0 )
        {
            if( (!node->s_quiescent) || qsDepth == 0 )
                return ret;
            qsDepth--;
        }
        else
            depth--;
    }


    return sssHistoryUpdates( node->s_childList.first(), depth, qsDepth, false )
           + ret;
}

////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer::checkSSS
///
/// @description    Determines if node can do single successor shortcut.
/// @pre            node points to an SChanceTreeNode.
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
bool CExpectMTracer::checkSSS( const SChanceTreeNode *node,
                                 int depth, int qsDepth ) const
{
    if(!m_conf.s_allowSSS)
        return false;

    if( node->s_childList.count() == 0 )
        return true;

    if( !node->s_chance )
    {
        if( depth == 0 )
        {
            if( (!node->s_quiescent) || qsDepth == 0 )
                return true;
            qsDepth--;
        }
        else
            depth--;
    }

    if( node->s_childList.count() == 1 )
        return checkSSS( node->s_childList.first(), depth, qsDepth );

    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer::sssValue
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
float CExpectMTracer::sssValue( const SChanceTreeNode *node,
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
/// CExpectMTracer::trace
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
/// @return SChanceTreeTrace *: Pointer to tree trace.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
SChanceTreeTrace * CExpectMTracer::trace( const SChanceTreeNode *rootNode,
    int depthLimit, int qsDepth, const STraceConfig &traceConf,
    qreal lowerBound, qreal upperBound) const
{
    m_conf = traceConf;

    QString header;

    // Setup collumn headers based on which collumns will be used.
    m_lowerBound = lowerBound;
    m_upperBound = upperBound;
    //header = "call|open|eval|value";
    header = "call|open|value";
    if(m_conf.s_isCP)
        header += "|LB,UB";
    if(m_conf.s_isAB)
        header += "|a,B";

    header += "|best action,value";

    if(!m_conf.s_isQS)
    {
            qsDepth = 0;
    }

    SChanceTreeTrace *newTrace = new SChanceTreeTrace;
    //newTrace->s_aiName = name();
    newTrace->s_columnNames =
        header.split('|');
// Uncomment this when stuff will work
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
    SChanceTreeCall *curDepth;
    if(m_conf.s_isDL) d = depth = depthLimit;
    if(m_conf.s_isID) d = 1;
    for(; d <= depth; ++d )
    {
        QString call;
        curDepth = new SChanceTreeCall;

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
            //broken?
            //return newTrace;
            bool success = ABtraceCall( curDepth, rootNode, d, qsDepth, 
                           lowerBound, upperBound, true );
            if (!success)
            {
                delete newTrace;
                return new SChanceTreeTrace;
            }

            call += QString( ",%1,%2" ).arg(lowerBound).arg(upperBound);
            //call += ",-INF,INF";
        }
        else
        {
            bool success = traceCall( curDepth, rootNode, d, qsDepth, true );
            if (!success)
            {
                delete newTrace;
                return new SChanceTreeTrace;
            }
        }
        call += ")";
        ((SExpectMLine*)(curDepth->s_lines.last()))->s_boxAction = true;
        ((SExpectMLine*)(curDepth->s_lines.last()))->s_boxValue = false;

        ((SExpectMLine*)(*curDepth)[0])->s_call = call;

        newTrace->s_depths.append( curDepth );

    }

    return newTrace;
}
////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer::traceECall
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
bool CExpectMTracer::traceECall( SChanceTreeCall *call, const SChanceTreeNode *node,
                               int depth, int qsDepth, bool max ) const
{
    if(call == NULL || node == NULL || !node->s_chance )
        return false;
    QList<SChanceTreeNode*> curChildren = node->s_childList;
    sortNodes( curChildren );
    QString depthString;
    QString open;
    //QString sumString;
    SChanceTreeNode *curNode;
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

    SExpectMLine *curLine;
    //SChanceTreeNode *bestAction = NULL;
    qreal sum = 0.0;
    int curDepth, curQSDepth;
    bool isEndPoint;
    foreach( curNode, curChildren )
    {
        curDepth = depth;
        curQSDepth = qsDepth;

        curLine = new SExpectMLine;
        curLine->s_conf = m_conf;
        curLine->s_chance = true;

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

        curLine->s_valueCall = max?"Max":"Min";
        curLine->s_valueCall += QString("(%1%2)")
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
            call->s_children.append( new SChanceTreeCall() );
            bool success = traceCall( call->s_children.last(), curNode, curDepth, curQSDepth, max );
            if(!success)
            {
                delete curLine;
                return false;
            }
            curLine->s_valueValue = call->s_children.last()->s_returnValue;
        }
        curLine->s_qs = (curDepth == 0 && curQSDepth > 0
                         && curNode->s_quiescent);

        qreal val = curLine->s_valueValue;
        qreal prob = curNode->s_probability / 100.0;
        sum += prob*val;
        //sumString += QString("%1*%2 + ").arg(prob).arg(val);
        //curLine->s_bestAction = QString("%1*%2=%3")
        //                        .arg(prob)
        //                        .arg(val)
        //                        .arg(prob*val);
        curLine->s_bestValue = 0;

        call->s_lines.append( curLine );
    }
    //sumString.chop(3); // Remove trailing " + ".
    //sumString += QString("=%1").arg(sum);
    //curLine = new SExpectMLine;
    //curLine->s_valueValue = sum;
    //curLine->s_conf = m_conf;
    //curLine->s_valueCall = sumString;
    //curLine->s_chance = true;
    //curLine->s_open = " "; // Prevent this line from being deleted later.
    
    //curLine->s_bestAction = sumString;
    //call->s_lines.append( curLine );  
    curLine->s_bestAction = QString("[%1]").arg(SExpectMLine::numberToString(sum));
    call->s_returnValue = sum;
    depthString = "";
    if( m_conf.s_isDL )
    {
        depthString = QString(",%1").arg(depth);
    }
    if( m_conf.s_isQS )
    {
        depthString += QString(",%1").arg(qsDepth);
    }
    ((SExpectMLine*)call->s_lines.first())->s_call =
        QString(max?"MinChance(%1%2)":"MaxChance(%1%2)")
        .arg(node->s_name)
        .arg(depthString);
    ((SExpectMLine*)call->s_lines.last())->s_boxValue = true;
    return true;
}


////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer::traceCall
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
bool CExpectMTracer::traceCall( SChanceTreeCall *call, const SChanceTreeNode *node,
                               int depth, int qsDepth, bool max ) const
{
    if( call == NULL || node == NULL || node->s_chance || node->s_childList.size() == 0)
        return false;
    QList<SChanceTreeNode*> curChildren = node->s_childList;
    sortNodes( curChildren );

    QString depthString;
    QString open;
    SChanceTreeNode *curNode;
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

    SExpectMLine *curLine;
    SChanceTreeNode *bestAction = NULL;
    float bestValue = -1;
    bool better;
    bool isEndPoint;
    foreach( curNode, curChildren )
    {

        curLine = new SExpectMLine;
        curLine->s_conf = m_conf;

        curLine->s_open = open;
        if( m_conf.s_isHT )
            curLine->s_open.remove(' ');
        curLine->s_eval = curNode->s_name;
        open = open.section( ' ', 1, -1 );


        isEndPoint = false;
        if( curNode->s_childList.count() == 0 )
            isEndPoint = true;

        depthString = "";
        if( m_conf.s_isDL )
        {
            depthString = QString(",%1").arg(depth);
        }
        if( m_conf.s_isQS )
        {
            depthString += QString(",%1").arg(qsDepth);
        }

        curLine->s_valueCall = max?"MaxChance":"MinChance";
        curLine->s_valueCall += QString("(%1%2)")
                                .arg(curNode->s_name)
                                .arg(depthString);
        if( isEndPoint )
        {
                curLine->s_valueValue = curNode->s_value;
                curLine->s_sss = false;
        }
        else if( checkSSS( curNode, depth, qsDepth ) )
        {
            curLine->s_valueValue = sssValue( curNode, depth, qsDepth );
            curLine->s_sss = true;
            if( m_conf.s_isHT )
                curLine->s_historyUpdates = sssHistoryUpdates( curNode,
                                                    depth, qsDepth );
        }
        else
        {
            call->s_children.append( new SChanceTreeCall() );
            bool success = traceECall( call->s_children.last(), 
                                       curNode, depth, qsDepth, !max );
            if(!success)
            {
                delete curLine;
                return false;
            }
            curLine->s_valueValue = call->s_children.last()->s_returnValue;
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
        }
        curLine->s_bestAction = node->s_name + bestAction->s_name;
        curLine->s_bestValue = bestValue;

        call->s_lines.append( curLine );
    }
    if( m_conf.s_isHT )
    {
        m_historyTable[bestAction]++;
        ((SExpectMLine*)call->s_lines.last())->s_historyUpdates
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
    ((SExpectMLine*)call->s_lines.first())->s_call =
        QString(max?"Max(%1%2)":"Min(%1%2)")
        .arg(node->s_name)
        .arg(depthString);
    ((SExpectMLine*)call->s_lines.last())->s_boxValue = true;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer::ABtraceECall
///
/// @description    Executes a trace call on a given node.
/// @pre            rootNode points to node trace from.
/// @post           call is filled with current trace call.
///
/// @param max: true if this is a max chance node, false for a min.
///        node: Node to trace from.
///        depthLimit: Depth Limit.
///        qsDepth: Quiessance depth.
///        alpha: Alpha value.
///        beta: Beta value.
///
/// @return         None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
bool CExpectMTracer::ABtraceECall( SChanceTreeCall *call, const SChanceTreeNode *node,
                     int depth, int qsDepth, float alpha, float beta, bool max ) const
{
    if(call == NULL || node == NULL || !node->s_chance || node->s_childList.size() == 0)
        return false;
    QList<SChanceTreeNode*> curChildren = node->s_childList;
    sortNodes( curChildren );
    QString depthString;
    QString open;
    bool failSoft = false;
    //QString boundPrefix = max ? "LB=" : "UB=";
    //QString sumString;
    //QString calcString;
    SChanceTreeNode *curNode;
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

    SExpectMLine *curLine;
    //SChanceTreeNode *bestAction = NULL;
    float curAlpha = alpha, curBeta = beta;
    qreal n = curChildren.size();
    qreal u = m_upperBound;
    qreal l = m_lowerBound;
    //qreal a = numChildren * (alpha - u) + u;
    //qreal b = numChildren * (beta  - l) + l;
    qreal v = 0;
    qreal x = 0; // This is X1 
    qreal y = 1; // This is Y0
    qreal sum = 0.0;	
    qreal probSum = 0.0;
    qreal bound;
    qreal curLowerBound = m_lowerBound;
    qreal curUpperBound = m_upperBound;
    int curDepth, curQSDepth;
    bool isEndPoint;
    foreach( curNode, curChildren )
    {
        qreal prob = curNode->s_probability / 100.0;
        bool sssPrune = false;
        curDepth = depth;
        curQSDepth = qsDepth;
        y = y - prob;
        qreal ax = (alpha  - u * y - x) / prob;
        qreal bx = (beta   - l * y - x) / prob;
        ax = MAX(ax, l); 
        bx = MIN(bx, u); 
        // "x = x + v" will happen once v is established.
        curLine = new SExpectMLine;
        curLine->s_conf = m_conf;
        curLine->s_chance = true;
        curLine->s_alpha = alpha;
        curLine->s_beta = beta;

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

        curLine->s_valueCall = max?"Max":"Min";
        curLine->s_valueCall += QString("(%1%2,%3,%4)")
                                .arg(curNode->s_name)
                                .arg(depthString)
                                .arg(SExpectMLine::numberToString(ax))
                                .arg(SExpectMLine::numberToString(bx));
        if( isEndPoint )
        {
                v = curLine->s_valueValue = curNode->s_value;
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
            call->s_children.append( new SChanceTreeCall() );
            bool success = ABtraceCall( call->s_children.last(), curNode, curDepth,
                                        curQSDepth, ax, bx, max );
            if(!success)
            {
                delete curLine;
                return false;
            }
            v = curLine->s_valueValue = call->s_children.last()->s_returnValue;
            //x += prob * v;
            //y -= prob; Happened earlier
        }
        x += prob * v;
        //y -= prob; Happened earlier
        curLine->s_qs = (curDepth == 0 && curQSDepth > 0
                         && curNode->s_quiescent);
        if( curLine->s_sss )
        {
            if( curLine->s_valueValue <= curAlpha )
                sssPrune = true;
            else if( curLine->s_valueValue >= curBeta )
                sssPrune = true;
        }
        
        sum = x;
        probSum += prob;
        //sumString += QString("%1*%2 + ").arg(prob).arg(val);

        curLowerBound = sum+(y)*m_lowerBound;
        curUpperBound = sum+(y)*m_upperBound;
        curLine->s_bounds = QString("%1, %2").arg(curLowerBound).arg(curUpperBound);
        /*
        if(probSum < 0.999)
        {
            calcString = sumString + QString("%1*%2=%3")
                                     .arg(1.0-probSum)
                                     .arg(bound)
                                     .arg(valueBound);
        }
        else
        {
            calcString = sumString;
            calcString.chop(3); // Remove trailing " + "
            calcString += QString("=%1").arg(sum);
        }
        curLine->s_bestAction = calcString; */
        curLine->s_bestValue = 0;

        if( v >= bx )
        {
            curLine->s_cPrune = true;
            call->s_returnValue = curLowerBound;
            call->s_returnValue = curUpperBound;
        }
	//else if( valueBound > curAlpha )
	//    curAlpha = valueBound;
        else if( v <= ax )
        {
            curLine->s_cPrune = true;
            call->s_returnValue = curUpperBound;
            call->s_returnValue = curLowerBound;
        }
            //else if( valueBound < curBeta )
            //    curBeta = valueBound;
        //curLine->s_alpha = curAlpha;
        //curLine->s_beta = curBeta;

        call->s_lines.append( curLine );
        
        if( sssPrune )
            curLine->s_prune = true;
	//curLine->s_bestAction = QString( curLine->s_cPrune ? "%1[%2]" : "%1%2" )
        //    .arg(boundPrefix)
        //    .arg(valueBound);
	if( curLine->s_cPrune )
        {
            break;
        }
    }
    //sumString.chop(3); // Remove trailing " + ".
    //sumString += QString("=%1").arg(sum);
    //curLine = new SExpectMLine;
    //curLine->s_valueValue = sum;
    //curLine->s_conf = m_conf;
    //curLine->s_valueCall = sumString;
    //curLine->s_chance = true;
    //curLine->s_open = " "; // Prevent this line from being deleted later.
    //curLine->s_bestAction = sumString;
    //call->s_lines.append( curLine );  
    //curLine->s_bestAction = QString("[%1]").arg(SExpectMLine::numberToString(sum));
    if( !curLine->s_cPrune )
        call->s_returnValue = sum;
    curLine->s_bestAction = QString("[%1]").arg(sum);
    //else call->s_returnValue = valueBound;
    depthString = "";
    if( m_conf.s_isDL )
    {
        depthString = QString(",%1").arg(depth);
    }
    if( m_conf.s_isQS )
    {
        depthString += QString(",%1").arg(qsDepth);
    }
    ((SExpectMLine*)call->s_lines.first())->s_call =
        QString(max?"MinChance(%1%2,%3,%4)":"MaxChance(%1%2,%3,%4)")
        .arg(node->s_name)
        .arg(depthString)
        .arg(SExpectMLine::numberToString(alpha))
        .arg(SExpectMLine::numberToString(beta));
    ((SExpectMLine*)call->s_lines.last())->s_boxValue = true;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer::ABtraceCall
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
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
bool CExpectMTracer::ABtraceCall( SChanceTreeCall *call, const SChanceTreeNode *node,
                     int depth, int qsDepth, float alpha, float beta, bool max ) const
{
    if( call == NULL || node == NULL || node->s_chance)
        return false;

    QList<SChanceTreeNode*> curChildren = node->s_childList;
    sortNodes( curChildren );

    QString depthString;
    QString open;
    SChanceTreeNode *curNode;
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

    SExpectMLine *curLine;
    SChanceTreeNode *bestAction = NULL;
    float bestValue = -1;
    float curAlpha = alpha, curBeta = beta;

    bool better;
    bool isEndPoint;
    foreach( curNode, curChildren )
    {
        bool sssPrune = false;

        curLine = new SExpectMLine;

        curLine->s_conf = m_conf;

        curLine->s_open = open;

        if( m_conf.s_isHT )
            curLine->s_open.remove(' ');
        curLine->s_eval = curNode->s_name;
        open = open.section( ' ', 1, -1 );

        depthString = "";
        isEndPoint = false;
        if( curNode->s_childList.count() == 0 )
            isEndPoint = true;

        if( m_conf.s_isDL )
        {
            depthString = QString(",%1").arg(depth);
        }
        if( m_conf.s_isQS )
        {
            depthString += QString(",%1").arg(qsDepth);
        }
        curLine->s_valueCall = max?"MaxChance":"MinChance";
        curLine->s_valueCall += QString("(%1%2,%3,%4)")
                                .arg(curNode->s_name)
                                .arg(depthString)
                                .arg(SExpectMLine::numberToString(curAlpha))
                                .arg(SExpectMLine::numberToString(curBeta));
        if( isEndPoint )
        {
            curLine->s_valueValue = curNode->s_value;
            curLine->s_sss = false;
        }
        else if( checkSSS( curNode, depth, qsDepth ) )
        {
            curLine->s_valueValue = sssValue( curNode, depth, qsDepth );

            curLine->s_sss = true;
            if(m_conf.s_isHT)
                curLine->s_historyUpdates = sssHistoryUpdates( curNode,
                                                    depth, qsDepth );

        }
        else
        {
            call->s_children.append( new SChanceTreeCall() );
            bool success = ABtraceECall( call->s_children.last(), curNode,
                                         depth, qsDepth,
                                         curAlpha, curBeta, !max );
            if(!success)
            {
                delete curLine;
                return false;
            }
            curLine->s_valueValue = call->s_children.last()->s_returnValue;
        }
        // Chance nodes are not quiescent.
        // curLine->s_qs = (curDepth == 0 && curQSDepth > 0
        //                  && curNode->s_quiescent);
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
        ((SExpectMLine*)call->s_lines.last())->s_historyUpdates
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
    ((SExpectMLine*)call->s_lines.first())->s_call =
        QString(max?"Max(%1%2,%3,%4)":"Min(%1%2,%3,%4)")
        .arg(node->s_name)
        .arg(depthString)
        .arg(SExpectMLine::numberToString(alpha))
        .arg(SExpectMLine::numberToString(beta));
    ((SExpectMLine*)call->s_lines.last())->s_boxValue = true;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// SExpectMLine::SExpectMLine
///
/// @description    This is the constructor of the SExpectMLine class.
/// @pre            None
/// @post           Object is created.
///
/// @param          None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
SExpectMLine::SExpectMLine()
{
    s_call = QString();
    s_open = QString();
    s_eval = QString();
    s_bounds = QString();

    s_valueCall = QString();
    s_valueValue = 0.0;
    s_qs = false;
    s_sss = false;
    s_prune = false;
    s_cPrune = false;
    s_chance = false;

    s_alpha = -INF;
    s_beta = INF;

    s_bestAction = QString();
    s_boxAction = false;
    s_bestValue = 0.0;
    s_boxValue = false;
}

////////////////////////////////////////////////////////////////////////////////
/// SExpectMLine::~SExpectMLine
///
/// @description    This is the destructor of the SExpectMLine class.
/// @pre            None
/// @post           Object is destroyed.
///
/// @param          None.
///
/// @limitations    None.
///
////////////////////////////////////////////////////////////////////////////////
SExpectMLine::~SExpectMLine()
{
}

////////////////////////////////////////////////////////////////////////////////
/// SExpectMLine::numberToString
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
QString SExpectMLine::numberToString( float val )
{
    if( val == INF )
        return QString("INF");
    if( val == -INF )
        return QString("-INF");
    return QString::number(val);
}

////////////////////////////////////////////////////////////////////////////////
/// SExpectMLine::operator []
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
QString SExpectMLine::operator [](int index)
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
        if(ret != "")
            ret += "=" + QString::number( s_valueValue );
        if( s_qs || s_sss || s_prune || s_cPrune )
        {
            ret += " (";
            if( s_qs )
            {
                ret += "QS";
                if( s_sss || s_prune | s_cPrune )
                    ret += ",";
            }
            if( s_sss )
            {
                ret += "SSS";
                if( s_prune || s_cPrune )
                    ret += ",";
            }
            if( s_prune )
	    {
                ret += "Prune";
                if( s_cPrune )
                    ret += ",";
            }
            if( s_cPrune )
                ret += "CP";
            ret += ")";
        }
        break;
    case 4:
        ret = s_bounds;
        break;
    case 5: //  a,b
        ret += numberToString(s_alpha);
        ret += ", ";
        ret += numberToString(s_beta);
        break;
    case 6: //  BEST ACTION,VALUE
        if( s_chance )
            return s_bestAction;
        else if( s_boxAction )
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
/// SExpectMLine::toStringList
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
QStringList SExpectMLine::toStringList()
{
    QStringList ret;

    for( int i = 0; i < 7; ++i )
    {
        if(i == 2) continue;
        if(i == 4 && !s_conf.s_isCP) continue;
        if(i == 5 && !s_conf.s_isAB) continue;

        ret += (*this)[i];
    }
    return ret;
}
