/*
	This example demonstrates how to use basic features of mlog tags.

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
	// define the state of our process
	int process_state = 0;
	// define a tag name, which will be used to identify global process state
	const char ps_tag[] = "process_state";
	// insert the state tag into format
	logger.set_format_all(mlog::cformat() << '[' << mlog::tag(ps_tag) << "] " << mlog::fp_message << std::endl);
	// set initial tag value
	logger.set_tag(ps_tag, "starting");
	// log messages
	LOGC(logger, mlog::warn) << "This message is logged when system is starting";
	// note that we first change the tag value, and then change the process state
	logger.set_tag(ps_tag, "started");
	process_state = 1;
	// log messages
	LOGC(logger, mlog::warn) << "This message is logged when the system has started";
	return 0;
}
