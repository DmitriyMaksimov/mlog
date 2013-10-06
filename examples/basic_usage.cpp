/*
	This example demonstrates how to use basic features of mlog:
	log module, sinks, format; and how to make logging calls.

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
#include <boost/thread.hpp>

int main(int argc, char* argv[])
{
	// create a log module
	mlog::cmodule_logger logger;
	// add a sink, which writes messages to stdout
	logger.add_sink(new mlog::cout_sink);
	// add a sink, which writes messages to a file
	logger.add_sink(new mlog::const_cfile_sink("log_file_name"));
	// create format, which is a combination of several log parts and character data
	mlog::cformat format;
	format << std::setw(8) << std::setfill(' ') << std::left << mlog::fp_level_name << ": " <<
		mlog::fp_year << '-' << mlog::fp_month << '-' << mlog::fp_day << ' ' <<
		mlog::fp_hour << ':' << mlog::fp_minute << ':' << mlog::fp_second << ':' << mlog::fp_milli << ' ' <<
		mlog::fp_message << " (" << mlog::fp_file_short << '@' << mlog::fp_line << ')' << std::endl;
	// set format on each sink
	logger.set_format_all(format);
	// set log level to debug, messages that are above this level are not logged
	// (only trace messages are not logged in this case)
	logger.set_level(mlog::dbg);
	// log messages
	LOGC(logger, mlog::crit) << "A critical error message";
	boost::this_thread::sleep(boost::posix_time::millisec(100));
	LOGC(logger, mlog::err) << "An error message";
	boost::this_thread::sleep(boost::posix_time::millisec(100));
	LOGC(logger, mlog::warn) << "A warning message";
	boost::this_thread::sleep(boost::posix_time::millisec(100));
	LOGC(logger, mlog::info) << "An informational message";
	boost::this_thread::sleep(boost::posix_time::millisec(100));
	LOGC(logger, mlog::dbg) << "A debug message";
	boost::this_thread::sleep(boost::posix_time::millisec(100));
	LOGC(logger, mlog::trace) << "A trace message";
	return 0;
}
