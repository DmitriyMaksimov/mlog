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

#include "log_tag_itf.h"
#include "log_wchars.h"
#include <map>
#ifdef _WIN32
	#pragma warning(push)
	#pragma warning(disable:4244)
	#pragma warning(disable:4512)
#endif
#include <boost/thread/shared_mutex.hpp>
#ifdef _WIN32
	#pragma warning(pop)
#endif

namespace mlog
{

namespace detail
{

// default thread safe provider for getting and setting tags
template<class TCH>
class default_tag_provider : public tag_provider<TCH>
{
public:
	~default_tag_provider() throw() { }

	const TCH* get_tag(const TCH* name) const
	{
		boost::shared_lock<boost::shared_mutex> lock(m_mutex);
		s_citer_t i = m_sdata.find(name);
		return i == m_sdata.end() ? select_wchars<TCH>::empty() : i->second.c_str();
	}

	const TCH* get_tag(int id) const
	{
		boost::shared_lock<boost::shared_mutex> lock(m_mutex);
		i_citer_t i = m_idata.find(id);
		return i == m_idata.end() ? select_wchars<TCH>::empty() : i->second.c_str();
	}

	void set_tag(const TCH* name, const TCH* value)
	{
		boost::unique_lock<boost::shared_mutex> lock(m_mutex);
		m_sdata[name] = value;
	}

	void set_tag(int id, const TCH* value)
	{
		boost::unique_lock<boost::shared_mutex> lock(m_mutex);
		m_idata[id] = value;
	}

private:
	typedef std::basic_string<TCH> str_t;
	std::map<str_t, str_t> m_sdata;
	typedef typename std::map<str_t, str_t>::const_iterator s_citer_t;
	std::map<int, str_t> m_idata;
	typedef typename std::map<int, str_t>::const_iterator i_citer_t;
	mutable boost::shared_mutex m_mutex;
};

} // detail

}
