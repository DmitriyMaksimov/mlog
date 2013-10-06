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

#include "log_thread_logger.h"
#include "log_level_itf.h"
#include "log_tag_itf.h"
#include "log_fmt_itf.h"

namespace mlog
{

template<class TCH>
struct module_logger_itf
{
	typedef boost::shared_ptr< mlog::detail::thread_logger<TCH> > thread_logger_ptr;

	virtual ~module_logger_itf() { }
	virtual void tl_completed(const thread_logger_ptr& tl, const format_provider<TCH>& fp) const = 0;
	virtual const TCH* get_name() const = 0;
	virtual int get_id() const = 0;
	virtual const tag_provider<TCH>* get_tag_provider() const = 0;
	virtual const level_name_provider<TCH>* get_level_name_provider() const = 0;
	virtual int get_level() const = 0;
	virtual thread_logger_ptr get_logger() = 0;
	virtual void set_tag(const TCH* name, const TCH* value) = 0;
	virtual void set_tag(int id, const TCH* value) = 0;
	virtual void enable(bool enable) = 0;
	virtual bool is_enabled() const = 0;
};

#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef module_logger_itf<wchar_t> wmodule_logger_itf;
#endif
typedef module_logger_itf<char> amodule_logger_itf;
typedef module_logger_itf<char> cmodule_logger_itf;

}
