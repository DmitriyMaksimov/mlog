/*
	This example demonstrates how to use several log modules, how to turn logging
	for that modules on and off, and how to redirect output of these modules
	between log sinks, shared by these modules.

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

// stub for the facade class of a complex system
class system_one
{
public:
	void do_operation()
	{
		LOGA(log(), mlog::info) << "Processing operation in system one";
	}

	void do_operation_after_filter()
	{
		LOGA(log(), mlog::info) << "Processing operation in system one, after filter was disabled";
	}

	static mlog::cmodule_logger& log()
	{
		static mlog::cmodule_logger log("system_one", 1);
		return log;
	}
};

// stub for the facade class of a complex system
class system_two
{
public:
	void do_operation()
	{
		LOGA(log(), mlog::info) << "Processing operation in system two";
	}

	static mlog::cmodule_logger& log()
	{
		static mlog::cmodule_logger log("system_two", 2);
		return log;
	}
};

int main(int argc, char* argv[])
{
	// create several sinks
	mlog::cmodule_logger::sink_ptr console(new mlog::cout_sink);
	mlog::cmodule_logger::sink_ptr file1(new mlog::const_cfile_sink("file1"));
	mlog::cmodule_logger::sink_ptr file2(new mlog::const_cfile_sink("file2"));
	// and all sinks to our complex systems, they will share the same sinks
	system_one::log().add_sink(console).add_sink(file1).add_sink(file2);
	system_two::log().add_sink(console).add_sink(file1).add_sink(file2);
	// format all sinks in a simple way, only one call to set_format_all is needed, because
	// each log module shares all sinks
	system_one::log().set_format_all(mlog::cformat() << '[' << std::left << std::setw(8) <<
		mlog::fp_level_name << "] " << mlog::fp_message << std::endl);
	// tell file1.log to save only log records from module "system_one"
	file1->add_filter(mlog::module_is("system_one"));
	// tell file2.log to save only log records from module "system_two", use module id this time
	mlog::cmodule_logger::filter_ptr pf(mlog::module_is<char>(2));
	file2->add_filter(pf);
	// create stubs for the facade class of some complex systems
	system_one one;
	system_two two;
	// call operations, which produce log messages
	for (int i = 0; i < 5; i++)
	{
		one.do_operation();
		two.do_operation();
	}
	// disable filter, which doesn't allow log records from system_one to be written to file2.log
	// using this method you may turn output from different log modules on and off
	pf->enable(false);
	one.do_operation_after_filter();
	return 0;
}
