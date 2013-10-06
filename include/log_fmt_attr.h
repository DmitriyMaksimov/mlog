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

#include "log_fmt_provider.h"
#include <iomanip>
#include <sstream>
#include <boost/smart_ptr.hpp>

namespace mlog
{

// different log parts, may be used as arguments to log format
enum log_part
{
	// full file name
	fp_file,
	// short file name, without a path
	fp_file_short,
	fp_short = fp_file_short,
	// line, on which log message appears
	fp_line,
	// full qualified function name, supported only on WIN32
	fp_func,
	// date and time: day as two decimal numbers 01-31
	fp_day,
	// date and time: month as two decimal numbers 01-12
	fp_month,
	// date and time: year as four decimal numbers 2008
	fp_year,
	// date and time: hour as two decimal numbers 00-23
	fp_hour,
	// date and time: minutes as two decimal numbers 00-59
	fp_minute,
	// date and time: seconds as two decimal numbers 00-59
	fp_second,
	// date and time: milliseconds as three decimal numbers 000-999
	fp_milli,
	// log level as an integer value
	fp_level,
	// log level name
	fp_level_name,
	// newline
	fp_newline,
	// id of the calling thread
	fp_thread,
	// id of the calling process
	fp_pid,
	// name of the logging module, which is used to log messages
	fp_module,
	// integer id of the logging module, which is used to log messages
	fp_module_id,
	// log message, only one message allowed per log format
	fp_message
};

namespace detail
{

template<class TCH>
struct attr
{
	virtual ~attr() throw() { }

	// write data, which corresponds to this attribute, to the provided output stream
	virtual void apply(const format_provider<TCH>& fmt_prov, std::basic_ostringstream<TCH>& strm) const = 0;

	// returns true, when this attribute is a log message
	// used to simplify creation of log format
	virtual bool is_message() const throw() { return false; }
};

#define LOG_ATTR_PTR(TCH) boost::shared_ptr< const mlog::detail::attr<TCH> >

// defines attribute for a log part
template<class TCH>
class part_attr : public attr<TCH>
{
public:
	part_attr(log_part part) :
		m_part(part)
	{
	}

	void apply(const format_provider<TCH>& fmt_prov, std::basic_ostringstream<TCH>& strm) const
	{
		using std::setw;
		// reset flags for date and time
		if (static_cast<int>(m_part) >= fp_day && static_cast<int>(m_part) <= fp_milli)
		{
			strm.fill(detail::select_wchars<TCH>::fill);
			strm.flags(std::ios::dec);
		}
		// output part
		switch (m_part)
		{
		case fp_module_id:	strm << fmt_prov.get_module_id();							break;
		case fp_module:		strm << fmt_prov.get_module();								break;
		case fp_thread:		strm << fmt_prov.get_thread();								break;
		case fp_pid:		strm << fmt_prov.get_pid();									break;
		case fp_file:		strm << fmt_prov.get_file(false);							break;
		case fp_line:		strm << fmt_prov.get_line();								break;
		case fp_func:		strm << fmt_prov.get_function();							break;
		case fp_day:		strm << setw(2) << detail::get_day(fmt_prov.get_time());	break;
		case fp_month:		strm << setw(2) << detail::get_month(fmt_prov.get_time());	break;
		case fp_year:		strm << setw(2) << detail::get_year(fmt_prov.get_time()) / 100 << setw(2) << detail::get_year(fmt_prov.get_time()) % 100; break;
		case fp_hour:		strm << setw(2) << detail::get_hour(fmt_prov.get_time());	break;
		case fp_minute:		strm << setw(2) << detail::get_minute(fmt_prov.get_time());	break;
		case fp_second:		strm << setw(2) << detail::get_second(fmt_prov.get_time());	break;
		case fp_milli:		strm << setw(3) << detail::get_millis(fmt_prov.get_time());	break;
		case fp_level:		strm << fmt_prov.get_level();								break;
		case fp_level_name:	strm << fmt_prov.get_level_name();							break;
		case fp_newline:	strm << std::endl;											break;
		case fp_file_short:	strm << fmt_prov.get_file(true);							break;
		}
	}

	bool is_message() const throw()
	{
		return m_part == fp_message;
	}

private:
	log_part m_part;

	static const std::locale& get_classic_loc()
	{
		return std::locale::classic();
	}
};

// defines a "tag" attribute
// the tag value will be inserted into log message
template<class TCH, class IDT>
class tag_attr : public attr<TCH>
{
public:
	tag_attr(const IDT* name) :
		m_name(name)
	{
	}

	void apply(const format_provider<TCH>& fmt_prov, std::basic_ostringstream<TCH>& strm) const
	{
		strm << fmt_prov.get_tag(m_name);
	}

private:
	const IDT* m_name;
};

template<class TCH>
class tag_attr<TCH, int> : public attr<TCH>
{
public:
	tag_attr(int id) :
		m_id(id)
	{
	}

	void apply(const format_provider<TCH>& fmt_prov, std::basic_ostringstream<TCH>& strm) const
	{
		strm << fmt_prov.get_tag(m_id);
	}

private:
	int m_id;
};

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4512)
#endif

// defines attribute for a simple type
template<class TCH, class TVAL>
class type_attr : public attr<TCH>
{
public:
	type_attr(TVAL val) :
		m_val(val)
	{
	}

	void apply(const format_provider<TCH>&, std::basic_ostringstream<TCH>& strm) const
	{
		strm << m_val;
	}

private:
	TVAL m_val;
};

#ifdef _WIN32
#pragma warning(pop)
#endif

// helper structure to declare different types, which put into log format via operator<<
template<class TCH, class TVAL>
struct format_type
{
	static inline LOG_ATTR_PTR(TCH) apply(TVAL val)
	{
		return LOG_ATTR_PTR(TCH)(new type_attr<TCH, TVAL>(val));
	}
};

// specification for log_part enum
template<class TCH>
struct format_type<TCH, log_part>
{
	static inline LOG_ATTR_PTR(TCH) apply(log_part part)
	{
		return LOG_ATTR_PTR(TCH)(new part_attr<TCH>(part));
	}
};

} // detail

// helper function to define a named tag
template<class TCH>
LOG_ATTR_PTR(TCH) tag(const TCH* name)
{
	return LOG_ATTR_PTR(TCH)(new detail::tag_attr<TCH, TCH>(name));
}

// helper function to define a tag, identified by an integer
template<class TCH>
LOG_ATTR_PTR(TCH) tag(int id)
{
	return LOG_ATTR_PTR(TCH)(new detail::tag_attr<TCH, int>(id));
}

}
