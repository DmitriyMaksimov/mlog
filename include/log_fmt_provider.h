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

#include "log_level_itf.h"
#include "log_tag_itf.h"
#include "log_fmt_itf.h"
#include "log_wchars.h"

#ifdef _WIN32
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

namespace mlog
{

namespace detail
{

template<class TCH>
class inplace_format_provider : public format_provider<TCH>
{
public:
	inline inplace_format_provider(unsigned int line, const TCH* file, const TCH* func, int level, const TCH* module, int module_id, const tag_provider<TCH>* ptags, const level_name_provider<TCH>* plevels) throw() :
		m_line(line),
		m_file(file),
		m_function(func),
		m_level(level),
		m_module(module),
		m_module_id(module_id),
		m_ptags(ptags),
		m_plevels(plevels)
	{
		try
		{
			detail::get_time(&m_time);
		}
		catch(...)
		{
			// prevent possible exceptions from the library to break host program flow
		}
	}

	unsigned int get_line() const throw()
	{
		return m_line;
	}

	const TCH* get_file(bool short_name) const throw()
	{
		if (!short_name)
			return m_file;
		else
		{
			#ifdef _WIN32
				__int64
			#else
				int
			#endif
			slash_pos = -1;
			for (const TCH* pc = m_file; *pc != '\0'; pc++)
			{
				#ifdef _WIN32
					if (*pc == '\\')
				#else
					if (*pc == '/')
				#endif
						slash_pos = pc - m_file;
			}
			return m_file + slash_pos + 1;
		}
	}

	const TCH* get_function() const throw()
	{
		return m_function;
	}

	int get_level() const throw()
	{
		return m_level;
	}

	const TCH* get_level_name() const throw()
	{
		return m_plevels == NULL ? select_wchars<TCH>::empty() : m_plevels->level_name(m_level);
	}

	const detail::time_type* get_time() const throw()
	{
		return &m_time;
	}

	const TCH* get_tag(const TCH* name) const
	{
		return m_ptags->get_tag(name);
	}

	const TCH* get_tag(int id) const
	{
		return m_ptags->get_tag(id);
	}

	boost::thread::id get_thread() const throw()
	{
		return boost::this_thread::get_id();
	}

	int get_pid() const throw()
	{
		#ifdef _WIN32
			return ::_getpid();
		#else
			return ::getpid();
		#endif
	}

	const TCH* get_module() const throw()
	{
		return m_module;
	}

	int get_module_id() const throw()
	{
		return m_module_id;
	}

private:
	unsigned int m_line;
	const TCH* m_file;
	const TCH* m_function;
    int m_level;
	const TCH* m_module;
	int m_module_id;
	detail::time_type m_time;
	const tag_provider<TCH>* m_ptags;
	const level_name_provider<TCH>* m_plevels;
};

} // detail

}
