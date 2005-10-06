/*	    
 *             Copyright (C) 2005 Maarten Keijzer
 *
 *          This program is free software; you can redistribute it and/or modify
 *          it under the terms of version 2 of the GNU General Public License as 
 *          published by the Free Software Foundation. 
 *
 *          This program is distributed in the hope that it will be useful,
 *          but WITHOUT ANY WARRANTY; without even the implied warranty of
 *          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *          GNU General Public License for more details.
 *
 *          You should have received a copy of the GNU General Public License
 *          along with this program; if not, write to the Free Software
 *          Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <vector>
#include <valarray>

#include "ErrorMeasure.h"
#include "Dataset.h"
#include "Sym.h"
#include "sym_compile.h"
#include "TargetInfo.h"

using namespace std;

#ifdef INTERVAL_DEBUG

#include <BoundsCheck.h>
#include <FunDef.h>

vector<double> none;
IntervalBoundsCheck bounds(none, none);

#endif



static double not_a_number = atof("nan");

class ErrorMeasureImpl {
    public:
	const Dataset& data;
	TargetInfo train_info;
	
	ErrorMeasure::measure measure;
	
	Scaling no_scaling;
	
	ErrorMeasureImpl(const Dataset& d, double t_p, ErrorMeasure::measure m) : data(d), measure(m) {
	    
#ifdef INTERVAL_DEBUG
	    bounds = IntervalBoundsCheck(d.input_minima(), d.input_maxima());
#endif

	    unsigned nrecords = d.n_records();
	    unsigned cases = unsigned(t_p * nrecords);
	    
	    valarray<double> t(cases);

	    for (unsigned i = 0; i < cases; ++i) {
		t[i] = data.get_target(i);
	    }

	    train_info = TargetInfo(t);
	    no_scaling = Scaling(new NoScaling);
	}
	
	ErrorMeasure::result eval(const valarray<double>& y) {
	    
	    ErrorMeasure::result result;
	    result.scaling = no_scaling;
	    
	    
	    switch(measure) {
		case ErrorMeasure::mean_squared:
		    result.error = pow(train_info.targets() - y, 2.0).sum() / y.size();
		    return result;
		case ErrorMeasure::absolute:
		    result.error = abs(train_info.targets() - y).sum() / y.size();
		    return result;
		case ErrorMeasure::mean_squared_scaled:
		    result.scaling = ols(y, train_info);
		    result.error  = pow(train_info.targets() - result.scaling->transform(y), 2.0).sum() / y.size();
		    return result;
		default: 
		    cerr << "Unknown measure encountered: " << measure << " " << __FILE__ << " " << __LINE__ << endl;
	    }
	    
	    return result;
	}   
	
	unsigned train_cases() const {
	    return train_info.targets().size();
	}

	vector<ErrorMeasure::result> multi_function_eval(const vector<Sym>& pop) {
	    
	    multi_function all = compile(pop);
	    std::vector<double> y(pop.size());
	    std::vector<double> err(pop.size()); 
	    
	    const std::valarray<double>& t = train_info.targets();
	    
	    for (unsigned i = 0; i < train_cases(); ++i) {
		// evaluate
		all(&data.get_inputs(i)[0], &y[0]);

		for (unsigned j = 0; j < y.size(); ++j) {
		    double diff = y[j] - t[i];
		    if (measure == ErrorMeasure::mean_squared) { // branch prediction will probably solve this inefficiency
			err[j] += diff * diff;
		    } else {
			err[j] += fabs(diff);
		    }
		    
		}
		
	    }
	    
	    std::vector<ErrorMeasure::result> result(pop.size());

	    double n = train_cases();
	    Scaling no = Scaling(new NoScaling);
	    for (unsigned i = 0; i < pop.size(); ++i) {
		result[i].error = err[i] / n;
		result[i].scaling = no;
	    }

	    return result;
	    
	}

	vector<ErrorMeasure::result> single_function_eval(const vector<Sym> & pop) {
	    
	    vector<single_function> funcs(pop.size());
	    compile(pop, funcs); // get one function pointer for each individual

	    valarray<double> y(train_cases());
	    vector<ErrorMeasure::result> result(pop.size());
	    for (unsigned i = 0; i < funcs.size(); ++i) {
		for (unsigned j = 0; j < train_cases(); ++j) {
		    y[j] = funcs[i](&data.get_inputs(j)[0]);
		}
	
#ifdef INTERVAL_DEBUG
		//cout << "eval func " << i << " " << pop[i] << endl;
		pair<double, double> b = bounds.calc_bounds(pop[i]);
		
		// check if y is in bounds
		for (unsigned j = 0; j < y.size(); ++j) {
		    if (y[j] < b.first -1e-4 || y[j] > b.second + 1e-4 || !finite(y[j])) {
			cout << "Error " << y[j] << " not in " << b.first << ' ' << b.second << endl;
			cout << "Function " << pop[i] << endl;
			exit(1);
		    }
		}
#endif
		
		result[i] = eval(y);
	    }
	    
	    return result; 
	}
	
	vector<ErrorMeasure::result> calc_error(const vector<Sym>& pop) {
	
	    // currently we can only accumulate simple measures such as absolute and mean_squared
	    switch(measure) {
		case ErrorMeasure::mean_squared:
		case ErrorMeasure::absolute:
		    return multi_function_eval(pop);
		case ErrorMeasure::mean_squared_scaled:
		    return single_function_eval(pop);
	    }
	    
	    return vector<ErrorMeasure::result>();
	}
	
};

ErrorMeasure::result::result() {
    error = 0.0;
    scaling = Scaling(0);
}

bool ErrorMeasure::result::valid() const {
    return isfinite(error);
}

ErrorMeasure::ErrorMeasure(const Dataset& data, double train_perc, measure meas) {
    pimpl = new ErrorMeasureImpl(data, train_perc, meas);
}

ErrorMeasure::~ErrorMeasure() { delete pimpl; }
ErrorMeasure::ErrorMeasure(const ErrorMeasure& that) { pimpl = new ErrorMeasureImpl(*that.pimpl); }


ErrorMeasure::result ErrorMeasure::calc_error(Sym sym) {
   
    single_function f = compile(sym);
    
    valarray<double> y(pimpl->train_cases());
     
    for (unsigned i = 0; i < y.size(); ++i) {

	y[i] = f(&pimpl->data.get_inputs(i)[0]);
	
	if (!finite(y[i])) {
	    result res;
	    res.scaling = Scaling(new NoScaling);
	    res.error = not_a_number;
	    return res;
	}
	
    }
   
    return pimpl->eval(y); 
}

vector<ErrorMeasure::result> ErrorMeasure::calc_error(const vector<Sym>& syms) {
    return pimpl->calc_error(syms);
    
}

double ErrorMeasure::worst_performance() const {
    
    if (pimpl->measure == mean_squared_scaled) {
	return pimpl->train_info.tvar();
    }
    
    return 1e+20; // TODO: make this general
}
