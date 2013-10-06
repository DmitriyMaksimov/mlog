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

// this function defined in log_second.cpp
void log_messages();

// log module becomes global
mlog::cmodule_logger g_logger;
mlog::cmodule_logger_itf& g_logger_ref = g_logger;

// a copy paste of basic_usage.cpp
int main(int argc, char* argv[])
{
	g_logger.add_sink(new mlog::cout_sink);
	g_logger.add_sink(new mlog::const_cfile_sink("log_file_name"));
	mlog::cformat format;
	format << mlog::fp_level_name << ": " <<
		mlog::fp_year << '-' << mlog::fp_month << '-' << mlog::fp_day << ' ' <<
		mlog::fp_hour << ':' << mlog::fp_minute << ':' << mlog::fp_second << ':' << mlog::fp_milli << ' ' <<
		mlog::fp_message << " (" << mlog::fp_file_short << '@' << mlog::fp_line << ')' << mlog::fp_newline;
	g_logger.set_format_all(format);
	g_logger.set_level(mlog::dbg);
	// messages are logged from another function
	log_messages();
	return 0;
}
