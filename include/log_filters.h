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

// this file defines log filters, which based on log prefix and suffix format filtering

#include "log_filter_itf.h"
#include <string>
#include <algorithm>

namespace mlog
{

namespace detail
{

// filter, which matches module name against the provided string
template<class TCH>
class module_is : public filter<TCH>
{
public:
	module_is(const TCH* name) : m_name(name) { }

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH*, typename std::basic_ostream<TCH>::pos_type) const
	{
		return m_name == fmt_prov.get_module();
	}

private:
	std::basic_string<TCH> m_name;
};

// filter, which matches module integer id against the provided value
template<class TCH>
class module_id_is : public filter<TCH>
{
public:
	module_id_is(int id) : m_id(id) { }

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH*, typename std::basic_ostream<TCH>::pos_type) const
	{
		return m_id == fmt_prov.get_module_id();
	}

private:
	int m_id;
};

// filter, which checks short file name against the provided string
template<class TCH>
class file_is : public filter<TCH>
{
public:
	file_is(const TCH* name) : m_name(name) { }

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH*, typename std::basic_ostream<TCH>::pos_type) const
	{
		return m_name == fmt_prov.get_file(true);
	}

private:
	std::basic_string<TCH> m_name;
};

// filter, which checks log level against the provided number
template<class TCH>
class level_is : public filter<TCH>
{
public:
	level_is(int level) : m_level(level) { }

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH*, typename std::basic_ostream<TCH>::pos_type) const
	{
		return m_level == fmt_prov.get_level();
	}

private:
	int m_level;
};

// filter, which checks a named tag against the provided string
// note that if tag doesn't exist, its value will be successfully matched against an empty string
template<class TCH>
class stag_is : public filter<TCH>
{
public:
	stag_is(const TCH* tag_name, const TCH* tag_val) :
		m_name(tag_name),
		m_val(tag_val)
	{
	}

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH*, typename std::basic_ostream<TCH>::pos_type) const
	{
		return m_val == fmt_prov.get_tag(m_name.c_str());
	}

private:
	std::basic_string<TCH> m_name;
	std::basic_string<TCH> m_val;
};

// filter, which checks a tag, identified by a number, against the provided string
// note that if tag doesn't exist, its value will be successfully matched against an empty string
template<class TCH>
class itag_is : public filter<TCH>
{
public:
	itag_is(int tag_id, const TCH* tag_val) :
		m_id(tag_id),
		m_val(tag_val)
	{
	}

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH*, typename std::basic_ostream<TCH>::pos_type) const
	{
		return m_val == fmt_prov.get_tag(m_id);
	}

private:
	int m_id;
	std::basic_string<TCH> m_val;
};

// filter, which checks if a tag exists
template<class TCH>
class stag_exists : public filter<TCH>
{
public:
	stag_exists(const TCH* tag_name) : m_name(tag_name) { }

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH*, typename std::basic_ostream<TCH>::pos_type) const
	{
		return fmt_prov.get_tag(m_name.c_str())[0] != detail::select_wchars<TCH>::eol;
	}

private:
	std::basic_string<TCH> m_name;
};

// filter, which checks if a tag exists
template<class TCH>
class itag_exists : public filter<TCH>
{
public:
	itag_exists(int id) : m_id(id) { }

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH*, typename std::basic_ostream<TCH>::pos_type) const
	{
		return fmt_prov.get_tag(m_id)[0] != detail::select_wchars<TCH>::eol;
	}

private:
	int m_id;
};

// filter, which checks log message for containing some text
template<class TCH>
class message_contains : public filter<TCH>
{
public:
	message_contains(const TCH* str) : m_str(str) { }

	virtual bool allow(const format_provider<TCH>&, const TCH* log_str, typename std::basic_ostream<TCH>::pos_type log_len) const
	{
		const TCH* str = m_str.c_str();
		return std::search(log_str, log_str+log_len, str, str+m_str.length()) != log_str+log_len;
	}

private:
	std::basic_string<TCH> m_str;
};

// filter, which checks log message for NOT containing some text
template<class TCH>
class message_not_contains : public message_contains<TCH>
{
public:
	message_not_contains(const TCH* str) : message_contains<TCH>(str) { }

	virtual bool allow(const format_provider<TCH>& fmt_prov, const TCH* log_str, typename std::basic_ostream<TCH>::pos_type log_len) const
	{
		return !message_contains<TCH>::allow(fmt_prov, log_str, log_len);
	}
};

} // namespace detail

// following is a set of helper functions to create different filters

// filter, which matches module name against the provided string
template<class TCH>
boost::shared_ptr< filter<TCH> > module_is(const TCH* name)
{
	return boost::shared_ptr< filter<TCH> >(new detail::module_is<TCH>(name));
}

// filter, which matches integer module id against the provided value
template<class TCH>
boost::shared_ptr< filter<TCH> > module_is(int id)
{
	return boost::shared_ptr< filter<TCH> >(new detail::module_id_is<TCH>(id));
}

// filter, which checks short file name against the provided string
template<class TCH>
boost::shared_ptr< filter<TCH> > file_is(const TCH* name)
{
	return boost::shared_ptr< filter<TCH> >(new detail::file_is<TCH>(name));
}

// filter, which checks log level against the provided number
template<class TCH>
boost::shared_ptr< filter<TCH> > level_is(int level)
{
	return boost::shared_ptr< filter<TCH> >(new detail::level_is<TCH>(level));
}

// helper function to create tag filters based on tag names
template<class TCH>
boost::shared_ptr< filter<TCH> > tag_is(const TCH* tag_name, const TCH* tag_val)
{
	return boost::shared_ptr< filter<TCH> >(new detail::stag_is<TCH>(tag_name, tag_val));
}

template<class TCH>
boost::shared_ptr< filter<TCH> > tag_exists(const TCH* tag_name)
{
	return boost::shared_ptr< filter<TCH> >(new detail::stag_exists<TCH>(tag_name));
}

// helper function to create tag filters based on integer identifiers
template<class TCH>
boost::shared_ptr< filter<TCH> > tag_is(int tag_id, const TCH* tag_val)
{
	return boost::shared_ptr< filter<TCH> >(new detail::itag_is<TCH>(tag_id, tag_val));
}

template<class TCH>
boost::shared_ptr< filter<TCH> > tag_exists(int tag_id)
{
	return boost::shared_ptr< filter<TCH> >(new detail::itag_exists<TCH>(tag_id));
}

// filter, which checks log message for containing some text
template<class TCH>
boost::shared_ptr< filter<TCH> > message_contains(const TCH* str)
{
	return boost::shared_ptr< filter<TCH> >(new detail::message_contains<TCH>(str));
}

// filter, which checks log message for NOT containing some text
template<class TCH>
boost::shared_ptr< filter<TCH> > message_not_contains(const TCH* str)
{
	return boost::shared_ptr< filter<TCH> >(new detail::message_not_contains<TCH>(str));
}

}
