ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I../include -Wall -Wno-switch -Wno-reorder
AM_LDFLAGS = $(BOOST_LDFLAGS)
LDADD = $(BOOST_THREAD_LIB)
CLEANFILES = *.log *.exe

EXTRA_DIST = *.sln *.vcproj memory_test.cpp performance_test.cpp

noinst_PROGRAMS=basic_usage filters_basics light custom_levels sinks_example tag_basics unicode log_modules

basic_usage_SOURCES=basic_usage.cpp

filters_basics_SOURCES=filters_basics.cpp

light_SOURCES=light_first.cpp light_second.cpp

custom_levels_SOURCES=custom_levels.cpp

sinks_example_SOURCES=sinks_example.cpp

tag_basics_SOURCES=tag_basics.cpp

unicode_SOURCES=unicode.cpp
unicode_CPPFLAGS=-finput-charset=cp1251 -fexec-charset=$(exec_charset) $(AM_CPPFLAGS)

log_modules_SOURCES=log_modules.cpp
