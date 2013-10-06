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

#include "log_fmt.h"
#include "log_filter_itf.h"
#include <boost/smart_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <memory>
#include <vector>

namespace mlog
{

// this class defines a log sink, which may be used to save log messages to a destination
// each sink has its own locale, format and filters
// the class is not thread safe, but it stores an exclusive lock to allow creation of sink pipeline,
// when multiple threads may write messages to each sink, one thread to one sink at any time
// first thread, which locks first sink in the pipeline, will be the first one to finish writing
template<class TCH>
class sink
{
protected:
	typedef typename std::basic_ostream<TCH>::pos_type pos_type;

public:
	// type of this sink
	typedef sink<TCH> self_t;

	// type of filter, filters are used to allow or not allow log records to be written to destination
	typedef boost::shared_ptr< filter<TCH> > filter_ptr;

	// constructor/destructor
	sink() : m_enabled(true) { }
	virtual ~sink() { }

	// sets format for this sink
	// this function is a setup function, and thus it is not thread safe
	self_t& set_format(const format<TCH>& new_format)
	{
		m_pformat = boost::shared_ptr< format<TCH> >(new format<TCH>(new_format));
		return *this;
	}

	// adds filter to this sink
	// this function is a setup function, and thus it is not thread safe
	self_t& add_filter(filter_ptr pfilter)
	{
		m_filters.push_back(pfilter);
		return *this;
	}

	// may be overwritten to provide custom functionality
	virtual void log_string(const TCH* log_str, pos_type log_len, const format_provider<TCH>& fmt_prov)
	{
		// filter the log record
		for (filter_iter i = m_filters.begin(); i != m_filters.end(); i++)
			if ((*i)->is_enabled() && !(*i)->allow(fmt_prov, log_str, log_len))
				return;
		// write prefix
		if (m_pformat)
		{
			m_pformat->get_prefix(fmt_prov, m_prefix_strm);
			write_format(m_prefix_strm);
		}
		// write the log message
		if (log_str != NULL)
			write_string(log_str, log_len);
		// write suffix
		if (m_pformat)
		{
			m_pformat->get_suffix(fmt_prov, m_suffix_strm);
			write_format(m_suffix_strm);
		}
	}

	// must override to support imbue for some sinks
	// this function is a setup function, and thus it is not thread safe
	virtual self_t& imbue(const std::locale& new_locale)
	{
		m_prefix_strm.imbue(new_locale);
		m_suffix_strm.imbue(new_locale);
		return *this;
	}

	// each log sink must override this function to provide data destination rotation
	// like file time or size based rotation, log cleanup and so on
	virtual void rotate()
	{
		// default implementation doesn't rotate
	}

	// returns an exclusive lock for this sink
	// any number of threads may write log messages to the sink pipeline simultaneously
	boost::mutex* get_mutex()
	{
		return &m_mutex;
	}

	// enable/disable this sink
	void enable(bool on)
	{
		m_enabled = on;
	}

	// returns true, if the sink is enabled
	// called by the logging framework, before get_mutex() is called and locked
	bool is_enabled() const
	{
		return m_enabled;
	}

protected:
	// implementation must not throw
	virtual void write_string(const TCH* str, pos_type len) = 0;

	// a set of filters, applied to this sink
	typedef typename std::vector<filter_ptr>::const_iterator filter_iter;
	std::vector<filter_ptr> m_filters;

private:
	// each sink has its own format, but one format may be shared between several sinks
	boost::shared_ptr< format<TCH> > m_pformat;
	// string streams, which used to create log prefix and log suffix
	std::basic_ostringstream<TCH> m_prefix_strm;
	std::basic_ostringstream<TCH> m_suffix_strm;
	// mutex for exclusive access to the sink
	boost::mutex m_mutex;
	// sinks may be disabled
	volatile bool m_enabled;

	void write_format(std::basic_ostringstream<TCH>& strm)
	{
		detail::get_stringbuf_base<TCH>* pgetbase = reinterpret_cast<detail::get_stringbuf_base<TCH>*>(strm.rdbuf());
		const TCH* str = pgetbase->get();
		if (str != NULL)
			write_string(str, strm.tellp());
		if (strm.tellp() > 0)
			strm.seekp(0);
	}
};

// defines a null sink, which doesn't write anywhere
template<class TCH>
class null_sink : public sink<TCH>
{
	void write_string(const TCH*, typename sink<TCH>::pos_type)
	{
	}
};

typedef null_sink<char> cnull_sink;
typedef null_sink<char> anull_sink;
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef null_sink<wchar_t> wnull_sink;
#endif

}
