////////////////////////////////////////////////////////////////////////////////
/// @file         CUDTracer.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CUDTracer
///               and SUDLine classes. The "UD" stands for "Uni-Directional".
///               CUDTracer is the class that executes all Uni-Directional
///               single agent traces.
///
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _CUDTRACER_H_
#define _CUDTRACER_H_

#include "IPuzzleTracer.h"
#include "CSATraceNode.h"

////////////////////////////////////////////////////////////////////////////////
/// CBUTracer
///
/// @description  This class is executes Uni-Directional Single Agent traces.
///
/// @limitations
///
////////////////////////////////////////////////////////////////////////////////
template <class OpenListType>
class CUDTracer : public ISingleAgentTracer
{
public:
    CUDTracer() {}
    ~CUDTracer() {}

    SPuzzleTrace *trace( const int depthLimit,
                         const bool iterativeDeepening,
                         const bool graphSearch,
                         const CPuzzleGraphModel *graph ) const;
    SPuzzleTrace *trace( const CPuzzleGraphModel *graph ) const {return NULL;};
private:
    QString goalString(const CPuzzleGraphModel *graph,
                       const QList<IPuzzleLine *> lines, CSATraceNode goal, int iOpen, int iEval) const;
};


struct SUDLine : public IPuzzleLine
{
    SUDLine();
    ~SUDLine();

    QString operator[](int index);
    QStringList toStringList();


    QStringList s_strings;
    //QString s_closed;
    //QString s_eval;

    //QString s_comment;
};

#endif

