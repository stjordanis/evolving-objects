//-----------------------------------------------------------------------------

// to avoid long name warnings
#pragma warning(disable:4786)

#include <stdexcept>  // runtime_error 

//-----------------------------------------------------------------------------
// tt.cpp: 
//
//-----------------------------------------------------------------------------


// general
#include <utils/eoRNG.h>		// Random number generators
#include <ga/eoBin.h>
#include <utils/eoParser.h>
#include <utils/eoState.h>

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

int the_main(int argc, char **argv)
{ // ok, we have a command line parser and a state
  
    typedef eoBin<float> Chrom;

    eoParser parser(argc, argv);
      
    // Define Parameters
    eoValueParam<unsigned> chrom_size(2, "chrom-size", "Chromosome size");
    eoValueParam<double> rate(0.01, "mutationRatePerBit", "Initial value for mutation rate per bit"); 
    eoValueParam<double> factor(0.99, "mutationFactor", "Decrease factor for mutation rate");
    eoValueParam<uint32> seed(time(0), "seed", "Random number seed");
    eoValueParam<string> load_name("", "Load","Load",'L');
    eoValueParam<string> save_name("", "Save","Save",'S');
 
    // Register them
    parser.processParam(chrom_size, "Representation");
    parser.processParam(rate, "Genetic Operators");
    parser.processParam(factor, "Genetic Operators");
    parser.processParam(load_name, "Persistence");
    parser.processParam(save_name, "Persistence");
    parser.processParam(seed,      "Rng seeding");

   eoState state;
   state.registerObject(parser);
 
   if (load_name.value() != "")
   { // load the parser. This is only neccessary when the user wants to 
     // be able to change the parameters in the state file by hand.
       state.load(load_name.value()); // load the parser
   }
  
    // Create the algorithm here
    
    // Register the algorithm
    state.registerObject(rng);
    //state.registerObject(pop);

    if (parser.userNeedsHelp())
    {
        parser.printHelp(cout);
        return 0;
    }

    // Either load or initialize
    if (load_name.value() != "")
    {
        state.load(load_name.value()); // load the rest
    }
    else
    {
        // else

        // initialize rng and population

        rng.reseed(seed.value());
    }

    // run the algorithm

    // Save when needed
    if (save_name.value() != "")
    {
        string file_name = save_name.value();
        save_name.value() = ""; // so that it does not appear in the parser section of the state file
        state.save(file_name);
    }

    for (int i = 0; i < 100; ++i)
        rng.rand();

    cout << "a random number is " << rng.random(1024) << endl;;

    return 1;
}

int main(int argc, char **argv)
{
    try
    {
        the_main(argc, argv);
    }
    catch(exception& e)
    {
        cout << "Exception: " << e.what() << endl;
    }

    return 1;
}