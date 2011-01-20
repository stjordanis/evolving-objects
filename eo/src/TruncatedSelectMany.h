/** -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

   -----------------------------------------------------------------------------
   TruncatedSelectMany.h
   (c) Maarten Keijzer, Marc Schoenauer, GeNeura Team, 2002

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
             Marc.Schoenauer@inria.fr
             mkeijzer@dhi.dk
 */
//-----------------------------------------------------------------------------

#ifndef _TruncatedSelectMany_h
#define _TruncatedSelectMany_h


//-----------------------------------------------------------------------------
#include <Select.h>
#include <SelectOne.h>
#include <utils/HowMany.h>
#include <math.h>
//-----------------------------------------------------------------------------

namespace eo
{

    /** TruncatedSelectMany selects many individuals using SelectOne as it's 
	mechanism. Therefore SelectMany needs an SelectOne in its ctor

	It will use an eoHowMnay to determine the number of guys to select,
	and push them to the back of the destination population.

	And it will only perform selection from the top guys in the population.

	It is NOT a special case of SelectMany because it needs to SORT 
	the population to discard the worst guys before doing the selection

	However, the same result can be obtained by embedding an 
	TruncatedSelectOne into an SelectMany ...

	@ingroup Selectors
    */
    template<class EOT>
    class TruncatedSelectMany : public Select<EOT>
    {
    public:
	/// Ctor
	TruncatedSelectMany(SelectOne<EOT>& _select, 
			      double  _rateGenitors, double  _rateFertile, 
			      bool _interpret_as_rateG = true, 
			      bool _interpret_as_rateF = true)
	    : select(_select), 
	      howManyGenitors(_rateGenitors, _interpret_as_rateG),
	      howManyFertile(_rateFertile, _interpret_as_rateF) {}

	// Ctor with HowManys
	TruncatedSelectMany(SelectOne<EOT>& _select, 
			      HowMany _howManyGenitors, HowMany _howManyFertile) 
	    : select(_select), howManyGenitors(_howManyGenitors),
	      howManyFertile(_howManyFertile) {}

	/**
	   The implementation repeatidly selects an individual

	   @param _source the source population
	   @param _dest  the resulting population (size of this population is the number of times SelectOne is called. It empties the destination and adds the selection into it)
	*/
	virtual void operator()(const Pop<EOT>& _source, Pop<EOT>& _dest)
	{
	    unsigned target = howManyGenitors(_source.size());

	    _dest.resize(target);

	    unsigned nbFertile = howManyFertile(_source.size());

	    //revert to standard selection (see SelectMany) if no truncation
	    if (nbFertile == _source.size())
		{
		    select.setup(_source);
	
		    for (size_t i = 0; i < _dest.size(); ++i)
			_dest[i] = select(_source);
		}
	    else
		{
		    // at the moment, brute force (rush rush, no good)
		    // what we would need otherwise is a std::vector<EOT &> class
		    // and selectors that act on such a thing
		    Pop<EOT> tmpPop = _source; // hum hum, could be a pain in the ass

		    tmpPop.sort();		   // maybe we could only do partial sort?
		    tmpPop.resize(nbFertile);  // only the best guys here now
		    tmpPop.shuffle();	   // as some selectors are order-sensitive

		    select.setup(tmpPop);
	
		    for (size_t i = 0; i < _dest.size(); ++i)
			_dest[i] = select(tmpPop);
		}
	}
  
    private :
	SelectOne<EOT>& select;	   // selector for one guy
	HowMany howManyGenitors;	   // number of guys to select
	HowMany howManyFertile;	   // number of fertile guys
    };

}

#endif
