#include "DpsSat.h"
#include "Sat.h"

static SatProblem* DpsToSat(SatData *s)
{
	SatProblem *s = new SatProblem(s->m_nVars, s->m_nConjunctions);
	VarData *vd = s->m_conjunctions;

	for(int i=0; i<s->m_nConjunctions; ++i) {
		Conjunction c = new Conjunction();
		for(int j=0; j<Conjunction::Size; ++j, ++vd) {
			c[j] = Conjunct(vd->m_varId, vd->m_isInverted);
		}
	}

	return s;
}

template <typename H>
void Split<H>::execute(SatProblem *in)
{
	SatProblem *s = DpsToSat(in);
	vector<Conjunct> partialSolution;
	H choice(s);
	choice.chooseVars();

	while(choice.next(partialSolution)) {
		/* if(s->simplify(partialSolution)) {
			cout << "Problem is satisfiable" << endl;
			return;
		} */

		SatData sd = new SatData;
		sd->m_nPredefinedVars = partialSolution.size();
		sd->m_nVars = in->m_nVars - partialSolution.size();
		sd->m_nConjunctions = sd->size();
		sd->m_conjunctions.resize(sd->size());
		sd->m_predefinedVars.resize(partialSolution.size());

		VarData *vd = sd->m_predefinedVars;
		for(vector<Conjunct>::const_iterator it = partialSolution.begin();
				it != choice.end(); ++it, ++vd) {
			vd->m_isInverted = it->inverted();
			vc->m_varId = it->id();
		}

		vd = sd->m_conjunctions;
		for(ConjunctionIter it = sd->begin(); it != sd->end(); ++it) {
			for(int i=0; i<Conjunction::Size; ++i, ++vd) {
				vd->m_isInverted = ((**it)[i]).inverted();
				vd->m_varId = ((**it)[i]).id();;
			}
		}
		postDataObject(sd);
	}
}

void Merge::execute(SatSolution *in)
{
	SatSolution *s = new SatSolution;
	s->m_isSatisfiable = false;

	do {
		if(in->m_isSatisfiable) {
			s->m_isSatisfiable = true;
			s->m_nVars = in->m_nVars;
			memcpy(s->m_vars, in->vars, sizeof(VarData) * in->m_nVars);
		}
	} while(in=waitForNextDataObject());

	postDataObject(s);
}

void Solve::execute(SatData *in)
{
	SatProblem *s = DpsToSat(in);
	vector<Conjunct> solution;
	SatSolution *dpsSolution = new SatSolution;

	if(!s->solve(solution)) {
		dpsSolution->m_isSatisfiable = false;
	} else {
		dpsSolution->m_isSatisfiable = true;
		dpsSolution->m_varAssignment->resize(in->m_nVars+in->m_nPredefinedVars);
		dpsSolution->m_nVars = solution.size();

		VarData *vd = dpsSolution->m_varAssignment;
		for(vector<Conjunct>::const_iterator it=solution.begin();
				it != solution.end(); ++it, ++vd) {
			vd->m_isInverted = it->inverted();
			vd->m_varId = it->id();
		}
		memcpy(vd, in->m_predefinedVars, m_nPredefinedVars*sizeof(VarData));
	}

	postDataObject(dpsSolution);
	delete s;
}
