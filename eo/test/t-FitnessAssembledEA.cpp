// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-
 
//-----------------------------------------------------------------------------
// t-FitnessAssembledEA.cpp
// Marc Wintermantel & Oliver Koenig
// IMES-ST@ETHZ.CH
// March 2003

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
             Marc.Schoenauer@inria.fr
             mak@dhi.dk
*/
//-----------------------------------------------------------------------------
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cmath>

// General eo includes
#include <eo>
#include <utils/RealVectorBounds.h>	// The real bounds (not yet in general eo include)

// Representation dependent includes and typedefs
#include <es/Real.h>			// Definition of representation
#include <es/RealInitBounded.h>	// Uniformly initializes real vector in bounds
#include <es/make_genotype_real.h>		// Initialization of a genotype
#include <EvalFunc.h>		        // Base class for fitness evaluation 
#include <es/make_op_real.h>		// Variation operators using standard Real operators
#include <ScalarFitnessAssembled.h>     // The fitness class

// Representation independent modules
#include <do/make_pop.h>		// Initialization of population
#include <do/make_continue.h>		// The stopping criterion
#include <do/make_checkpoint_assembled.h>	// Outputs (stats, population dumps, ...)
#include <do/make_algo_scalar.h>	// Evolution engine (selection and replacement)
#include <do/make_run.h>		// simple call to the algo.stays there for consistency reasons 

using namespace eo;

typedef Real<AssembledMinimizingFitness> Indi;

// Define a fitness class
template <class EOT>
class AssembledEvalFunc : public EvalFunc<EOT>{
public:
  // Constructor defining number and descriptions of fitness terms
  AssembledEvalFunc() {
    
    // Define a temporary fitness object to have access to its static traits
    typename EOT::Fitness tmpfit(3, 0.0);    
    tmpfit.setDescription(0,"Fitness");
    tmpfit.setDescription(1,"Some Value");
    tmpfit.setDescription(2,"Other Value");

  }

  void operator()(EOT& _eo){

    // Define temporary fitness object 
    // (automatically gets initialized with size given in constructor)
    typename EOT::Fitness tmpfit;
    
    // Eval some dummy fitness
    double sum1=0.0, sum2=0.0;
    for (unsigned i=0; i < _eo.size(); ++i){
      sum1 += _eo[i]*_eo[i];
      sum2 += fabs(_eo[i]) + fabs(_eo[i]);
    }
    
    // Store some fitness terms
    tmpfit[1]= sum1;
    tmpfit[2]= sum2;
    
    // Store the fitness
    tmpfit = (sum1 + sum2)/_eo.size();

    // Pass it
    _eo.fitness( tmpfit );
  
  }
};

// checks for help demand, and writes the status file and make_help; in libutils
//void make_help(Parser & _parser);

// now use all of the above, + representation dependent things
int main(int argc, char* argv[]){
  
  std::cout << "-----------------------------------" << std::endl;
  std::cout << "START t-FitnessAssembledEA" << std::endl;

  try{

    // Parser & State
    Parser parser(argc, argv);  // for user-parameter reading    
    State state;    // keeps all things allocated

    ////
    // A) Representation dependent stuff
    ////

    // The fitness
    AssembledEvalFunc<Indi> plainEval;
    // turn that object into an evaluation counter
    EvalFuncCounter<Indi> eval(plainEval);

    // The genotype
    RealInitBounded<Indi>& init = do_make_genotype(parser, state, Indi() );
    
    // The variation operators
    GenOp<Indi>& op = do_make_op(parser, state, init);

    ////
    // B) Create representation independent stuff
    ////

    // initialize the population
    // yes, this is representation indepedent once you have an Init
    Pop<Indi>& pop   = do_make_pop(parser, state, init);

    // stopping criteria
    Continue<Indi> & term = do_make_continue(parser, state, eval);
    // output
    CheckPoint<Indi> & checkpoint = do_make_checkpoint_assembled(parser, state, eval, term);
    // algorithm (need the operator!)
    Algo<Indi>& ga = do_make_algo_scalar(parser, state, eval, checkpoint, op);


    make_help(parser);	// To be called after all parameters have been read !

    ////
    // C) Run the algorithm
    ////

    // evaluate intial population AFTER help and status in case it takes time
    apply<Indi>(eval, pop);
    // if you want to print it out
    std::cout << "Initial Population\n";
    pop.sortedPrintOn(std::cout);
    std::cout << std::endl;

    do_run(ga, pop); // run the ga

    std::cout << "Final Population\n";
    pop.sortedPrintOn(std::cout);
    std::cout << std::endl;
    
  }
  catch(std::exception& e)
    {
      std::cout << e.what() << std::endl;
      return 1;
    }

  std::cout << "-----------------------------------" << std::endl;
  std::cout << "END t-FitnessAssembledEA" << std::endl;

  return 0;

}
