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

#include "log_module_itf.h"
#include "log_fmt_provider.h"

namespace mlog
{

namespace detail
{

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4512)
#endif

template<class TCH>
class tl_caller
{
private:
	typedef boost::shared_ptr< mlog::detail::thread_logger<TCH> > thread_logger_ptr;

public:
	tl_caller(const module_logger_itf<TCH>& ml, const thread_logger_ptr& tl, unsigned int line, const TCH* file, const TCH* func, int level) throw() :
		m_ml(ml),
		m_tl(tl),
		m_prov(line, file, func, level, ml.get_name(), ml.get_id(), ml.get_tag_provider(), ml.get_level_name_provider())
	{
		try
		{
			m_tl->begin();
		}
		catch(...)
		{
			// prevent possible exceptions from the library to break host program flow
		}
	}

	~tl_caller() throw()
	{
		try
		{
			m_ml.tl_completed(m_tl, m_prov);
		}
		catch(...)
		{
			// prevent possible exceptions from the library to break host program flow
		}
	}

	std::basic_ostringstream<TCH>& get_stream()
	{
		return m_tl->get_stream();
	}

private:
	const module_logger_itf<TCH>& m_ml;
    const thread_logger_ptr& m_tl;
	const inplace_format_provider<TCH> m_prov;
};

#ifdef _WIN32
#pragma warning(pop)
#endif

} // detail

}
