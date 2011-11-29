
#ifndef _SAT_H_
#define _SAT_H_

#include <dps/dps.h>

using namespace dps;

class ConjunctionData : public AutoSerial
{
	CLASSDEF(VarData)
		ITEM(bool, m_isNegated)
		ITEM(int,  m_varId)
	CLASSEND;
};

class SatData : public AutoSerial 
{
	CLASSDEF(Sat)
		ITEM(Buffer<VarData>, m_conjunctions)
		ITEM(int, m_nVars)
	CLASSEND;
};

#endif // _SAT_H_

