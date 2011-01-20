/** -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// EsGlobalXover.h : ES global crossover
// (c) Marc Schoenauer 2001

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: marc.schoenauer@polytechnique.fr http://eeaax.cmap.polytchnique.fr/
 */
//-----------------------------------------------------------------------------


#ifndef _EsGlobalXover_H
#define _EsGlobalXover_H

#include <utils/RNG.h>

#include <es/EsSimple.h>
#include <es/EsStdev.h>
#include <es/EsFull.h>

#include <GenOp.h>
// needs a selector - here random
#include <RandomSelect.h>

namespace eo
{

    /** Global crossover operator for ES genotypes.
     *  Uses some Atom crossovers to handle both the object variables
     *  and the mutation strategy parameters
     *
     *  @ingroup Real
     *  @ingroup Variators
     */
    template<class EOT>
    class EsGlobalXover: public GenOp<EOT>
    {
    public:
	typedef typename EOT::Fitness FitT;

	/**
	 * (Default) Constructor.
	 */
	EsGlobalXover(BinOp<double> & _crossObj, BinOp<double> & _crossMut) :
	    crossObj(_crossObj), crossMut(_crossMut) {}

	/// The class name. Used to display statistics
	virtual std::string className() const { return "EsGlobalXover"; }

	/// The TOTAL number of offspring (here = nb of parents modified in place)
	unsigned max_production(void) { return 1; }

	/**
	 * modifies one parents in the populator
	 *     using 2 new parents for each component!
	 *
	 * @param _plop a POPULATOR (not a simple population)
	 */
	void apply(Populator<EOT>& _plop)
	{
	    // First, select as many parents as you will have offspring
	    EOT& parent = *_plop; // select the first parent

	    // first, the object variables
	    for (unsigned i=0; i<parent.size(); i++)
		{
		    // get extra parents - use private selector
		    // _plop.source() is the Pop<EOT> used by _plop to get parents
		    const EOT& realParent1 = sel(_plop.source());
		    const EOT& realParent2 = sel(_plop.source());
		    parent[i] = realParent1[i];
		    crossObj(parent[i], realParent2[i]); // apply BinOp
		}
	    // then the self-adaptation parameters
	    cross_self_adapt(parent, _plop.source());
	    // dont' forget to invalidate
	    parent.invalidate();
	}

    private:

	/** Method for cross self-adaptation parameters

	    Specialization for EsSimple.
	*/
	void cross_self_adapt(EsSimple<FitT> & _parent, const Pop<EsSimple<FitT> >& _pop)
	{
	    const EOT& realParent1 = sel(_pop);
	    const EOT& realParent2 = sel(_pop);
	    _parent.stdev = realParent1.stdev;
	    crossMut(_parent.stdev, realParent2.stdev); // apply BinOp
	}

	/** Method for cross self-adaptation parameters

	    Specialization for EsStdev.
	*/
	void cross_self_adapt(EsStdev<FitT> & _parent, const Pop<EsStdev<FitT> >& _pop)
	{
	    for (unsigned i=0; i<_parent.size(); i++)
		{
		    const EOT& realParent1 = sel(_pop);
		    const EOT& realParent2 = sel(_pop);
		    _parent.stdevs[i] = realParent1.stdevs[i];
		    crossMut(_parent.stdevs[i], realParent2.stdevs[i]); // apply BinOp
		}
	}

	/** Method for cross self-adaptation parameters

	    Specialization for EsFull.
	*/
	void cross_self_adapt(EsFull<FitT> & _parent, const Pop<EsFull<FitT> >& _pop)
	{
	    unsigned i;
	    // the StDev
	    for (i=0; i<_parent.size(); i++)
		{
		    const EOT& realParent1 = sel(_pop);
		    const EOT& realParent2 = sel(_pop);
		    _parent.stdevs[i] = realParent1.stdevs[i];
		    crossMut(_parent.stdevs[i], realParent2.stdevs[i]); // apply BinOp
		}
	    // the roataion angles
	    for (i=0; i<_parent.correlations.size(); i++)
		{
		    const EOT& realParent1 = sel(_pop);
		    const EOT& realParent2 = sel(_pop);
		    _parent.correlations[i] = realParent1.correlations[i];
		    crossMut(_parent.correlations[i], realParent2.correlations[i]); // apply BinOp
		}

	}

	// the data
	RandomSelect<EOT> sel;
	BinOp<double> & crossObj;
	BinOp<double> & crossMut;
    };

}

#endif