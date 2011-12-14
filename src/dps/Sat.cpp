#include <cstring>
#include <iostream>
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

Backtrack::Backtrack():
	m_varId(0),
	m_clauses(vector<Conjunction const*>()),
	m_pureVars(vector<int>()),
	m_uniteVars(vector<int>()),
	m_backtracked(false) { }

bool SatProblem::pick_var(Conjunct &c) const
{
	for(ConjunctionIter it = begin(); it != end(); ++it) {
		for(int i=0; i < Conjunction::Size; ++i) {
			if(m_values[(**it)[i].id()] == Unset) {
				c = (**it)[i];
				return true;
			}
		}
	}

	return false;
}

#define OnlyTrue  0
#define OnlyFalse 1
#define Unknown   2
#define Differs   3

void SatProblem::detect_pure_literals()
{
	char values[m_numVars+1];
	memset(values, Unknown, m_numVars+1);

	for(vector<Conjunction const*>::iterator it = m_conjunctions.begin();
			it != m_conjunctions.end(); ++it) {
		for(int i=0; i<Conjunction::Size; ++i) {
			VarId id = ((**it)[i]).id();
			bool inverted = ((**it)[i]).inverted();
			if(m_values[id] == Unset) {
				if(values[id] == Unknown) {
					values[id] = inverted? OnlyFalse : OnlyTrue;
				} else if(values[id] == OnlyTrue) {
					values[id] = (inverted)? Differs : OnlyTrue;
				} else if(values[id] == OnlyFalse) {
					values[id] = (inverted)? OnlyFalse : Differs;
				}
			}
		}
	}

	for(int i=1; i<=m_numVars; ++i) {
		if(values[i] == OnlyTrue || values[i] == OnlyFalse) {
			m_backtracks[m_index].m_pureVars.push_back(i);
			m_values[i] = (values[i] == OnlyTrue)? True : False;
		}
	}
}

void SatProblem::detect_unite_clauses()
{
	for(ConjunctionIter it = m_conjunctions.begin();
			it != m_conjunctions.end(); ++it) {
		int nUndefined = 0;
		VarId undefinedId;
		bool undefinedInverted;
		bool sat = false, inverted = false;
		for(int i=0; i<Conjunction::Size; ++i) {
			VarId id = ((**it)[i]).id();
			inverted = ((**it)[i]).inverted();
			if(m_values[id] == Unset) {
				nUndefined++;
				undefinedId = id;
				undefinedInverted = inverted;
			} else if((m_values[id] == False && inverted) ||
						(m_values[id] == True && !inverted)) {
				sat = true;
				break;
			}
		}

		if(!sat && nUndefined == 1) {
			m_backtracks[m_index].m_uniteVars.push_back(undefinedId);
			m_values[undefinedId] = undefinedInverted ? False : True;
		}
	}
}

// Returns false in case one of the conjunct is evaluated to false,
// true otherwise.
bool SatProblem::simplify()
{
	vector<Conjunction const*>::iterator it = m_conjunctions.begin();
	Backtrack *b = &m_backtracks[m_index];

	while(it != m_conjunctions.end()) {
		bool sat = false, undefined = false;

		for(int i=0; i<Conjunction::Size; ++i) {
			VarId id = ((**it)[i]).id();
			bool inverted = ((**it)[i]).inverted();

			if(m_values[id] == Unset)
				undefined = true;
			else if((m_values[id] == True && !inverted)
					|| (m_values[id] == False && inverted)) {
				b->m_clauses.push_back(*it);
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

typedef vector<VarId>::iterator VarIdIter;

bool SatProblem::backtrack()
{
	while(m_index >= 0) {
		Backtrack* back = &(m_backtracks[m_index]);
		m_conjunctions.insert(m_conjunctions.begin(), back->begin(), back->end());
		(back->m_clauses).clear();

		if(back->m_backtracked || back->m_varId == 0) {
			for(VarIdIter it = back->m_uniteVars.begin();
					it != back->m_uniteVars.end(); ++it)
				m_values[*it] = Unset;
			back->m_uniteVars.clear();

			for(VarIdIter it = back->m_pureVars.begin();
					it != back->m_pureVars.end(); ++it)
				m_values[*it] = Unset;
			back->m_pureVars.clear();
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
		for(int i=1; i<=m_numVars; ++i) {
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
	bool backtracked = false;
	while(!m_conjunctions.empty()) {
		Conjunct c;
		if(!backtracked) {
			detect_unite_clauses();
			detect_pure_literals();
		}
		backtracked = false;
		if(pick_var(c)) {
			m_backtracks[m_index].m_varId = c.id();
			m_backtracks[m_index].m_backtracked = false;
			m_values[c.id()] = c.inverted()? False : True;
		} else m_backtracks[m_index].m_varId = 0;

		while(!simplify()) {
			backtracked = true;
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

SplitHeuristic::SplitHeuristic(SatProblem *s, int nVars):
	m_chosenVars(vector<Conjunct>()),
	m_problem(s),
	m_nVars(nVars),
	m_moreCombination(true) { }

void SplitHeuristic::init()
{
	for(vector<Conjunct>::iterator it=m_chosenVars.begin();
			it != m_chosenVars.end(); ++it) {
		*it = Conjunct(it->id(), false);
		m_problem->m_values[it->id()] = True;
	}
}

bool SplitHeuristic::nextCombination()
{
	vector<Conjunct> saved;

	Conjunct c = m_chosenVars.back();
	m_chosenVars.pop_back();
	for(; c.inverted(); c = m_chosenVars.back(), m_chosenVars.pop_back()) {
		saved.push_back(c);
		if(m_chosenVars.empty()) return false;
	}

	m_chosenVars.push_back(Conjunct(c.id(), true));
	for(vector<Conjunct>::reverse_iterator it = saved.rbegin();
			it != saved.rend(); ++it) {
		m_chosenVars.push_back(Conjunct(it->id(), false));
	}
	return true;
}

bool SplitHeuristic::next(vector<Conjunct>& values)
{
	if(!m_moreCombination) return false;
	bool ok = false;

	while(!ok) {
		m_problem->m_conjunctions.insert((m_problem->m_conjunctions).begin(),
				(m_problem->m_backtracks)[0].begin(),
				(m_problem->m_backtracks)[0].end());
		(m_problem->m_backtracks)[0].m_clauses.clear();
		ok = m_problem->simplify();

		m_moreCombination = nextCombination();
		if(!ok && !m_moreCombination) return false;

		for(ConjunctIter it=m_chosenVars.begin(); it != m_chosenVars.end(); ++it) {
			m_problem->m_values[it->id()] = (it->inverted()? False : True);
		}
	}
	
	values.insert(values.begin(), m_chosenVars.begin(), m_chosenVars.end());
	return true;
}

void FirstNHeuristic::chooseVars()
{
	for(int i=1; i<=m_nVars; ++i) {
		m_chosenVars.push_back(Conjunct(i, false));
	}

	init();
}

