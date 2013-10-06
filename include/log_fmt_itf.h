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

#pragma once

#include "log_fmt_time.h"
#ifdef _WIN32
	#pragma warning(push)
	#pragma warning(disable:4244)
	#pragma warning(disable:4512)
#endif
#include <boost/thread/thread.hpp>
#ifdef _WIN32
	#pragma warning(pop)
#endif

namespace mlog
{

// this interface may be used to retrieve data for each log line
template<class TCH>
struct format_provider
{
	virtual ~format_provider() throw() { }

	// return line in the source code, on which log message appears
	virtual unsigned int get_line() const throw() = 0;

	// return file with the source code, where log message appears
	// if short_name is true, then this function skips full path and returns only filename
	virtual const TCH* get_file(bool short_name) const throw() = 0;

	// return function, which contains line, where log message appears
	virtual const TCH* get_function() const throw() = 0;

	// return log level, with which log message is being written
	virtual int get_level() const throw() = 0;

	// return string representation of the log level
	virtual const TCH* get_level_name() const throw() = 0;

	// return current date, time, and milliseconds
	virtual const detail::time_type* get_time() const throw() = 0;

	// return a tag value, which corresponds to the provided tag names
	virtual const TCH* get_tag(const TCH* name) const = 0;
	virtual const TCH* get_tag(int id) const = 0;

	// return id of the thread, which is writing log message
	virtual boost::thread::id get_thread() const throw() = 0;

	// return id of the process, which is writing log message
	virtual int get_pid() const throw() = 0;

	// return log module name, to which log message is being written
	virtual const TCH* get_module() const throw() = 0;

	// return integer id of the log module
	virtual int get_module_id() const throw() = 0;
};

}
