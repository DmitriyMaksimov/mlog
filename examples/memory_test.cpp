/*
	This example allows to check, how effective iostreams are
	organized. Currently, this is a WIN32 only example, but you
	can change it to provide tests for your operating system.
	The example works only in DEBUG build. It demonstrates that
	under WIN32/VC++ the library doesn't reallocate memory for log
	messages, shorter than previous ones.

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

#ifndef _WIN32
	#error This example is for WIN32 only.
#endif

#include "log_system.h"
#include <memory>

// define log module
std::auto_ptr<mlog::cmodule_logger> g_plogger;

mlog::cmodule_logger& get_log()
{
	return *g_plogger.get();
}

// number of memory allocations
int g_allocations = 0;

#if defined(_DEBUG) && defined(_WIN32)
// memory allocation hook, which simply counts number of memory allocations and reallocations
int alloc_hook(int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char * szFileName, int nLine)
{
	if (nBlockUse == _CRT_BLOCK)
		return 1;
	if (nAllocType == _HOOK_ALLOC || nAllocType == _HOOK_REALLOC)
		g_allocations ++;
	return 1;
}
#endif

int main(int, char*[])
{
#if defined(_DEBUG) && defined(_WIN32)
	// start memory checking facilities
	::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
	::_CrtSetAllocHook(alloc_hook);
#endif
	// run everything in scope
	{
		g_plogger = std::auto_ptr<mlog::cmodule_logger>(new mlog::cmodule_logger());
		// setup for basic log operations
		get_log().add_sink(new mlog::cout_sink);
		get_log().add_sink(new mlog::const_cfile_sink("log_file_name"));
		mlog::cformat format;
		format << std::setw(8) << std::setfill(' ') << std::left << mlog::fp_level_name << ": " <<
			mlog::fp_year << '-' << mlog::fp_month << '-' << mlog::fp_day << ' ' <<
			mlog::fp_hour << ':' << mlog::fp_minute << ':' << mlog::fp_second << ':' << mlog::fp_milli << ' ' <<
			mlog::fp_message << " (" << mlog::fp_file_short << '@' << mlog::fp_line << ')' << std::endl;
		get_log().set_format_all(format);
		get_log().set_level(mlog::dbg);
		std::cout << "Allocations after setup: " << g_allocations << std::endl;
		// log several long strings to consume memory
		for (int i = 0; i < 4; i++)
		{
			LOGC(get_log(), mlog::warn) << "****************" << std::hex << std::showbase << 0x18
				<< "****************" << std::dec << std::fixed << std::noshowbase <<
				std::setprecision(4) << std::setfill(' ') << std::setw(16) << 145.0056 <<
				"************************************************";
			std::cout << "Allocations: " << g_allocations << std::endl;
		}
		// now we can check number of allocations during logging
		for (int i = 0; i < 4; i++)
		{
			LOGC(get_log(), mlog::crit) << "A critical error message";
			LOGC(get_log(), mlog::err) << "An error message";
			LOGC(get_log(), mlog::warn) << "A warning message";
			LOGC(get_log(), mlog::info) << "An informational message";
			LOGC(get_log(), mlog::dbg) << "A debug message";
			LOGC(get_log(), mlog::trace) << "A trace message";
			std::cout << "Allocations: " << g_allocations << std::endl;
		}
		// finally, we can check number of allocations, when we log another long string
		LOGC(get_log(), mlog::warn) << "************************************************" <<
			"************************************************" <<
			"************************************************" <<
			"************************************************" <<
			"************************************************" <<
			"************************************************";
		std::cout << "Allocations after longest line: " << g_allocations << std::endl;
		// delete log module
		g_plogger.reset();
	}
#if defined(_DEBUG) && defined(_WIN32)
	// dump possible memory leaks
	::_CrtDumpMemoryLeaks();
#endif
}
