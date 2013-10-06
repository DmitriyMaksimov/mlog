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
#include <fstream>
#include <string>
#include <locale>
#include <stdio.h>

namespace mlog
{

namespace detail
{

// use an approach, which allows us to avoid memory allocations even with file names
template<class TCH>
struct base_filename
{
	typedef TCH char_type;
	typename std::basic_stringstream<TCH> m_strm;

	void prepare_stream()
	{
		if (m_strm.tellp() > 0)
			m_strm.seekp(0);
		m_strm.fill(detail::select_wchars<TCH>::fill);
	}

	const TCH* get_string()
	{
		m_strm.put(select_wchars<TCH>::eol);
		return reinterpret_cast<get_stringbuf_base<TCH>*>(m_strm.rdbuf())->get();
	}

	void get_string(std::basic_string<TCH>& str)
	{
		str.assign(reinterpret_cast<get_stringbuf_base<TCH>*>(m_strm.rdbuf())->get(), m_strm.tellp());
	}
};

} // namespace detail

struct day_rotation
{
	bool operator()(const detail::time_type* ptime, unsigned long)
	{
		using namespace mlog::detail;
		time_type now;
		get_time(&now);
		return get_day(&now) != get_day(ptime) || get_month(&now) != get_month(ptime) || get_year(&now) != get_year(ptime);
	}
};

template<class TCH>
struct day_filename : public detail::base_filename<TCH>
{
	const TCH* operator()(const TCH* prefix, const TCH* suffix, bool)
	{
		detail::base_filename<TCH>::prepare_stream();
		using namespace mlog::detail;
		time_type now;
		get_time(&now);
		static const TCH dash = detail::select_wchars<TCH>::dash;
		detail::base_filename<TCH>::m_strm << prefix << dash << std::setw(4) << get_year(&now) << dash << std::setw(2) <<
			get_month(&now) << dash << std::setw(2) << get_day(&now) << suffix;
		return detail::base_filename<TCH>::get_string();
	}
};

struct month_rotation
{
	bool operator()(const detail::time_type* ptime, unsigned long)
	{
		detail::time_type now;
		detail::get_time(&now);
		return detail::get_month(&now) != detail::get_month(ptime) ||
			detail::get_year(&now) != detail::get_year(ptime);
	}
};

template<class TCH>
struct month_filename : public detail::base_filename<TCH>
{
	const TCH* operator()(const TCH* prefix, const TCH* suffix, bool)
	{
		detail::base_filename<TCH>::prepare_stream();
		using namespace mlog::detail;
		time_type now;
		get_time(&now);
		static const TCH dash = detail::select_wchars<TCH>::dash;
		detail::base_filename<TCH>::m_strm << prefix << dash << std::setw(4) <<
			get_year(&now) << dash << std::setw(2) << get_month(&now) << suffix;
		return detail::base_filename<TCH>::get_string();
	}
};

struct const_rotation
{
	bool operator()(const detail::time_type*, unsigned long)
	{
		return false;
	}
};

template<class TCH>
struct const_filename : public detail::base_filename<TCH>
{
	const TCH* operator()(const TCH* prefix, const TCH* suffix, bool)
	{
		detail::base_filename<TCH>::prepare_stream();
		detail::base_filename<TCH>::m_strm << prefix << suffix;
		return detail::base_filename<TCH>::get_string();
	}
};

struct size_rotation
{
	unsigned long m_max_size;

	size_rotation(unsigned long size) :
		m_max_size(size)
	{
	}

	bool operator()(const detail::time_type*, unsigned long size)
	{
		return m_max_size <= size;
	}
};

template<class TCH>
class rotate_filename : public detail::base_filename<TCH>
{
public:
	rotate_filename(int max_files) :
		m_max_files(max_files)
	{
		if (m_max_files <= 0)
			throw std::logic_error("maximum number of files must be greater than 0");
	}

	const TCH* operator()(const TCH* prefix, const TCH* suffix, bool rotate)
	{
		// rename files to their older versions
		if (rotate)
		{
			for (int i = m_max_files-1; i > 0; i--)
			{
				detail::base_filename<TCH>::prepare_stream();
				if (i == 1)
					detail::base_filename<TCH>::m_strm << prefix << suffix;
				else
					detail::base_filename<TCH>::m_strm << prefix << suffix << detail::select_wchars<TCH>::dot << i-1;
				get_string(m_from);
				detail::base_filename<TCH>::prepare_stream();
				detail::base_filename<TCH>::m_strm << prefix << suffix << detail::select_wchars<TCH>::dot << i;
				get_string(m_to);
				delete_file(m_to.c_str());
				rename_file(m_from.c_str(), m_to.c_str());
			}
		}
		detail::base_filename<TCH>::prepare_stream();
		detail::base_filename<TCH>::m_strm << prefix << suffix;
		return detail::base_filename<TCH>::get_string();
	}

private:
	int m_max_files;
	typename std::basic_string<TCH> m_from;
	typename std::basic_string<TCH> m_to;

