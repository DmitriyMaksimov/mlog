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

#include "log_sink.h"
#include "log_module_itf.h"
#include "log_level.h"
#include "log_tag_provider.h"
#include <vector>
#include <map>
#include <locale>
#include <boost/smart_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/utility.hpp>

namespace mlog
{

namespace detail
{

class log_locker
{
public:
	log_locker() :
		m_pmutex(NULL)
	{
	}

	~log_locker()
	{
		if (m_pmutex != NULL)
			m_pmutex->unlock();
	}

	void lock_next(boost::mutex* pmutex)
	{
		pmutex->lock();
		if (m_pmutex != NULL)
			m_pmutex->unlock();
		m_pmutex = pmutex;
	}

private:
	boost::mutex* m_pmutex;
};

} //detail

// main class for logging support
// stores and manipulates log sinks, log levels, per-thread loggers, values of tags
template<class TCH>
class module_logger : boost::noncopyable, public module_logger_itf<TCH>
{
private:
	typedef boost::shared_ptr< mlog::detail::thread_logger<TCH> > thread_logger_ptr;

public:
	// type of this module
	typedef module_logger<TCH> self_t;

	// type of sink, sinks write log messages to their destination
	typedef sink<TCH> sink_t;
	typedef boost::shared_ptr<sink_t> sink_ptr;

	// type of filters, used with our sinks
	typedef typename sink_t::filter_ptr filter_ptr;

	// constructor, which takes module name as an argument
	module_logger(const TCH* name = NULL, int id = 0) :
		m_name(name == NULL ? detail::select_wchars<TCH>::main() : name),
		m_id(id),
		m_level(info),
		m_enabled(true),
		m_plevels(new detail::default_level_name_provider<TCH>),
		m_ptags(new detail::default_tag_provider<TCH>)
	{
	}

	// this function is a setup function, and thus it is not thread safe
	// the function imbues all existing sinks with provided locale
	self_t& imbue_all(const std::locale& new_locale)
	{
		if (!m_map_thread_loggers.empty())
			throw std::logic_error(get_setup_error_str());
		for (sink_iter_t i = m_vec_sinks.begin(); i != m_vec_sinks.end(); i++)
			i->get()->imbue(new_locale);
		m_locale = new_locale;
		return *this;
	}

	// this function is used to setup log level
	self_t& set_level(int level)
	{
		m_level = level;
		return *this;
	}

	// returns log level
	int get_level() const
	{
		return m_level;
	}

	// this function is a setup function, and thus it is not thread safe
	// the function sets format of all sinks, which were previously added to this log module
	self_t& set_format_all(const format<TCH>& new_format)
	{
		if (!m_map_thread_loggers.empty())
			throw std::logic_error(get_setup_error_str());
		for (sink_iter_t i = m_vec_sinks.begin(); i != m_vec_sinks.end(); i++)
			i->get()->set_format(new_format);
		return *this;
	}

	// this function is a setup function, and thus it is not thread safe
	// the function adds a new sink to this log module
	self_t& add_sink(sink_ptr new_sink)
	{
		if (!m_map_thread_loggers.empty())
			throw std::logic_error(get_setup_error_str());
		new_sink->imbue(m_locale);
		m_vec_sinks.push_back(new_sink);
		return *this;
	}

	// this function is a setup function, and thus it is not thread safe
	// the function adds a new sink to this log module
	// it takes ownership of the pointer, converting it into shared pointer
	self_t& add_sink(sink_t* psink)
	{
		if (!m_map_thread_loggers.empty())
			throw std::logic_error(get_setup_error_str());
		psink->imbue(m_locale);
		m_vec_sinks.push_back(sink_ptr(psink));
		return *this;
	}

	// this is a thread safe function, which is automatically called by the framework
	// it returns a shared pointer to the logger for the current thread, creating new if necessary
	// each thread logger is automatically imbued with the locale of this log module, when it is created
	thread_logger_ptr get_logger()
	{
		boost::shared_lock<boost::shared_mutex> lock(m_mutex_tl);
		boost::thread::id tid = boost::this_thread::get_id();
		tl_iter_t i = m_map_thread_loggers.find(tid);
		if (i == m_map_thread_loggers.end())
		{
			lock.unlock();
			boost::unique_lock<boost::shared_mutex> lock(m_mutex_tl);
			thread_logger_ptr ptl(new detail::thread_logger<TCH>);
			m_map_thread_loggers[tid] = ptl;
			ptl->imbue(m_locale);
			return ptl;
		}
		else
			return i->second;
	}

