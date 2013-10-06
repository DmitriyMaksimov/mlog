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

#include "log_sink.h"
#include "log_wchars.h"
#include <iostream>

namespace mlog
{

#if !defined(__CYGWIN__) && !defined(BOOST_NO_INTRINSIC_WCHAR_T)

#define LOG_HAS_WCOUT_SINK 1

class wcout_sink : public sink<wchar_t>
{
	void write_string(const wchar_t* str, pos_type len)
	{
		std::wcout.write(str, static_cast<std::streamsize>(len));
	}

	virtual sink<wchar_t>::self_t& imbue(const std::locale& new_locale)
	{
		sink<wchar_t>::imbue(new_locale);
		std::wcout.imbue(new_locale);
		return *this;
	}
};

class wcerr_sink : public sink<wchar_t>
{
	void write_string(const wchar_t* str, pos_type len)
	{
		std::wcerr.write(str, static_cast<std::streamsize>(len));
	}

	virtual sink<wchar_t>::self_t& imbue(const std::locale& new_locale)
	{
		sink<wchar_t>::imbue(new_locale);
		std::wcerr.imbue(new_locale);
		return *this;
	}
};

#endif

class cout_sink : public sink<char>
{
	void write_string(const char* str, sink<char>::pos_type len)
	{
		std::cout.write(str, static_cast<std::streamsize>(len));
	}

	virtual sink<char>::self_t& imbue(const std::locale& new_locale)
	{
		sink<char>::imbue(new_locale);
		std::cout.imbue(new_locale);
		return *this;
	}
};

class cerr_sink : public sink<char>
{
	void write_string(const char* str, sink<char>::pos_type len)
	{
		std::cerr.write(str, static_cast<std::streamsize>(len));
	}

	virtual sink<char>::self_t& imbue(const std::locale& new_locale)
	{
		sink<char>::imbue(new_locale);
		std::cerr.imbue(new_locale);
		return *this;
	}
};

}
