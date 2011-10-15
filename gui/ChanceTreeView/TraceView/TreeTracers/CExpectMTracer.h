////////////////////////////////////////////////////////////////////////////////
/// @file         CExpectMTracer.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CExpectMTracer
///               class. This is the class that executes ExpectiMiniMax traces.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _CEXPECTMTRACER_H_
#define _CEXPECTMTRACER_H_

#include "../../SChanceTreeTrace.h"
#include "../../CChanceTreeGraph.h"
#include <QString>
#include <QMap>

typedef QMap<const SChanceTreeNode*, int> HistTable;
// The name HistoryTable can't be used because there is another typedef
// of that name with conflicting type in CChanceTreeGraphModel.h

struct SChanceTreeTrace;
struct SChanceTreeCall;

struct STraceConfig
{
    // Quiessence Search?
    bool s_isQS;
    // History Table?
    bool s_isHT;
    // Depth Limited?
    bool s_isDL;
    // Iterative Deepening?
    bool s_isID;
    // Alpha Beta Pruning?
    bool s_isAB;
    // Chance Pruning
    bool s_isCP;
    // Allow Single Successor Shortcut?
    bool s_allowSSS;

};

////////////////////////////////////////////////////////////////////////////////
/// CExpectMTracer
///
/// @description  This class is capable of executing many combinations of
///               MiniMax variations.
///
/// @limitations
///
////////////////////////////////////////////////////////////////////////////////
class CExpectMTracer
{
public:
    CExpectMTracer();

    SChanceTreeTrace *trace( const SChanceTreeNode *rootNode, int depthLimit, int qsDepth,
                       const STraceConfig &traceConf, qreal lowerBound, qreal upperBound ) const;
private:
    bool traceCall( SChanceTreeCall *call, const SChanceTreeNode *node,
                    int depth, int qsDepth, bool max ) const;
    bool traceECall( SChanceTreeCall *call, const SChanceTreeNode *node,
                    int depth, int qsDepth, bool max ) const;
    bool ABtraceCall( SChanceTreeCall *call, const SChanceTreeNode *node,
                    int depth, int qsDepth, float alpha, float beta, bool max ) const;
    bool ABtraceECall( SChanceTreeCall *call, const SChanceTreeNode *node,
                    int depth, int qsDepth, float alpha, float beta, bool max ) const;

    bool checkSSS( const SChanceTreeNode *node, int depth, int qsDepth ) const;

    float sssValue( const SChanceTreeNode *node, int depth, int qsDepth ) const;
    QStringList sssHistoryUpdates( const SChanceTreeNode *node,
                                   int depth, int qsDepth,
                                   bool top = true ) const;
    void generateHistoryTable( const SChanceTreeNode *node ) const;
    void sortNodes( QList<SChanceTreeNode *> &nodes ) const;

    QString moveName( const SChanceTreeNode *node ) const;

    mutable HistTable m_historyTable;
    mutable STraceConfig m_conf;
    mutable qreal m_lowerBound;
    mutable qreal m_upperBound;
};

////////////////////////////////////////////////////////////////////////////////
/// SExpectMLine
///
/// @description  The SExpectMLine represents an actual line in a trace grid. The
///               different cells are returned as QStrings through the []
///               operator or they can be extracted as a list by the
///               toStringList function.
///
/// @limitations
///
////////////////////////////////////////////////////////////////////////////////
struct SExpectMLine
{
    SExpectMLine();
    ~SExpectMLine();

    QString operator[](int index);
    QStringList toStringList();

    static QString numberToString( float val );

    QString s_call;

    QString s_open;

    QString s_eval;

    QString s_valueCall;
    
    QString s_bounds;

    float   s_valueValue;
    bool    s_sss;
    bool    s_qs;
    bool    s_prune;
    bool    s_cPrune;
    bool    s_chance;
    float   s_alpha;
    float   s_beta;

    QString s_bestAction;
    bool    s_boxAction;
    float   s_bestValue;
    bool    s_boxValue;

    STraceConfig s_conf;
    QStringList  s_historyUpdates;
};


#endif//_CEXPECTMTRACER_H_
