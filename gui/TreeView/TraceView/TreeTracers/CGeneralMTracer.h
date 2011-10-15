////////////////////////////////////////////////////////////////////////////////
/// @file         CGeneralMTracer.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CGeneralMTracer
///               class. This is the class that executes all adversarial traces.
///               The "M" stands for "MiniMax".
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _CGENERALMTRACER_H_
#define _CGENERALMTRACER_H_

#include "../../STreeTrace.h"
#include "../../CTreeGraph.h"
#include <QString>
#include <QMap>

typedef QMap<const STreeNode*, int> HistTable;
// The name HistoryTable can't be used because there is another typedef
// of that name with conflicting type in CTreeGraphModel.h

struct STreeTrace;
struct STreeCall;

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
    // Allow Single Successor Shortcut?
    bool s_allowSSS;

};

////////////////////////////////////////////////////////////////////////////////
/// CGeneralMTracer
///
/// @description  This class is capable of executing many combinations of
///               MiniMax variations.
///
/// @limitations
///
////////////////////////////////////////////////////////////////////////////////
class CGeneralMTracer
{
public:
    CGeneralMTracer();

    STreeTrace *trace( const STreeNode *rootNode, int depthLimit, int qsDepth,
                       const STraceConfig &traceConf ) const;
private:
    void traceCall( STreeCall *call, const STreeNode *node,
                    int depth, int qsDepth, bool max ) const;
    void ABtraceCall( STreeCall *call, const STreeNode *node,
                    int depth, int qsDepth, float alpha, float beta, bool max ) const;

    bool checkSSS( const STreeNode *node, int depth, int qsDepth ) const;

    float sssValue( const STreeNode *node, int depth, int qsDepth ) const;
    QStringList sssHistoryUpdates( const STreeNode *node,
                                   int depth, int qsDepth,
                                   bool top = true ) const;
    void generateHistoryTable( const STreeNode *node ) const;
    void sortNodes( QList<STreeNode *> &nodes ) const;

    QString moveName( const STreeNode *node ) const;

    mutable HistTable m_historyTable;
    mutable STraceConfig m_conf;
};

////////////////////////////////////////////////////////////////////////////////
/// SGeneralMLine
///
/// @description  The SGeneralMLine represents an actual line in a trace grid. The
///               different cells are returned as QStrings through the []
///               operator or they can be extracted as a list by the
///               toStringList function.
///
/// @limitations
///
////////////////////////////////////////////////////////////////////////////////
struct SGeneralMLine
{
    SGeneralMLine();
    ~SGeneralMLine();

    QString operator[](int index);
    QStringList toStringList();

    static QString numberToString( float val );

    QString s_call;

    QString s_open;

    QString s_eval;

    QString s_valueCall;
    float   s_valueValue;
    bool    s_sss;
    bool    s_qs;
    bool    s_prune;

    float   s_alpha;
    float   s_beta;
    QString s_bestAction;
    bool    s_boxAction;
    float   s_bestValue;
    bool    s_boxValue;

    STraceConfig s_conf;
    QStringList  s_historyUpdates;
};


#endif//_CGENERALMTRACER_H_