	// called on a thread, when the thread finishes writing to its stream
	void tl_completed(const thread_logger_ptr& tl, const format_provider<TCH>& fp) const
	{
		// access buffer of the stream to get raw log message
		std::basic_ostringstream<TCH>& strm = tl->get_stream();
		detail::get_stringbuf_base<TCH>* pgetbase = reinterpret_cast<detail::get_stringbuf_base<TCH>*>(strm.rdbuf());
		const TCH* log_str = pgetbase->get();
		typename std::basic_ostream<TCH>::pos_type log_len = strm.tellp();
		// access the sink pipeline
		detail::log_locker locker;
		for (sink_citer_t i = m_vec_sinks.begin(); i != m_vec_sinks.end(); i++)
		{
			sink<TCH>* psink = i->get();
			if (!psink->is_enabled())
				continue;
			locker.lock_next(psink->get_mutex());
			psink->rotate();
			psink->log_string(log_str, log_len, fp);
		}
	}

	// returns a stored pointer to the instance of log level name provider,
	// a class, which is used to convert log levels to string representation
	const level_name_provider<TCH>* get_level_name_provider() const
	{
		return m_plevels.get();
	}

	// this function is a setup function, and thus it is not thread safe
	// the function stores a pointer to log level name provider, which is used to convert log levels
	// to string representation
	self_t& set_level_name_provider(std::auto_ptr< level_name_provider<TCH> > prov)
	{
		if (!m_map_thread_loggers.empty())
			throw std::logic_error(get_setup_error_str());
		m_plevels = prov;
		return *this;
	}

	// returns a stored pointer to the instance of log tag provider, which is used to maintain tags
	// (text values mapped to other text values)
	const tag_provider<TCH>* get_tag_provider() const
	{
		return m_ptags.get();
	}

	// this function is a setup function, and thus it is not thread safe
	// the function stores a pointer to log tag provider, which is used to resolve tag names into their values
	self_t& set_tag_provider(std::auto_ptr< tag_provider<TCH> > prov)
	{
		if (!m_map_thread_loggers.empty())
			throw std::logic_error(get_setup_error_str());
		m_ptags = prov;
		return *this;
	}

	// this function is thread safe, if the tag provider of this log module is thread safe
	// it sets a named tag to the provided value
	void set_tag(const TCH* name, const TCH* value)
	{
		m_ptags->set_tag(name, value);
	}

	// this function is thread safe, if the tag provider of this log module is thread safe
	// it sets a tag, identified by an integer, to the provided value
	void set_tag(int id, const TCH* value)
	{
		m_ptags->set_tag(id, value);
	}

	// this function returns name of this log module
	const TCH* get_name() const
	{
		return m_name.c_str();
	}

	// this function returns integer id of this module
	int get_id() const
	{
		return m_id;
	}

	// this function is a setup function, and thus it is not thread safe
	// the function adds filter for all sinks
	self_t& add_filter_all(filter_ptr pfilter)
	{
		if (!m_map_thread_loggers.empty())
			throw std::logic_error(get_setup_error_str());
		for (sink_iter_t i = m_vec_sinks.begin(); i != m_vec_sinks.end(); i++)
			i->get()->add_filter(pfilter);
		return *this;
	}

	// this function can be used to completely disable the module
	void enable(bool enable)
	{
		m_enabled = enable;
	}

	// returns true, if this module is enabled
	bool is_enabled() const
	{
		return m_enabled;
	}

private:
	std::vector<sink_ptr> m_vec_sinks;
	typedef typename std::vector<sink_ptr>::iterator sink_iter_t;
	typedef typename std::vector<sink_ptr>::const_iterator sink_citer_t;
	std::map<boost::thread::id, thread_logger_ptr> m_map_thread_loggers;
	typedef typename std::map<boost::thread::id, thread_logger_ptr>::iterator tl_iter_t;
	std::locale m_locale;
	mutable boost::shared_mutex m_mutex_tl;
	std::basic_string<TCH> m_name;
	int m_id;
    volatile int m_level;
	volatile bool m_enabled;
    std::auto_ptr< level_name_provider<TCH> > m_plevels;
    std::auto_ptr< tag_provider<TCH> > m_ptags;

	const char* get_setup_error_str()
	{
		static const char* err_str = "setup functions must be called before a thread has made an attempt to write a log message";
		return err_str;
	}
};

typedef module_logger<char> cmodule_logger;
typedef module_logger<char> amodule_logger;
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
typedef module_logger<wchar_t> wmodule_logger;
#endif

}
