# - Find the Spread Toolkit
# This module defines
#  SPREAD_INCLUDE_DIRS, where to find json.h, etc.
#  SPREAD_LIBRARIES, the libraries needed to use jsoncpp.
#
# Will react on:
#  SPREAD_ROOT, additional directory to search for spread
#  SPREAD_USE_STATIC, if enabled, search for static lib instead of shared, default OFF
#
# Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

INCLUDE(FindPackageHandleStandardArgs)

FIND_PATH(SPREAD_INCLUDE_DIRS
          NAMES sp.h
          PATHS "${SPREAD_ROOT}/include"
                "$ENV{HOME}/include")

SET(LIBNAME "libspread.so")
IF(SPREAD_USE_STATIC)
    SET(LIBNAME "libspread.a")
ENDIF()

FIND_LIBRARY(SPREAD_LIBRARIES
             NAMES ${LIBNAME}
             PATHS "${SPREAD_ROOT}/lib"
                   "$ENV{HOME}/lib")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Spread DEFAULT_MSG SPREAD_LIBRARIES SPREAD_INCLUDE_DIRS)

MARK_AS_ADVANCED(JSONCPP_LIBRARIES JSONCPP_INCLUDE_DIR)