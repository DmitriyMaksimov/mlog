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

#include <memory>

namespace mlog
{

template<class TCH>
struct tag_provider
{
	virtual ~tag_provider() throw() { }

	// this function must be thread safe, unless you create a single threaded tag provider
	// it must return empty string, if tag is not found
	virtual const TCH* get_tag(const TCH* name) const = 0;
	virtual const TCH* get_tag(int id) const = 0;

	// this function must be thread safe, unless you create a single threaded tag provider
	virtual void set_tag(const TCH* name, const TCH* value) = 0;
	virtual void set_tag(int id, const TCH* value) = 0;
};

typedef std::auto_ptr< tag_provider<char> > ctag_provider_ptr;
typedef std::auto_ptr< tag_provider<char> > atag_provider_ptr;
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef std::auto_ptr< tag_provider<wchar_t> > wtag_provider_ptr;
#endif

}
