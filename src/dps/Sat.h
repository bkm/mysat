
#ifndef _SAT_H_
#define _SAT_H_

#include <vector>
#include <ostream>

typedef int VarId;

using std::vector;
using std::ostream;

class Conjunct {
	public :
		Conjunct(): m_varId(0) { }
		Conjunct(VarId id, bool isInverted):
			m_varId(id),
			m_isInverted(isInverted) { }
		Conjunct(int val):
			m_varId(val < 0? -val : val),
			m_isInverted(val < 0) { }

		bool inverted() const { return m_isInverted; }
		VarId id() const { return m_varId; }

	private :
		VarId m_varId;
		bool m_isInverted;
};

typedef vector<Conjunct>::const_iterator ConjunctIter;

class Conjunction {
	public :
		enum Size {
			Size = 3
		};

		Conjunction() { }

		Conjunct& operator[](int idx) { return m_conjuncts[idx]; }
		const Conjunct& operator[](int idx) const { return m_conjuncts[idx]; }

		Conjunct& at(int idx) { return m_conjuncts[idx]; }
		const Conjunct& at(int idx) const { return m_conjuncts[idx]; }

	private :
		Conjunct m_conjuncts[Size];
};

typedef vector<Conjunction const*>::const_iterator ConjunctionIter;

struct Backtrack {
	Backtrack();

	int m_varId;
	vector<Conjunction const*> m_clauses;
	vector<VarId> m_pureVars;
	vector<VarId> m_uniteVars;

	bool m_backtracked;

	ConjunctionIter begin() const { return m_clauses.begin(); }
	ConjunctionIter end() const { return m_clauses.end(); }
};

enum ConjunctVal {
	True,
	False,
	Unset
};

class SatProblem;

class SplitHeuristic {
	public :
		SplitHeuristic(SatProblem *s, int nVars);

		bool next(vector<Conjunct>&);
		void init();

	protected :
		SatProblem *m_problem;
		int m_nVars;
		vector<Conjunct> m_chosenVars;

	private :
		bool nextCombination();
		bool m_moreCombination;
};

class FirstNHeuristic : public SplitHeuristic {
	public :
		FirstNHeuristic(SatProblem *s, int nVars):
			SplitHeuristic(s, nVars) { }

		void chooseVars();
};

class SatProblem {
	friend class SplitHeuristic;

	public :
		SatProblem(int numVars, int numClauses);
		~SatProblem();

		void addConjunction(Conjunction const* c) { m_conjunctions.push_back(c); }

		bool solve(vector<Conjunct>&);

		ConjunctionIter begin() const { return m_conjunctions.begin(); }
		ConjunctionIter end() const { return m_conjunctions.end(); }

		size_t size() const { return m_conjunctions.size(); }

	private :
		void detect_unite_clauses();
		void detect_pure_literals();

		bool pick_var(Conjunct& c) const;
		bool simplify();
		bool backtrack();

		int m_numVars;
		int m_numClauses;

		int m_index;
		ConjunctVal *m_values;
		Backtrack *m_backtracks;

		vector<Conjunction const*> m_conjunctions;
};

ostream& operator<<(ostream&, const Conjunct&);
ostream& operator<<(ostream&, const Conjunction&);
ostream& operator<<(ostream&, const SatProblem&);

#endif // _SAT_H_
