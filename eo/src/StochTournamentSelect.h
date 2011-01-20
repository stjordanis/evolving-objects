// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// StochTournamentSelect.h
// (c) GeNeura Team, 1998 - EEAAX 1999
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
 */

#ifndef StochTournamentSelect_h
#define StochTournamentSelect_h

#include <functional>
#include <numeric>           // accumulate
#include <SelectOne.h>     // SelectOne
#include <utils/selectors.h> // stochastic_tournament

namespace eo
{

    /** StochTournamentSelect: a selection method that selects ONE individual by
	binary stochastic tournament 
	-MS- 24/10/99 
 
	@ingroup Selectors
    */
    template <class EOT> class StochTournamentSelect: public SelectOne<EOT>
    {
    public:

	///
	StochTournamentSelect(double _Trate = 1.0 ) : SelectOne<EOT>(), Trate(_Trate) 
	{
	    // consistency checks
	    if (Trate < 0.5) {
		std::cerr << "Warning, Tournament rate should be > 0.5\nAdjusted to 0.55\n";
		Trate = 0.55;
	    }
	    if (Trate > 1) {
		std::cerr << "Warning, Tournament rate should be < 1\nAdjusted to 1\n";
		Trate = 1;
	    }
	}
  
	/** Perform the stochastic tournament  */
	virtual const EOT& operator()(const Pop<EOT>& pop) 
	{
	    return stochastic_tournament(pop, Trate);
	}
  
    private:
	double Trate;
    };

}

#endif
