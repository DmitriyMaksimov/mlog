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

#include "log_sink_stdout.h"
#include "log_sink_file.h"
#ifdef _WIN32
	#include "log_sink_msdebug.h"
#endif
#include "log_tag_provider.h"
#include "log_module.h"
#include "log_filters.h"
//#include "log_setup.h"

#define LOG_MAKE_WCHAR2(str) L ## str
#define LOG_MAKE_WCHAR(str) LOG_MAKE_WCHAR2(str)

// example log formats
#define LOG_DDMMYYYYHHMMSSL_W mlog::format<wchar_t>() << L'[' << mlog::fp_day << L'.' << mlog::fp_month << L'.' << mlog::fp_year << L' ' <<	\
			mlog::fp_hour << L':' << mlog::fp_minute << L':' << mlog::fp_second << L"] [" << std::setfill(L' ') << std::left << std::setw(8) << mlog::fp_level_name << L"] " << \
			mlog::fp_message << \
			L" (" << mlog::fp_file_short << L':' << mlog::fp_line << L')' << mlog::fp_newline

#define LOG_DDMMYYYYHHMMSSMML_W mlog::format<wchar_t>() << L'[' << mlog::fp_day << L'.' << mlog::fp_month << L'.' << mlog::fp_year << L' ' <<	\
	mlog::fp_hour << L':' << mlog::fp_minute << L':' << mlog::fp_second << L',' << mlog::fp_milli << L"] [" << std::setfill(L' ') << std::left << std::setw(8) << mlog::fp_level_name << L"] " << \
		mlog::fp_message << \
		L" (" << mlog::fp_file_short << L':' << mlog::fp_line << L')' << mlog::fp_newline

#define LOG_DDMMYYYYHHMMSSL_A mlog::format<char>() << '[' << mlog::fp_day << '.' << mlog::fp_month << '.' << mlog::fp_year << ' ' <<	\
	mlog::fp_hour << ':' << mlog::fp_minute << ':' << mlog::fp_second << "] [" << std::setfill(' ') << std::left << std::setw(8) << mlog::fp_level_name << "] " << \
			mlog::fp_message << \
			" (" << mlog::fp_file_short << ':' << mlog::fp_line << ')' << mlog::fp_newline

#define LOG_DDMMYYYYHHMMSSMML_A mlog::format<char>() << '[' << mlog::fp_day << '.' << mlog::fp_month << '.' << mlog::fp_year << ' ' <<	\
	mlog::fp_hour << ':' << mlog::fp_minute << ':' << mlog::fp_second << ',' << mlog::fp_milli << "] [" << std::setfill(' ') << std::left << std::setw(8) << mlog::fp_level_name << "] " << \
		mlog::fp_message << \
		" (" << mlog::fp_file_short << ':' << mlog::fp_line << ')' << mlog::fp_newline

#include "log.h"
