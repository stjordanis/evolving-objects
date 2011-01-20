// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// SimpleEDA.h
// (c) Marc Schoenauer, Maarten Keijzer, 2001
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

    Contact: Marc.Schoenauer@polytechnique.fr
             mkeijzer@dhi.dk
 */
//-----------------------------------------------------------------------------

#ifndef _SimpleEDA_h
#define _SimpleEDA_h

//-----------------------------------------------------------------------------

#include <apply.h>
#include <EDA.h>
#include <Continue.h>
#include <DistribUpdater.h>
#include <EvalFunc.h>

namespace eo
{

    /** A very simple Estimation of Distribution Algorithm
     *  
     *  The algorithm that evolves a probability distribution 
     *  on the spaces of populations with the loop
     *                 generate a population from the current distribution
     *                 evaluate that population
     *                 update the distribution
     *
     *  @ingroup Algorithms
     */

    template<class EOT> class SimpleEDA: public EDA<EOT>
    {
    public:

	/** Ctor from an DistribUpdater,
	 *  plus an Eval and Continue of course.
	 */
	SimpleEDA(DistribUpdater<EOT>& _update,
		    EvalFunc<EOT>& _eval,
		    unsigned _popSize, 
		    Continue<EOT>& _continuator
		    ) : 
	    update(_update),
	    eval(_eval),
	    popSize(_popSize),
	    continuator(_continuator)
	{}

	/** The algorithm: 
	 *    generate pop from distrib, 
	 *    evaluate pop, 
	 *    update distrib
	 */
	virtual void operator()(Distribution<EOT>& _distrib)
	{
	    Pop<EOT> pop(popSize, _distrib);
	    do
		{
		    try
			{
			    apply<EOT>(_distrib, pop); // re-init. of _pop from distrib

			    apply<EOT>(eval, pop);     // eval of current population

			    update(_distrib, pop);     // updates distrib from _pop
	
			}
		    catch (std::exception& e)
			{
			    std::string s = e.what();
			    s.append( " in SimpleEDA");
			    throw std::runtime_error( s );
			}
		} while ( continuator( pop ) );
	}

    private:
	DistribUpdater<EOT> & update;

	EvalFunc<EOT>&          eval;
  
	unsigned                  popSize;

	Continue<EOT>&          continuator;
    };

    //-----------------------------------------------------------------------------

}

#endif
