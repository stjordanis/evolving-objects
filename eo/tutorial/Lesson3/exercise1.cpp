//-----------------------------------------------------------------------------
// SecondBitGA.cpp
//-----------------------------------------------------------------------------
//*
// Same code than FirstBitEA as far as Evolutionary Computation is concerned
// but now you learn to enter the parameters in a more flexible way
// and to twidle the output to your preferences!
//-----------------------------------------------------------------------------
// standard includes
#include <stdexcept>  // runtime_error 
#include <iostream>   // cout
#include <strstream>  // ostrstream, istrstream
#include <fstream>

// the general include for eo
#include <eo>
#include <utils/eoGnuplot1DMonitor.h>

// EVAL
#include "binary_value.h"

// REPRESENTATION
//-----------------------------------------------------------------------------
// define your genotype and fitness types
typedef eoBin<double> Indi;

// PARAMETRES
//-----------------------------------------------------------------------------
// instead of having all values of useful parameters as constants, read them:
// either on the command line (--option=value or -o=value)
//     or in a parameter file (same syntax, order independent, 
//                             # = usual comment character 
//     or in the environment (TODO)

// note that the parameters are passed by reference so they can be updated
void read_param(int argc, char *argv[], 
		uint32 & _seed,
		unsigned int & _vecSize,
		unsigned int & _popSize,
		unsigned int & _tSize,
		double & _pCross,
		double & _pMut,
		string & _load_name,
		unsigned int & _maxGen,
		unsigned int & _minGen,
		unsigned int & _steadyGen,
		double & _onePointRate, 
		double & _twoPointsRate, 
		double & _uRate, 
		double & _pMutPerBit, 
		double & _bitFlipRate, 
		double & _oneBitRate 
		)
{ 
  // define a parser from the command-line arguments
    eoParser parser(argc, argv);

    // For each parameter, define Parameters directly in the parser, 
    // and assign the value to the variable
    eoValueParam<uint32>& seedParam = parser.createParam<uint32>(time(0), "seed", "Random number seed", 'S');
    _seed = seedParam.value();

    eoValueParam<unsigned int>& vecSizeParam = parser.createParam<unsigned int>(8, "vecSize", "Genotype size",'V', "Representation");
    _vecSize = vecSizeParam.value();

    eoValueParam<unsigned int>& popSizeParam = parser.createParam<unsigned int>(10, "popSize", "Population size",'P', "Evolution");
    _popSize = popSizeParam.value();

    eoValueParam<unsigned int>& tSizeParam = parser.createParam<unsigned int>(10, "tSize", "Tournament size",'T', "Evolution");
    _tSize = tSizeParam.value();

    eoValueParam<string>& load_nameParam = parser.createParam<string>("", "Load","A save file to restart from",'L', "Persistence");
    _load_name = load_nameParam.value();
 
    eoValueParam<unsigned int>& maxGenParam = parser.createParam<unsigned int>(100, "maxGen", "Maximum number of generations",'G', "Stopping criterion");
    _maxGen = maxGenParam.value();

    eoValueParam<unsigned int>& minGenParam = parser.createParam<unsigned int>(100, "minGen", "Minimum number of generations",'g', "Stopping criterion");
    _minGen = minGenParam.value();

    eoValueParam<unsigned int>& steadyGenParam = parser.createParam<unsigned int>(100, "steadyGen", "Number of generations with no improvement",'s', "Stopping criterion");
    _steadyGen = steadyGenParam.value();

    eoValueParam<double>& pCrossParam = parser.createParam<double>(0.6, "pCross", "Probability of Crossover", 'C', "Genetic Operators"); 
    _pCross = pCrossParam.value();

    eoValueParam<double>& pMutParam = parser.createParam<double>(0.1, "pMut", "Probability of Mutation", 'M', "Genetic Operators");
    _pMut = pMutParam.value();

    eoValueParam<double>& onePointRateParam = parser.createParam<double>(1, "onePointRate", "Relative rate for one point crossover", '1', "Genetic Operators");
    _onePointRate = onePointRateParam.value();

    eoValueParam<double>& twoPointsRateParam = parser.createParam<double>(1, "twoPointRate", "Relative rate for two point crossover", '2', "Genetic Operators");
    _twoPointsRate = twoPointsRateParam.value();

    eoValueParam<double>& uRateParam = parser.createParam<double>(2, "uRate", "Relative rate for uniform crossover", 'U', "Genetic Operators");
    _uRate =  uRateParam.value();

    eoValueParam<double>& pMutPerBitParam = parser.createParam<double>(0.01, "pMutPerBit", "Probability of flipping 1 bit in bit-flip mutation", 'b', "Genetic Operators");
    _pMutPerBit = pMutPerBitParam.value();

    eoValueParam<double>& bitFlipRateParam = parser.createParam<double>(0.01, "bitFlipRate", "Relative rate for bit-flip mutation", 'B', "Genetic Operators");
    _bitFlipRate =  bitFlipRateParam.value();
      
    eoValueParam<double>& oneBitRateParam = parser.createParam<double>(0.01, "oneBitRate", "Relative rate for deterministic bit-flip mutation", 'D', "Genetic Operators");
      _oneBitRate = oneBitRateParam.value();

    // the name of the "status" file where all actual parameter values will be saved
    string str_status = parser.ProgramName() + ".status";
    eoValueParam<string>& status_nameParam = parser.createParam<string>(str_status.c_str(), "status","Status file",'S', "Persistence");

   // do the following AFTER ALL PARAMETERS HAVE BEEN PROCESSED
   // i.e. in case you need parameters somewhere else, postpone these
    if (parser.userNeedsHelp())
      {
        parser.printHelp(cout);
        exit(1);
      }
    if (status_nameParam.value() != "")
      {
	ofstream os(status_nameParam.value().c_str());
	os << parser;		// and you can use that file as parameter file
      }
}

