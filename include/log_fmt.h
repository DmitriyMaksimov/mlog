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

#include "log_fmt_attr.h"
#include "log_wchars.h"
#include <stdexcept>
#include <vector>

namespace mlog
{

// this is a set of format rules
// each sink may have its own format
template<class TCH>
class format
{
public:
	format() :
		m_prefix_complete(false)
	{
	}

	void get_prefix(const format_provider<TCH>& fmt_prov, std::basic_ostringstream<TCH>& strm) const
	{
		get_string(fmt_prov, strm, m_prefix);
	}

	void get_suffix(const format_provider<TCH>& fmt_prov, std::basic_ostringstream<TCH>& strm) const
	{
		get_string(fmt_prov, strm, m_suffix);
	}

	format& operator<<(LOG_ATTR_PTR(TCH) attr)
	{
		if (attr->is_message())
		{
			if (m_prefix_complete)
				throw std::runtime_error("duplicated message attribute");
			m_prefix_complete = true;
		}
		else if (m_prefix_complete)
			m_suffix.push_back(attr);
		else
			m_prefix.push_back(attr);
		return *this;
	}

	template<class T>
	format& operator<<(T val)
	{
		return operator<<(detail::format_type<TCH, T>::apply(val));
	}

	typedef std::basic_ostream<TCH>&(*manip_t)(std::basic_ostream<TCH>&);

	format& operator<<(manip_t val)
	{
		return operator<<(detail::format_type<TCH, manip_t>::apply(val));
	}

private:
	typedef LOG_ATTR_PTR(TCH) ptr_type;
	std::vector<ptr_type> m_prefix;
	std::vector<ptr_type> m_suffix;
	typedef typename std::vector<ptr_type>::const_iterator iter_type;
	bool m_prefix_complete;

	void get_string(const format_provider<TCH>& fmt_prov, std::basic_ostringstream<TCH>& strm, const std::vector<ptr_type>& fmt) const
	{
		for (iter_type i = fmt.begin(); i != fmt.end(); i++)
			i->get()->apply(fmt_prov, strm);
	}
};

#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef format<wchar_t> wformat;
#endif
typedef format<char> aformat;
typedef format<char> cformat;

}
