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

// we include only light version of header file
#include "log.h"

// log module is defined and initialized in log_first.cpp
extern mlog::cmodule_logger_itf& g_logger_ref;

void log_messages()
{
	g_logger_ref.set_tag(1, "tags can be modified by using interfaces");
	// log messages
	LOGC(g_logger_ref, mlog::crit) << "critical error message";
	LOGC(g_logger_ref, mlog::err) << "error message";
	LOGC(g_logger_ref, mlog::warn) << "warning message";
	LOGC(g_logger_ref, mlog::info) << "informational message";
	LOGC(g_logger_ref, mlog::dbg) << "debug message";
	LOGC(g_logger_ref, mlog::trace) << "trace message";
}
