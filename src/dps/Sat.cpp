#include <cassert>
#include <sstream>

#include "Sat.h"

using namespace std;

ostream& operator<<(ostream& out, const Conjunct& c)
{
	stringstream ss;
	string sign(c.inverted()? "-" : "");
	ss << c.id();

	out << sign << ss.str();
	return out;
}

ostream& operator<<(ostream& out, const Conjunction& c)
{
	for(int i = 0; i < Conjunction::Size; ++i) {
		out << c[i];
		out << " ";
	}
	return out;
}

ostream& operator<<(ostream& out, const SatProblem& p)
{
	for(ConjunctionIter i = p.begin(); i != p.end(); ++i) {
		out << **i << endl;
	}
	return out;
}

const Conjunct& SatProblem::pick_var() const
{
	Conjunction const* c = m_conjunctions.at(0);
	for(int i=0; i < Conjunction::Size; ++i) {
		if(m_values[(*c)[i].id()] == Unset)
			return (*c)[i];
	}

	assert(false);
}

// Not the most optimized version
bool SatProblem::simplify()
{
	vector<Conjunction const*>::iterator it = m_conjunctions.begin();
	while(it != m_conjunctions.end()) {
		bool undefined = false, sat = false;
		for(int i=0; i<Conjunction::Size; ++i) {
			VarId id = ((**it)[i]).id();
			bool inverted = ((**it)[i]).inverted();
			if(m_values[id] == Unset)
				undefined = true;

			if((m_values[id] == True && !inverted)
					|| (m_values[id] == False && inverted)) {
				m_backtracks[m_index].m_clauses.push_back(*it);
				it = m_conjunctions.erase(it);
				sat = true;
				break;
			}
		}
		if(!undefined && !sat) return false;
		if(undefined && !sat) ++it;
	}
	return true;
}

bool SatProblem::backtrack()
{
	while(m_index >= 0) {
		Backtrack* back = &(m_backtracks[m_index]);
		m_conjunctions.insert(m_conjunctions.begin(), back->begin(), back->end());
		(back->m_clauses).clear();
		if(back->m_backtracked) {
			m_values[back->m_varId] = Unset;
			--m_index;
			continue;
		}

		back->m_backtracked = true;
		m_values[back->m_varId] = (m_values[back->m_varId] == True)? False : True;
		return true;
	}

	// couldn't backtrack, this problem is not satisfiable
	return false;
}

SatProblem::SatProblem(int numVars, int numClauses):
	m_numVars(numVars),
	m_numClauses(numClauses),
	m_index(0),
	m_values(new ConjunctVal[numVars+1]),
	m_backtracks(new Backtrack[numVars]),
	m_conjunctions(vector<Conjunction const*>()) {
		for(int i=1; i<=numVars; ++i) {
			m_values[i] = Unset;
		}
	}

SatProblem::~SatProblem()
{
	delete m_values;
	delete m_backtracks;
	for(vector<Conjunction const*>::iterator it = m_conjunctions.begin();
			it != m_conjunctions.end(); ++it) {
		delete *it;
	}
}

bool SatProblem::solve(vector<Conjunct>& solution)
{
	while(!m_conjunctions.empty()) {
		const Conjunct c = pick_var();
		m_backtracks[m_index].m_varId = c.id();
		m_backtracks[m_index].m_backtracked = false;
		m_values[c.id()] = c.inverted()? False : True;
		while(!simplify()) {
			if(!backtrack()) return false;
		}

		++m_index;
	}

	for(int i=1; i<=m_numVars; ++i) {
		if(m_values[i] == Unset) {
			solution.push_back(i);
		} else {
			solution.push_back(Conjunct(i, m_values[i] != True));
		}
	}

	return true;
}

