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
#include "log_level_itf.h"

namespace mlog
{

enum level
{
	none = 0,
	crit = 1,
	err = 2,
	warn = 3,
	info = 4,
	dbg = 5,
	trace = 6
};

namespace detail
{

template<class TCH>
struct default_level_name
{
	static const TCH* get(int n);
};

#ifndef BOOST_NO_INTRINSIC_WCHAR_T
template<>
struct default_level_name<wchar_t>
{
	static const wchar_t* get(int n)
	{
		switch(n)
		{
		case none: return L"NONE";
		case trace: return L"TRACE";
		case dbg: return L"DEBUG";
		case info: return L"INFO";
		case warn: return L"WARN";
		case err: return L"ERROR";
		case crit: return L"CRITICAL";
		default: return L"UNKNOWN";
		}
	}
};
#endif

template<>
struct default_level_name<char>
{
	static const char* get(int n)
	{
		switch(n)
		{
		case none: return "NONE";
		case trace: return "TRACE";
		case dbg: return "DEBUG";
		case info: return "INFO";
		case warn: return "WARN";
		case err: return "ERROR";
		case crit: return "CRITICAL";
		default: return "UNKNOWN";
		}
	}
};

template<class TCH>
struct default_level_name_provider : public level_name_provider<TCH>
{
	const TCH* level_name(int n) const
	{
		return default_level_name<TCH>::get(n);
	}
};

} // detail

}
