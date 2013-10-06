/*
	This example demonstrates how to use basic features of mlog filters.

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

int main(int argc, char* argv[])
{
	mlog::cmodule_logger logger;
	logger.add_sink(new mlog::cout_sink);
	// add two file sinks, which will records only error and only warning messages
	mlog::cmodule_logger::sink_ptr perr(new mlog::const_cfile_sink("errors"));
	logger.add_sink(perr);
	mlog::cmodule_logger::sink_ptr pwarn(new mlog::const_cfile_sink("warnings"));
	logger.add_sink(pwarn);
	// format sinks in a simple way
	logger.set_format_all(mlog::cformat() << '[' << std::left << std::setw(8) << mlog::fp_level_name << "] " <<
		mlog::fp_message << std::endl);
	// add filters to sinks, to allow only messages with specified log level
	perr->add_filter(mlog::level_is<char>(mlog::err));
	pwarn->add_filter(mlog::level_is<char>(mlog::warn));
	// write log messages
	LOGC(logger, mlog::err) << "Error message";
	LOGC(logger, mlog::warn) << "Warning message";
	LOGC(logger, mlog::info) << "Information message, it is not written in files";
	return 0;
}
