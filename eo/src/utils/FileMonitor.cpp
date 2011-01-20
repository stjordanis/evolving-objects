#ifdef _MSC_VER
// to avoid long name warnings
#pragma warning(disable:4786)
#endif

#include <iostream>
#include <fstream>
#include <stdexcept>

#include <utils/FileMonitor.h>
#include <utils/compatibility.h>
#include <utils/Param.h>

namespace eo
{
    using namespace std;

    void FileMonitor::printHeader(std::ostream& os)
    {
	iterator it = vec.begin();

	os << (*it)->longName();

	++it;

	for (; it != vec.end(); ++it)
	    {
		// use the longName of the Param for the header
		os << delim.c_str() << (*it)->longName();
	    }
	os << std::endl;
    }

    void FileMonitor::printHeader()
    {
	// create file
	ofstream os(filename.c_str());

	if (!os)
	    {
		string str = "FileMonitor could not open: " + filename;
		throw runtime_error(str);
	    }

	printHeader(os);
    }

    Monitor& FileMonitor::operator()(void)
    {
	ofstream os(filename.c_str(),
		    overwrite ?
		    ios_base::out|ios_base::trunc // truncate
		    :
		    ios_base::out|ios_base::app   // append
		    );

	if (!os)
	    {
		string str = "FileMonitor could not write to: " + filename;
		throw runtime_error(str);
	    }

	if (
            header      // we want to write headers
	    &&  firstcall   // we do not want to write headers twice
	    && !keep        // if we append to an existing file, headers are useless
	    && !overwrite   // we do not want to write headers if the file is to be overwriten
	       ) {
	    printHeader();
	    firstcall = false;
	}

	return operator()(os);
    }

    Monitor& FileMonitor::operator()(std::ostream& os)
    {
	iterator it = vec.begin();

	os << (*it)->getValue();

	for(++it; it != vec.end(); ++it)
	    {
		os << delim.c_str() << (*it)->getValue();
	    }

	os << std::endl;

	return *this;
    }

}
