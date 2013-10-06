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

#include <time.h>
#ifndef _WIN32
	#include <sys/time.h>
#else
	#include <sys/timeb.h>
#endif

namespace mlog
{

namespace detail
{

#ifdef _WIN32

struct time_union
{
	_timeb time;
	struct tm local_time;
};

typedef time_union time_type;

inline void get_time(time_type* ptime)
{
	::_ftime64_s(&ptime->time);
	::_localtime64_s(&ptime->local_time, &ptime->time.time);
}

inline unsigned int get_millis(const time_type* ptime)
{
	return ptime->time.millitm;
}

#else

struct time_union
{
	unsigned int millis;
	struct tm local_time;
};

typedef time_union time_type;

inline void get_time(time_type* ptime)
{
	timeval tv;
	::gettimeofday(&tv, NULL);
	ptime->millis = tv.tv_usec / 1000;
	time_t time_sec = tv.tv_sec;
	::localtime_r(&time_sec, &ptime->local_time);
}

inline unsigned int get_millis(const time_type* ptime)
{
	return ptime->millis;
}

#endif

inline unsigned int get_day(const time_type* ptime)
{
	return ptime->local_time.tm_mday;
}

inline unsigned int get_month(const time_type* ptime)
{
	return ptime->local_time.tm_mon + 1;
}

inline unsigned int get_year(const time_type* ptime)
{
	return ptime->local_time.tm_year + 1900;
}

inline unsigned int get_hour(const time_type* ptime)
{
	return ptime->local_time.tm_hour;
}

inline unsigned int get_minute(const time_type* ptime)
{
	return ptime->local_time.tm_min;
}

inline unsigned int get_second(const time_type* ptime)
{
	return ptime->local_time.tm_sec;
}

}

}
