
#include <stdexcept>

#include <iostream>

#include "Sat.h"
#include "Dimacs.h"

using namespace std;

void DimacsFile::read()
{
	
	// skip comments
	do {
		string line;
		if(m_inputStream.peek() != 'c')
			break;
		getline(m_inputStream, line);
	} while(m_inputStream.good());

	if(!m_inputStream.good())
		throw runtime_error("Wrong file format");

	char p, c, n, f;
	m_inputStream >> skipws >> p >> c >> n >> f;
	if(!m_inputStream.good() || p != 'p' || c != 'c' || n != 'n' || f != 'f')
		throw runtime_error("Wrong file format");

	int numVars, numClauses, i;
	m_inputStream >> numVars >> numClauses;
	m_satProblem = new SatProblem(numVars, numClauses);

	try {
		for(i=0; i < numClauses; ++i) {
			m_satProblem->addConjunction(readConjunction());
		}

		if(i != numClauses)
			throw runtime_error("Wrong number of conjunctions");
	} catch(runtime_error e) {
		delete m_satProblem;
		m_satProblem = NULL;
		throw e;
	}
}

Conjunction const* DimacsFile::readConjunction()
{
	int i;
	Conjunction *c = new Conjunction();

	for(i=0;; ++i) {
		int val;
		m_inputStream >> val;
		if(i > Conjunction::Size)
			throw runtime_error("Too many conjuncts");
		if(val == 0) {
			if(i < Conjunction::Size)
				throw runtime_error("Too few conjuncts");
			return c;
		}

		if(!m_inputStream.good()) {
			delete c;
			throw runtime_error("Error while reading file");
		}

		(*c)[i] = val;
	}

	return c;
}
