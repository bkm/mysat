
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

#include <stdio.h>

#include "Sat.h"

using namespace std;

namespace Sat {

ostream& operator<<(ostream& out, const Clause& c)
{
	for(VarIter i = c.begin(); i != c.end(); ++i) {
		out << *i << "  ";
	}
	return out;
}

ostream& operator<<(ostream& out, const Var& v)
{
	stringstream ss;
	string sign(v.m_inverted? "-" : "");
	ss << v.m_id;
	out << sign << ss.str();
	return out;
}

ostream& operator<<(ostream& out, const Sat& s)
{
	for(ClausePtrIter it = s.begin(); it != s.end(); ++it) {
		Clause const *c = *it;
		out << *c << endl;
	}
	return out;
}

bool Sat::solve(vector<Var>& solution)
{
	while(!m_clauses.empty()) {
		Var v = pick_var();
		m_backtracks[m_index].m_varId = v.id();
		m_backtracks[m_index].m_backtracked = false;
		m_vars[v.id()] = v.inverted()? False : True;
		while(!simplify()) {
			if(!backtrack()) return false;
		}
		++m_index;
	}

	for(int i = 1; i <= m_nVars; ++i) {
		if(m_vars[i] == Unset) {
			solution.push_back(Var(i, true));
		} else {
			solution.push_back(Var(i, m_vars[i] != True));
		}
	}
	return true;
}

bool Sat::backtrack()
{
	while(m_index >= 0) {
		Backtrack* back = &(m_backtracks[m_index]);
		m_clauses.insert(m_clauses.begin(), back->begin(), back->end());
		(back->m_clauses).clear();
		if(back->m_backtracked) {
			m_vars[back->m_varId] = Unset;
			--m_index;
			continue;
		}

		back->m_backtracked = true;
		m_vars[back->m_varId] = (m_vars[back->m_varId] == True)? False : True;
		return true;
	}

	// couldn't backtrack, this problem is not satisfiable
	return false;
}

// returns false if an unsatisfied clause is detected
// returns true otherwise
bool Sat::simplify()
{
	vector<Clause const*>::iterator it = m_clauses.begin();
	while(it != m_clauses.end()) {
		bool undefined = false, sat = false;
		for(VarIter vIt = (*it)->begin(); vIt != (*it)->end(); ++vIt) {
			if(m_vars[vIt->id()] == Unset)
				undefined = true;

			if((m_vars[vIt->id()] == True && !vIt->inverted())
					|| (m_vars[vIt->id()] == False && vIt->inverted())) {
				m_backtracks[m_index].m_clauses.push_back(*it);
				it = m_clauses.erase(it);
				sat = true;
				break;
			}
		}
		if(!undefined && !sat) return false;
		if(undefined && !sat) ++it;
	}
	return true;
}

const Var& Sat::pick_var() const
{
	Clause const* clause = m_clauses.at(0);
	for(VarIter it = clause->begin(); it != clause->end(); ++it) {
		if(m_vars[it->id()] == Unset)
			return *it;
	}

	// shouldn't arrive here
	assert(false);
}

// Note : assumes correctly formated file
void Sat::read(const char* file)
{
	ifstream in(file, ifstream::in);
	while(in.peek() == 'c')
		in.ignore(256, '\n');

	in.ignore(5);
	in >> m_nVars;
	in >> m_nClauses;
	m_backtracks = new Backtrack[m_nVars];
	m_vars = new VarValue[m_nVars+1];
	for(int i=0; i < m_nVars+1; ++i) {
		m_vars[i] = Unset;
	}

	int var;
	for(int i=0; i < m_nClauses; ++i) {
		Clause *newClause = new Clause;
		m_clauses.push_back(newClause);
		while(true) {
			in >> var;
			if(var == 0) break;
			newClause->addVar((var < 0)? -var : var, (var < 0));
		}
	}
}
}
