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

#include "log_wchars.h"
#include <sstream>
#include <locale>

namespace mlog
{

namespace detail
{

template<class TCH>
class thread_logger
{
public:
	std::basic_ostringstream<TCH>& get_stream()
	{
		return m_stream;
	}

	void imbue(const std::locale& new_locale)
	{
		m_stream.imbue(new_locale);
	}

	void begin()
	{
		if (m_stream.tellp() > 0)
			m_stream.seekp(0);
		m_stream.fill(detail::select_wchars<TCH>::fill);
		m_stream.flags(std::ios::skipws | std::ios::dec | std::ios::fixed);
		m_stream.precision(2);
	}

private:
	std::basic_ostringstream<TCH> m_stream;
};

} //detail

}
