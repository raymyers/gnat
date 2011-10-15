////////////////////////////////////////////////////////////////////////////////
/// @file         CBDTracer.h
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the declaration of the CBDTracer
///               and SBDLine classes. The "BD" stands for "Bi-Directional".
///               CBDTracer is the class that executes all Bi-Directional
///               single agent traces.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _CBDTRACER_H_
#define _CBDTRACER_H_

#include "IPuzzleTracer.h"
#include "CSATraceNode.h"

////////////////////////////////////////////////////////////////////////////////
/// CBDTracer
///
/// @description  This class is executes Bi-Directional Single Agent traces.
///
/// @limitations
///
////////////////////////////////////////////////////////////////////////////////
template <class OpenListType>
class CBDTracer : public ISingleAgentTracer
{
public:
    CBDTracer() {}
    ~CBDTracer() {}

    SPuzzleTrace *trace( const int depthLimit,
                         const bool iterativeDeepening,
                         const bool graphSearch,
                         const CPuzzleGraphModel *graph ) const;
    SPuzzleTrace *trace( const CPuzzleGraphModel *graph ) const { return NULL; };
private:
    QString goalString(const CPuzzleGraphModel *graph,
                       const QList<IPuzzleLine *> lines, int indexF, int indexB, int cost,
                       int iOpenForward,  int iEvalForward,
                       int iOpenBackward, int iEvalBackward) const;

};


struct SBDLine : public IPuzzleLine
{
    SBDLine();
    ~SBDLine();

    QString operator[](int index);
    QStringList toStringList();


    QStringList s_strings;
    //QString s_closed;
    //QString s_eval;

    //QString s_comment;
};

#endif

