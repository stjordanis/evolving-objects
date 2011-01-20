// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// AssembledFitnessStat.h
// Marc Wintermantel & Oliver Koenig
// IMES-ST@ETHZ.CH
// April 2003

//-----------------------------------------------------------------------------
// (c) Marc Schoenauer, Maarten Keijzer and GeNeura Team, 2000
/*
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

    Contact: todos@geneura.ugr.es, http://geneura.ugr.es
             Marc.Schoenauer@polytechnique.fr
             mkeijzer@dhi.dk
 */
//-----------------------------------------------------------------------------

#ifndef _AssembledFitnessStat_h
#define _AssembledFitnessStat_h

#include <utils/Stat.h>
#include <ScalarFitnessAssembled.h>

namespace eo
{

    /** @addtogroup Stats
     * @{
     */

    /**
       Average fitness values of a population, where the fitness is
       of type ScalarAssembledFitness. Specify in the constructor,
       for which fitness term (index) the average should be evaluated.
       Only values of object where the failed boolean = false is set are counted.
    */
    template <class EOT>
    class AssembledFitnessAverageStat : public Stat<EOT, double>
    {
    public :

	using Stat<EOT, double>::value;

	typedef typename EOT::Fitness Fitness;


	AssembledFitnessAverageStat(unsigned _whichTerm=0, std::string _description = "Average Fitness")
	    : Stat<EOT, double>(Fitness(), _description), whichFitnessTerm(_whichTerm)
        {}


	virtual void operator()(const Pop<EOT>& _pop) {
	    if( whichFitnessTerm >= _pop[0].fitness().size() )
		throw std::logic_error("Fitness term requested out of range");

	    double result =0.0;
	    unsigned count = 0;
	    for (typename Pop<EOT>::const_iterator it = _pop.begin(); it != _pop.end(); ++it){
		if ( it->fitness().failed == false ){
		    result+= it->fitness()[whichFitnessTerm];
		    ++count;
		}
	    }

	    value() = result / (double) count;
	}

    private:
	// Store an index of the fitness term to be evaluated in ScalarFitnessAssembled
	unsigned whichFitnessTerm;
    };

    /**
       Fitness values of best individuum in a population, where the fitness is
       of type ScalarAssembledFitness. Specify in the constructor,
       for which fitness term (index) the value should be evaluated.
    */
    template <class EOT>
    class AssembledFitnessBestStat : public Stat<EOT, double>
    {
    public:

	using Stat<EOT, double>::value;

	typedef typename EOT::Fitness Fitness;

	AssembledFitnessBestStat(unsigned _whichTerm=0, std::string _description = "Best Fitness")
	    : Stat<EOT, double>(Fitness(), _description), whichFitnessTerm(_whichTerm)
        {}

	virtual void operator()(const Pop<EOT>& _pop) {
	    if( whichFitnessTerm >= _pop[0].fitness().size() )
		throw std::logic_error("Fitness term requested out of range");

	    value() = _pop.best_element().fitness()[whichFitnessTerm];
	}

    private:

	// Store an index of the fitness term to be evaluated in ScalarFitnessAssembled
	unsigned whichFitnessTerm;
    };

}

/** @} */
#endif