	void delete_file(const char* name)
	{
		::remove(name);
	}

	void rename_file(const char* oldname, const char* newname)
	{
		::rename(oldname, newname);
	}

#if !defined(BOOST_NO_INTRINSIC_WCHAR_T) && defined(_WIN32)
	void delete_file(const wchar_t* name)
	{
		::_wremove(name);
	}

	void rename_file(const wchar_t* oldname, const wchar_t* newname)
	{
		::_wrename(oldname, newname);
	}
#endif
};

// define a sink to file, with rotate and naming policy
// rotate and naming policy are two functor objects, which provide
// moment, when file rotation is needed and how to name log files
template<class rotate_policy, class name_policy, class TCH>
class file_sink : public sink<TCH>
{
public:
	typedef typename name_policy::char_type name_type;

	file_sink(const name_type* prefix, const name_type* suffix = detail::select_wchars<name_type>::suffix(), bool allow_buffer = false) :
		m_prefix(prefix),
		m_suffix(suffix),
		m_allow_buffer(allow_buffer)
	{
		open(false);
	}

protected:
	file_sink(unsigned long max_size, int max_files, const name_type* prefix, const name_type* suffix, bool allow_buffer) :
		m_prefix(prefix),
		m_suffix(suffix),
		m_rotate(max_size),
		m_name(max_files),
		m_allow_buffer(allow_buffer)
	{
		open(false);
	}

private:
	detail::time_type m_time;
	std::basic_string<name_type> m_prefix;
	std::basic_string<name_type> m_suffix;
	std::basic_ofstream<TCH> m_file;
	std::locale m_locale;
	rotate_policy m_rotate;
	name_policy m_name;
	bool m_allow_buffer;

	void open(bool rotate)
	{
		detail::get_time(&m_time);
		m_file.clear();
		m_file.open(m_name(m_prefix.c_str(), m_suffix.c_str(), rotate), std::ios::in | std::ios::out | std::ios::ate);
		if (!m_file.is_open())
		{
			m_file.clear();
			m_file.open(m_name(m_prefix.c_str(), m_suffix.c_str(), false), std::ios::out | std::ios::trunc);
		}
		if (m_file.is_open())
			m_file.imbue(m_locale);
	}

	void rotate()
	{
		if (!m_file.is_open())
			open(false);
		if (m_file.is_open() && m_rotate(&m_time, static_cast<unsigned long>(m_file.tellp())))
		{
			m_file.close();
			open(true);
		}
	}

	void write_string(const TCH* str, typename sink<TCH>::pos_type len)
	{
		if (m_file.is_open())
		{
			m_file.write(str, len);
			if (!m_allow_buffer)
				m_file.flush();
		}
	}

	virtual typename sink<TCH>::self_t& imbue(const std::locale& new_locale)
	{
		sink<TCH>::imbue(new_locale);
		m_locale = new_locale;
		if (m_file.is_open())
			m_file.imbue(new_locale);
		return *this;
	}
};

template<class TCH, class TNCH>
class size_file_sink : public file_sink<size_rotation, rotate_filename<TNCH>, TCH>
{
public:
	size_file_sink(unsigned long max_size, int max_files, const TNCH* prefix, const TNCH* suffix = detail::select_wchars<TNCH>::suffix(), bool allow_buffer = false) :
		file_sink<size_rotation, rotate_filename<TNCH>, TCH>::file_sink(max_size, max_files, prefix, suffix, allow_buffer)
	{
	}
};

// day rotation file sink
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef file_sink<day_rotation, day_filename<char>, wchar_t> day_wfile_sink;
#endif
typedef file_sink<day_rotation, day_filename<char>, char> day_afile_sink;
typedef file_sink<day_rotation, day_filename<char>, char> day_cfile_sink;
// month rotation file sink
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef file_sink<month_rotation, month_filename<char>, wchar_t> month_wfile_sink;
#endif
typedef file_sink<month_rotation, month_filename<char>, char> month_afile_sink;
typedef file_sink<month_rotation, month_filename<char>, char> month_cfile_sink;
// constant file, without rotation
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef file_sink<const_rotation, const_filename<char>, wchar_t> const_wfile_sink;
#endif
typedef file_sink<const_rotation, const_filename<char>, char> const_afile_sink;
typedef file_sink<const_rotation, const_filename<char>, char> const_cfile_sink;
// size based rotation with rotating filename
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef size_file_sink<wchar_t, char> size_wfile_sink;
#endif
typedef size_file_sink<char, char> size_afile_sink;
typedef size_file_sink<char, char> size_cfile_sink;

}
