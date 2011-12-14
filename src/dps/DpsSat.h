#ifndef _SAT_H_
#define _SAT_H_

#include <dps/dps.h>

#include "Sat.h"

using namespace dps;

class VarData : public AutoSerial
{
	CLASSDEF(VarData)
	MEMBERS
		ITEM(bool, m_isInverted)
		ITEM(int,  m_varId)
	CLASSEND;
};

class SatSolution : public AutoSerial
{
	CLASSDEF(SatSolution)
	MEMBERS
		ITEM(int, m_nVars)
		ITEM(bool, m_isSatisfiable)
		ITEM(Buffer<VarData>, m_vars)
	CLASSEND;
};

class SatData : public AutoSerial
{
	CLASSDEF(SatData)
	MEMBERS
		ITEM(Buffer<VarData>, m_conjunctions)
		ITEM(Buffer<VarData>, m_predefinedVars)
		ITEM(int, m_nPredefinedVars)
		ITEM(int, m_nVars)
		ITEM(int, m_nConjunctions)
	CLASSEND;
};

class Split : public SplitOperation<SatData, SatData>
{
	IDENTIFY(Split);

	void execute(SatData *in);
};

class Merge : public MergeOperation<SatSolution, SatSolution>
{
	IDENTIFY(Merge);

	void execute(SatData *in);
};

template <typename H>
class Solve<H> : public LeafOperation<SatData, SatSolution>
{
	IDENTIFY(Solve);

	void execute(SatData *in);
};

class SatApp : public Application
{
	IDENTIFY(SatApp);
};

#endif // _SAT_H_
