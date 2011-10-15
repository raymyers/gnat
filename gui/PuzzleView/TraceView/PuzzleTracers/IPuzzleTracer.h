////////////////////////////////////////////////////////////////////////////////
/// @file         IPuzzleTracer.h
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
#ifndef _ISINGLEAGENTTRACER_H_
#define _ISINGLEAGENTTRACER_H_

#include "../../CPuzzleTrace.h"
#include "../../GraphView/CPuzzleGraphModel.h"
#include <QString>

////////////////////////////////////////////////////////////////////////////////
/// ISingleAgentTracer
///
/// @description  This is an interface class. It is the base class of all trace
///               algorithms. When subclassing it, make sure and override the
///               name function to return the name of the new trace. An example
///               of a subclass is the CUDTracer.
///
/// @limitations  None
///
////////////////////////////////////////////////////////////////////////////////
class ISingleAgentTracer
{
public:
    ISingleAgentTracer();
    virtual ~ISingleAgentTracer();

    //virtual QString name() const;
    virtual SPuzzleTrace *trace( const CPuzzleGraphModel *graphModel ) const { return NULL;};
    virtual SPuzzleTrace *trace( const int depthLimit,
                             const bool iterativeDeepening,
                             const bool graphSearch,
                             const CPuzzleGraphModel *graph ) const { return NULL;}
};

#endif