// GENERAL
// now the main_function: nothing changed, except input/output
void main_function(int argc, char **argv)
{
// PARAMETRES
   uint32 seed;
   // decription of genotype
   unsigned int vecSize;
   // parameters for evolution engine
   unsigned int popSize;
   unsigned int tSize;
   // operators probabilities at the algorithm level
   double pCross;
   double pMut;
   // init and stop
   string load_name;
   unsigned int maxGen;
   unsigned int minGen;
   unsigned int steadyGen;
   // rates for crossovers
   double onePointRate;
   double twoPointsRate;
   double URate;
   // rates and private parameters for mutations;
   double pMutPerBit;
   double bitFlipRate;
   double oneBitRate;

   // Now read the parameters of the program
    read_param(argc, argv, seed, vecSize, popSize, tSize,
	       pCross, pMut, load_name, maxGen, minGen, steadyGen,
	       onePointRate, twoPointsRate, URate, 
	       pMutPerBit, bitFlipRate, oneBitRate );

// EVAL
  /////////////////////////////
  // Fitness function
  ////////////////////////////
  // Evaluation: from a plain C++ fn to an EvalFunc Object ...
  eoEvalFuncPtr<Indi, double, const vector<bool>& > plainEval( binary_value );
  // ... to an object that counts the nb of actual evaluations
  eoEvalFuncCounter<Indi> eval(plainEval);

// INIT
  ////////////////////////////////
  // Initilisation of population
  ////////////////////////////////
  // Either load or initialize
  // create an empty pop
  eoPop<Indi> pop;
  // create a state for reading
  eoState inState;		// a state for loading - WITHOUT the parser
  // register the rng and the pop in the state, so they can be loaded,
  // and the present run will be the exact conitnuation of the saved run
  // eventually with different parameters
  inState.registerObject(rng);
  inState.registerObject(pop);
    
  if (load_name != "")
    {
      inState.load(load_name); //  load the pop and the rng
      // the fitness is read in the file: 
      // do only evaluate the pop if the fitness has changed
    }
  else
    {
      rng.reseed(seed);
      // a Indi random initializer
      // based on boolean_generator class (see utils/rnd_generator.h)
      eoInitFixedLength<Indi, boolean_generator> 
	random(vecSize, boolean_generator());
	
      // Init pop from the randomizer: need to use the append function
      pop.append(popSize, random);      
      // and evaluate pop (STL syntax)    
      apply<Indi>(eval, pop);
    } // end of initializatio of the population

// OUTPUT    
  // sort pop for pretty printout
  //   pop.sort();
  // Print (sorted) intial population (raw printout)
  cout << "Initial Population" << endl << pop ;
  cout << "and best is " << pop.best_element() << "\n\n";
  cout << "and worse is " << pop.worse_element() << "\n\n";
// ENGINE
  /////////////////////////////////////
  // selection and replacement
  ////////////////////////////////////
// SELECT
  // The robust tournament selection
  eoDetTournament<Indi> selectOne(tSize);       // tSize in [2,POPSIZE]
  // is now encapsulated in a eoSelectPerc (entage)
  eoSelectPerc<Indi> select(selectOne);// by default rate==1

// REPLACE
  // And we now have the full slection/replacement - though with 
  // no replacement (== generational replacement) at the moment :-)
  eoNoReplacement<Indi> replace; 
  //  eoWeakElitistReplacement<Indi> replace(replace_main); 

// OPERATORS
  //////////////////////////////////////
  // The variation operators
  //////////////////////////////////////
// CROSSOVER
  // 1-point crossover for bitstring
  eoBinCrossover<Indi> xover1;
  // uniform crossover for bitstring
  eoBinUxOver<Indi> xoverU;
  // 2-pots xover
  eoBinNxOver<Indi> xover2(2);
  // Combine them with relative rates
  eoPropCombinedQuadOp<Indi> xover(xover1, onePointRate);
  xover.add(xoverU, URate);
  xover.add(xover2, twoPointsRate, true);

// MUTATION
  // standard bit-flip mutation for bitstring
  eoBinMutation<Indi>  mutationBitFlip(pMutPerBit);
  // mutate exactly 1 bit per individual
  eoDetBitFlip<Indi> mutationOneBit; 
  // Combine them with relative rates
  eoPropCombinedMonOp<Indi> mutation(mutationBitFlip, bitFlipRate);
  mutation.add(mutationOneBit, oneBitRate, true);

  // The operators are  encapsulated into an eoTRansform object
  eoSGATransform<Indi> transform(xover, pCross, mutation, pMut);

// STOP
  //////////////////////////////////////
  // termination condition see FirstBitEA.cpp
  /////////////////////////////////////
  eoGenContinue<Indi> genCont(maxGen);
  eoSteadyFitContinue<Indi> steadyCont(minGen, steadyGen);
  eoFitContinue<Indi> fitCont(vecSize);
  eoCombinedContinue<Indi> continuator(genCont);
  continuator.add(steadyCont);
  continuator.add(fitCont);
  
  
// CHECKPOINT
  // but now you want to make many different things every generation 
  // (e.g. statistics, plots, ...).
  // the class eoCheckPoint is dedicated to just that:

  // Declare a checkpoint (from a continuator: an eoCheckPoint 
  // IS AN eoContinue and will be called in the loop of all algorithms)
  eoCheckPoint<Indi> checkpoint(continuator);
  
    // Create a counter parameter
    eoValueParam<unsigned> generationCounter(0, "Gen.");
    
    // Create an incrementor (sub-class of eoUpdater). Note that the 
    // parameter's value is passed by reference, 
    // so every time the incrementer is updated (every generation),
    // the data in generationCounter will change.
    eoIncrementor<unsigned> increment(generationCounter.value());

    // Add it to the checkpoint, 
    // so the counter is updated (here, incremented) every generation
    checkpoint.add(increment);

    // now some statistics on the population:
    // Best fitness in population
    eoBestFitnessStat<Indi> bestStat;
    eoAverageStat<Indi> averageStat;
    // Second moment stats: average and stdev
    eoSecondMomentStats<Indi> SecondStat;

    // Add them to the checkpoint to get them called at the appropriate time
    checkpoint.add(bestStat);
    checkpoint.add(averageStat);
    checkpoint.add(SecondStat);

    // The Stdout monitor will print parameters to the screen ...
    eoStdoutMonitor monitor(false);
     
    // when called by the checkpoint (i.e. at every generation)
    checkpoint.add(monitor);

    // the monitor will output a series of parameters: add them 
    monitor.add(generationCounter);
    monitor.add(eval);		// because now eval is an eoEvalFuncCounter!
    monitor.add(bestStat);
    monitor.add(SecondStat);

    // A file monitor: will print parameters to ... a File, yes, you got it!
    eoFileMonitor fileMonitor("stats.xg", " ");
    eoGnuplot1DMonitor gnuMonitor("best_average.xg");
     
    // the checkpoint mechanism can handle multiple monitors
    checkpoint.add(fileMonitor);
    checkpoint.add(gnuMonitor);

    // the fileMonitor can monitor parameters, too, but you must tell it!
    fileMonitor.add(generationCounter);
    fileMonitor.add(bestStat);
    fileMonitor.add(SecondStat);
    // the fileMonitor can monitor parameters, too, but you must tell it!
    gnuMonitor.add(eval);
    gnuMonitor.add(bestStat);
    gnuMonitor.add(averageStat);

    // Last type of item the eoCheckpoint can handle: state savers:
    eoState outState;
    // Register the algorithm into the state (so it has something to save!!)
    outState.registerObject(rng);
    outState.registerObject(pop);

    // and feed the state to state savers
    // save state every 100th  generation
    eoCountedStateSaver stateSaver1(100, outState, "generation"); 
    // save state every 1 seconds 
    eoTimedStateSaver   stateSaver2(1, outState, "time"); 

    // Don't forget to add the two savers to the checkpoint
    checkpoint.add(stateSaver1);
    checkpoint.add(stateSaver2);
    // and that's it for the (control and) output

// GENERATION
  /////////////////////////////////////////
  // the algorithm
  ////////////////////////////////////////

  // Easy EA requires 
  // selection, transformation, eval, replacement, and stopping criterion
  eoEasyEA<Indi> gga(checkpoint, eval, select, transform, replace);

  // Apply algo to pop - that's it!
  gga(pop);
  
// OUTPUT
  // Print (sorted) intial population
  pop.sort();
  cout << "FINAL Population\n" << pop << endl;
// GENERAL
}

// A main that catches the exceptions
int main(int argc, char **argv)
{
#ifdef _MSC_VER
    int flag = _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
     flag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(flag);
//   _CrtSetBreakAlloc(100);
#endif

    try
    {
        main_function(argc, argv);
    }
    catch(exception& e)
    {
        cout << "Exception: " << e.what() << '\n';
    }

    return 1;
}