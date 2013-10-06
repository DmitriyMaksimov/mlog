/*
	This example demonstrates how to use unicode logs. Note that unicode
	characters are in russian, CP-1251.

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

#include "log_system.h"
#if !defined(_WIN32) && !defined(__CYGWIN__)
	#include <locale.h>
#endif

int main(int, char*[])
{
	// create a logger module
	mlog::wmodule_logger logger;
#ifdef LOG_HAS_WCOUT_SINK
	// add a sink, which writes messages to stdout
	logger.add_sink(new mlog::wcout_sink);
#endif
	// add a sink, which writes messages to a file
	logger.add_sink(new mlog::const_wfile_sink("log_file_name"));
	// create format, which is a combination of several log parts and character data
	mlog::wformat format;
	format << mlog::fp_level_name << L": " <<
		mlog::fp_year << L'-' << mlog::fp_month << L'-' << mlog::fp_day << L' ' <<
		mlog::fp_hour << L':' << mlog::fp_minute << L':' << mlog::fp_second << L':' << mlog::fp_milli << L' ' <<
		mlog::fp_message <<
		L" (файл: " << mlog::fp_file_short << L" строка: " << mlog::fp_line << L')' << mlog::fp_newline;
	// set format on each sink
	logger.set_format_all(format);
	// we must imbue logger module so that iostreams can convert unicode characters
	// we will write output as CP-866
	try
	{
#ifdef _WIN32
		logger.imbue_all(std::locale("rus_Russia.866"));
#else
	#if !defined(__CYGWIN__)
		::setlocale(LC_ALL, "ru_RU.UTF-8");
		logger.imbue_all(std::locale("ru_RU.UTF-8"));
	#endif
#endif
	}
	catch(std::exception& e)
	{
		std::cout << "failed to imbue: " << e.what() << std::endl;
		return -1;
	}
	// set log level to debug, messages that are above this level are not logged
	// (only trace messages are not logged in this case)
	logger.set_level(mlog::dbg);
	// log messages
	LOGW(logger, mlog::crit) << L"критическая ошибка";
	LOGW(logger, mlog::err) << L"ошибка";
	LOGW(logger, mlog::warn) << L"предупреждение";
	LOGW(logger, mlog::info) << L"информация";
	LOGW(logger, mlog::dbg) << L"дебаг";
	LOGW(logger, mlog::trace) << L"спамм";
	return 0;
}
