#include "Common.h"
#include "FilesystemUtils.h"
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

void Utils::FilesystemUtils::CopyDirectory( const string& source, const string& destination, const string& includeRegexp, const string& excludeRegexp )
{
	ocInfo << "Copying " << source << " into " << destination;

	boost::filesystem::create_directories(destination);

	boost::regex includeFilter;
	boost::regex excludeFilter;

	try
	{
		includeFilter.assign(includeRegexp, boost::regex::extended|boost::regex::icase);
	}
	catch (std::exception& e)
	{
		ocError << "includeRegexp '" << includeRegexp << "' is not valid: " << e.what();
		return;
	}
	try
	{
		if (excludeRegexp.size()>0) excludeFilter.assign(excludeRegexp, boost::regex::extended|boost::regex::icase);
	}
	catch (std::exception& e)
	{
		ocError << "excludeRegexp '" << excludeRegexp << "' is not valid: " << e.what();
		return;
	}

	boost::filesystem::directory_iterator iend;
	for (boost::filesystem::directory_iterator i(source); i!=iend; ++i)
	{
		if (!regex_match(i->path().filename(), includeFilter)) continue;
		if (!excludeFilter.empty() && regex_match(i->path().filename(), excludeFilter)) continue;

		boost::filesystem::path destinationPath = destination + "/" + i->path().filename();
		if (boost::filesystem::is_directory(i->path()))
		{	
			CopyDirectory(i->path().string(), destinationPath.string(), includeRegexp, excludeRegexp);
		}
		else
		{
			boost::filesystem::copy_file(i->path(), destinationPath, boost::filesystem::copy_option::overwrite_if_exists);
		}
	}

}