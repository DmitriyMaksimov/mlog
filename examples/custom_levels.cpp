/*
    Copyright (c) 2007-2008 by Mikhail Borbot <mike_n@hotbox.ru>

    This file is part of mlog

    mlog is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    mlog is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with mlog.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "log_system.h"

struct my_level_name : public mlog::level_name_provider<char>
{
	const char* level_name(int n) const
	{
		switch(n)
		{
		default:	return "UNKNOWN";
		case 1:		return "ONE";
		case 2:		return "TWO";
		case 3:		return "THREE";
		case 4:		return "FOUR";
		}
	}
};

int main(int, char*[])
{
	// create a logger module
	mlog::cmodule_logger logger;
	// add a sink, which writes messages to stdout
	logger.add_sink(new mlog::cout_sink);
	// create format, which is a combination of several log parts and character data
	mlog::cformat format;
	format << mlog::fp_level_name << ": " << mlog::fp_message << mlog::fp_newline;
	// set format on each sink
	logger.set_format_all(format);
	// setup custom log levels
	logger.set_level(3);
	logger.set_level_name_provider(mlog::clevel_name_provider_ptr(new my_level_name));
	// log messages with custom log level names
	LOGC(logger, 1) << "first level message";
	boost::this_thread::sleep(boost::posix_time::millisec(100));
	LOGC(logger, 2) << "second level message";
	boost::this_thread::sleep(boost::posix_time::millisec(100));
	LOGC(logger, 3) << "third level message";
	boost::this_thread::sleep(boost::posix_time::millisec(100));
	LOGC(logger, 4) << "fourth level message, it is not displayed";
	return 0;
}
