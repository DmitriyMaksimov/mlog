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

#ifdef _WIN32

#include "log_sink.h"
#include <string>

namespace mlog
{

template<class TCH>
class ms_debug_sink : public sink<TCH>
{
	std::basic_string<TCH> m_str_dbg;

	void write_string(const TCH* str, pos_type len)
	{
		if (::IsDebuggerPresent())
		{
			m_str_dbg.resize(0);
			m_str_dbg.insert(0, str, len);
			m_str_dbg.insert(m_str_dbg.end(), detail::select_wchars<TCH>::eol);
			inner_write(m_str_dbg.c_str());
		}
	}

	void inner_write(const wchar_t* str)
	{
		::OutputDebugStringW(str);
	}

	void inner_write(const char* str)
	{
		::OutputDebugStringA(str);
	}
};

}

#endif
