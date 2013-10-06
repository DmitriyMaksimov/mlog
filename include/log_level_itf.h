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
struct level_name_provider
{
	virtual ~level_name_provider() throw() { }
	virtual const TCH* level_name(int n) const = 0;
};

typedef std::auto_ptr< level_name_provider<char> > clevel_name_provider_ptr;
typedef std::auto_ptr< level_name_provider<char> > alevel_name_provider_ptr;
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef std::auto_ptr< level_name_provider<wchar_t> > wlevel_name_provider_ptr;
#endif

}
