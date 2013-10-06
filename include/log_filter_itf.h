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

#include "log_fmt_itf.h"
#include <ostream>
#include <boost/smart_ptr.hpp>

namespace mlog
{

// class, which represent a single filter for a log sink
template<class TCH>
class filter
{
public:
	filter() :
		m_enabled(true)
	{
	}

	virtual ~filter() { }

	// returns true, if parameters from the format provider may be allowed to the sink, and/or
	// user message is allowed to the sink
	// used to test for log module name, line, file, function, tags, and so on
	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH* log_str, typename std::basic_ostream<TCH>::pos_type log_len) const = 0;

	// use this function to turn filter on and off
	void enable(bool on)
	{
		m_enabled = on;
	}

	// returns true, if the filter is enabled
	// called by the logging framework, before allow() is called
	bool is_enabled() const
	{
		return m_enabled;
	}

private:
	volatile bool m_enabled;
};

namespace detail
{

// NOT filter, which simply negates result of its inner filter
template<class TCH>
class not_filter : public filter<TCH>
{
public:
	not_filter(boost::shared_ptr< filter<TCH> > inner) :
		m_pfilter(inner)
	{
	}

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH* log_str, typename std::basic_ostream<TCH>::pos_type log_len) const
	{
		return !m_pfilter->allow(fmt_prov, log_str, log_len);
	}

private:
	boost::shared_ptr< filter<TCH> > m_pfilter;
};

// OR filter, which checks either of its inner filters
// if left filter allows a log record to pass, then right filter will not be evaluated for the record
template<class TCH>
class or_filter : public filter<TCH>
{
public:
	or_filter(boost::shared_ptr< filter<TCH> > pleft, boost::shared_ptr< filter<TCH> > pright) :
		m_pleft(pleft),
		m_pright(pright)
	{
	}

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH* log_str, typename std::basic_ostream<TCH>::pos_type log_len) const
	{
		return m_pleft->allow(fmt_prov, log_str, log_len) || m_pright->allow(fmt_prov, log_str, log_len);
	}

private:
	boost::shared_ptr< filter<TCH> > m_pleft;
	boost::shared_ptr< filter<TCH> > m_pright;
};

// AND filter, which checks both of its inner filters
// if left filter doesn't allow a log record to pass, then right filter will not be evaluated for the record
template<class TCH>
class and_filter : public filter<TCH>
{
public:
	and_filter(boost::shared_ptr< filter<TCH> > pleft, boost::shared_ptr< filter<TCH> > pright) :
		m_pleft(pleft),
		m_pright(pright)
	{
	}

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH* log_str, typename std::basic_ostream<TCH>::pos_type log_len) const
	{
		return m_pleft->allow(fmt_prov, log_str, log_len) && m_pright->allow(fmt_prov, log_str, log_len);
	}

private:
	boost::shared_ptr< filter<TCH> > m_pleft;
	boost::shared_ptr< filter<TCH> > m_pright;
};

} // namespace detail

// helper functions to create NOT filters
template<class TCH>
boost::shared_ptr< filter<TCH> > not_filter(boost::shared_ptr< filter<TCH> > pfilter)
{
	return boost::shared_ptr< filter<TCH> >(new detail::not_filter<TCH>(pfilter));
}

// helper functions to create OR filters
template<class TCH>
boost::shared_ptr< filter<TCH> > or_filter(boost::shared_ptr< filter<TCH> > pleft, boost::shared_ptr< filter<TCH> > pright)
{
	return boost::shared_ptr< filter<TCH> >(new detail::or_filter<TCH>(pleft, pright));
}

// helper functions to create AND filters
template<class TCH>
boost::shared_ptr< filter<TCH> > and_filter(boost::shared_ptr< filter<TCH> > pleft, boost::shared_ptr< filter<TCH> > pright)
{
	return boost::shared_ptr< filter<TCH> >(new detail::and_filter<TCH>(pleft, pright));
}

} // namespace mlog

template<class TCH>
boost::shared_ptr< mlog::filter<TCH> > operator~(boost::shared_ptr< mlog::filter<TCH> > pfilter)
{
	return boost::shared_ptr< mlog::filter<TCH> >(new mlog::detail::not_filter<TCH>(pfilter));
}

template<class TCH>
boost::shared_ptr< mlog::filter<TCH> > operator||(boost::shared_ptr< mlog::filter<TCH> > pleft, boost::shared_ptr< mlog::filter<TCH> > pright)
{
	return boost::shared_ptr< mlog::filter<TCH> >(new mlog::detail::or_filter<TCH>(pleft, pright));
}

template<class TCH>
boost::shared_ptr< mlog::filter<TCH> > operator|(boost::shared_ptr< mlog::filter<TCH> > pleft, boost::shared_ptr< mlog::filter<TCH> > pright)
{
	return boost::shared_ptr< mlog::filter<TCH> >(new mlog::detail::or_filter<TCH>(pleft, pright));
}

template<class TCH>
boost::shared_ptr< mlog::filter<TCH> > operator&&(boost::shared_ptr< mlog::filter<TCH> > pleft, boost::shared_ptr< mlog::filter<TCH> > pright)
{
	return boost::shared_ptr< mlog::filter<TCH> >(new mlog::detail::and_filter<TCH>(pleft, pright));
}

template<class TCH>
boost::shared_ptr< mlog::filter<TCH> > operator&(boost::shared_ptr< mlog::filter<TCH> > pleft, boost::shared_ptr< mlog::filter<TCH> > pright)
{
	return boost::shared_ptr< mlog::filter<TCH> >(new mlog::detail::and_filter<TCH>(pleft, pright));
}
