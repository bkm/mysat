
#ifndef _SAT_H_
#define _SAT_H_

#include <dps/dps.h>

#include "Sat.h"

using namespace dps;

class SatSolution : public AutoSerial
{
	CLASSDEF(SatSolution)
	MEMBERS
		ITEM(bool, m_isSatisfiable)
		ITEM(Buffer<bool>, m_varAssignment)
	CLASSEND;
};

class ConjunctionData : public AutoSerial
{
	CLASSDEF(VarData)
	MEMBERS
		ITEM(bool, m_isNegated)
		ITEM(int,  m_varId)
	CLASSEND;
};

class SatData : public AutoSerial
{
	CLASSDEF(SatData)
	MEMBERS
		ITEM(Buffer<VarData>, m_conjunctions)
		ITEM(int, m_nVars)
	CLASSEND;
};

class Split : public SplitOperation<SatData, SatData>
{
	void execute(SatData *in);
};

class Merge : public MergeOperation<SatData, SatSolution>
{
	void execute(SatData *in);
};

class Solve : public LeafOperation<SatData, SatSolution>
{
	void execute(SatData *in);
};

class SatApp : public Application
{
	IDENTIFY(SatApp);
};

#endif // _SAT_H_
