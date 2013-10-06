/*
	This example demonstrates how to use several sinks with several
	log modules. It also shows that two sinks may have different
	formats and different filters.

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
	// create two logger modules
	mlog::cmodule_logger logger1("LOG1");
	mlog::cmodule_logger logger2("LOG2");
	// create a sink to write messages to console,
	// setup a simple format for the sink, and add the sink to both loggers
	mlog::cmodule_logger::sink_ptr cout_sink(new mlog::cout_sink);
	cout_sink->set_format(mlog::cformat() << mlog::fp_module << ": " << mlog::fp_message <<
		" (" << mlog::fp_file_short << '@' << mlog::fp_line << ')' << mlog::fp_newline);
	cout_sink->add_filter(mlog::message_not_contains("not to cout"));
	logger1.add_sink(cout_sink);
	logger2.add_sink(cout_sink);
	// create a sink to write messages to a file,
	// setup another format for the sink, and add the sink to both loggers
	mlog::cmodule_logger::sink_ptr file_sink(new mlog::const_cfile_sink("log_file_name"));
	file_sink->set_format(mlog::cformat() << mlog::fp_module << ": " << mlog::fp_message <<
		" (" << mlog::fp_year << '.' << mlog::fp_month << '.' << mlog::fp_day << ')' << mlog::fp_newline);
	file_sink->add_filter(mlog::message_not_contains("not to file"));
	logger1.add_sink(file_sink);
	logger2.add_sink(file_sink);
	// write log messages
	LOGC(logger1, mlog::warn) << "A test warning message";
	LOGC(logger2, mlog::warn) << "A test warning message";
	LOGC(logger1, mlog::info) << "Some information logged " << 5;
	LOGC(logger2, mlog::info) << "Some information logged " << 1;
	LOGC(logger1, mlog::err) << "error not to file";
	LOGC(logger2, mlog::err) << "error not to cout";
	return 0;
}
