/*
	This example allows to check, how effective mlog system works.
	Currently, this is a WIN32 only example, but you can change it
	to provide tests for your operating system. The example demonstrates
	that under WIN32/VC++ the library is working about 3-6 times slower
	than direct writing to ofstream, and 4-10 times slower than calling
	fprintf(). During tests on my computer, 2.4ghz core 2 duo, each call
	under debug takes at most 0.100ms, while under release it is 0.030ms,
	which means if one log record of such type is written per 1.000ms of
	program time, the program is slowed by mlog by 10% in debug and by 3%
	in release.

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

#ifndef _WIN32
	#error This example is for WIN32 only.
#else
	#include <windows.h>
	#pragma comment(lib, "winmm.lib")
	#pragma warning(disable:4996)
#endif

#ifdef _WIN32
unsigned int get_time()
{
	return static_cast<unsigned int>(::timeGetTime());
}

void sleep(unsigned int millis)
{
	::Sleep(static_cast<DWORD>(millis));
}
#endif

#define MSG_COUNT 10000

int main(int, char*[])
{
#ifdef _WIN32
	// setup high resolution timer
	::timeBeginPeriod(1);
#endif
	// setup logging with file sink, create a very simple format, which we will
	// repeat for comparing mlog with direct output to ofstream
	mlog::cmodule_logger logger;
	logger.add_sink(new mlog::const_cfile_sink("log_file_name"));
	logger.set_format_all(mlog::cformat() <<
		mlog::fp_year << '-' << mlog::fp_month << '-' << mlog::fp_day << ": " << mlog::fp_message << std::endl);
	// show that we can measure time precisely
	unsigned int nbefore = get_time();
	sleep(1);
	unsigned int nafter = get_time();
	// log several warnings and measure time
	unsigned int lstart = get_time();
	for (int i = 0; i < MSG_COUNT; i++)
		LOGC(logger, mlog::warn) << "A warning message: " << 55;
	unsigned int lend = get_time();
	// create ofstream to measure time without library overhead
	// and write messages similar to those produced by mlog
	std::ofstream file("ofstream_file.log", std::ios::app);
	std::cout.fill('0');
	file.fill('0');
	unsigned int ofstart = get_time();
	for (int i = 0; i < MSG_COUNT; i++)
	{
		mlog::detail::time_type time;
		mlog::detail::get_time(&time);
		file << std::setw(4) << mlog::detail::get_year(&time) << '-' << std::setw(2) <<
			mlog::detail::get_month(&time) << '-' << std::setw(2) << mlog::detail::get_day(&time) << ": " <<
			"A warning message: " << 55 << std::endl;
		file.flush();
	}
	unsigned int ofend = get_time();
	// create FILE and test its timings
	FILE* pfile = ::fopen("pfile_file.log", "a+");
	unsigned int fstart = get_time();
	for (int i = 0; i < MSG_COUNT; i++)
	{
		mlog::detail::time_type time;
		mlog::detail::get_time(&time);
		::fprintf(pfile, "%04d-%02d-%02d: A warning message: %d\n", mlog::detail::get_year(&time),
			mlog::detail::get_month(&time), mlog::detail::get_day(&time), 55);
		::fflush(pfile);
	}
	unsigned int fend = get_time();
	::fclose(pfile);
#ifdef _WIN32
	::timeEndPeriod(1);
#endif
	// print the results
	std::cout << "Time for 1 ms: " << nafter-nbefore << std::endl;
	std::cout << "Time for calling log functions: " << lend-lstart << ", " << ((double)lend-lstart)/MSG_COUNT << "ms per message" << std::endl;
	std::cout << "Time for calling iostream functions: " << ofend-ofstart << ", " << ((double)ofend-ofstart)/MSG_COUNT << "ms per message" << std::endl;
	std::cout << "Time for calling C functions: " << fend-fstart << ", " << ((double)fend-fstart)/MSG_COUNT << "ms per message" << std::endl;
	return 0;
}
