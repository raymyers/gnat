///////////////////////////////////////////////////////////////////////////////
/// @file         IPuzzleTracer.cpp
///
/// @author       Ray Myers
///
/// @compiler     Windows: MinGW / Linux: GNU G++
///
/// @project      GNAT
///
/// @description  This file contains the implementation of the ISingleAgentTracer
///               interface class.
///
////////////////////////////////////////////////////////////////////////////////
#include "IPuzzleTracer.h"

////////////////////////////////////////////////////////////////////////////////
/// ISingleAgentTracer::ISingleAgentTracer
///
/// @description    This is the constructor of the ISingleAgentTracer class.
/// @pre            None
/// @post           None
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
ISingleAgentTracer::ISingleAgentTracer()
{
}

////////////////////////////////////////////////////////////////////////////////
/// ISingleAgentTracer::ISingleAgentTracer
///
/// @description    This is the destructor of the ISingleAgentTracer class. It does
///                 nothing, but it is virtual to make sure subclass's
///                 destructors are called.
/// @pre            None
/// @post           None
///
/// @limitations    None
///
////////////////////////////////////////////////////////////////////////////////
ISingleAgentTracer::~ISingleAgentTracer()
{
}
