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

#include <boost/config.hpp>
#include <sstream>

namespace mlog
{

namespace detail
{

template<class TCH> struct select_wchars { };

template<> struct select_wchars<char>
{
	static const char dot = '.';
	static const char dash = '-';
	static const char fill = '0';
	static const char eol = '\0';
	static const char* empty() { return ""; };
	static const char* main() { return "MLOG_LOGGING_SYSTEM"; };
	static const char* suffix() { return ".log"; };
};

#ifndef BOOST_NO_INTRINSIC_WCHAR_T
template<> struct select_wchars<wchar_t>
{
	static const wchar_t dot = L'.';
	static const wchar_t dash = L'-';
	static const wchar_t fill = L'0';
	static const char eol = L'\0';
	static const wchar_t* empty() { return L""; };
	static const wchar_t* main() { return L"MLOG_LOGGING_SYSTEM"; };
	static const wchar_t* suffix() { return L".log"; };
};
#endif

template<class TCH>
struct get_stringbuf_base : public std::basic_stringbuf<TCH>
{
};

template<>
struct get_stringbuf_base<char> : public std::basic_stringbuf<char>
{
	const char* get() const { return pbase(); }
};

#ifndef BOOST_NO_INTRINSIC_WCHAR_T
template<>
struct get_stringbuf_base<wchar_t> : public std::basic_stringbuf<wchar_t>
{
	const wchar_t* get() const { return pbase(); }
};
#endif

} // detail

}
