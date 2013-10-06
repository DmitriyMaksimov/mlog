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

#include "log_caller.h"
#include "log_level.h"
//#include "log_setup_facade.h"

#if !defined(_WIN32) || !defined(__FUNCTION__)
	#define __FUNCTION__ "WIN32ONLY"
#endif

#define LOGW(LOGGER, LEVEL)	\
	if (LOGGER.is_enabled() && LOGGER.get_level() < LEVEL)	{ }	\
	else	\
		mlog::detail::tl_caller<wchar_t>(LOGGER, LOGGER.get_logger(), __LINE__, LOG_MAKE_WCHAR(__FILE__), LOG_MAKE_WCHAR(__FUNCTION__), LEVEL).get_stream()

#define LOGA(LOGGER, LEVEL)	\
	if (LOGGER.is_enabled() && LOGGER.get_level() < LEVEL)	{ }	\
	else	\
		mlog::detail::tl_caller<char>(LOGGER, LOGGER.get_logger(), __LINE__, __FILE__, __FUNCTION__, LEVEL).get_stream()

#define LOGC LOGA

#ifdef BOOST_NO_INTRINSIC_WCHAR_T
	#define LOG LOGA
#else
	#define LOG LOGW
#endif
