##
# This file is part of QFCgi.
#
# QFCgi is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# QFCgi is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with QFCgi. If not, see <http://www.gnu.org/licenses/>.
##

TEMPLATE  = lib
TARGET    = qfcgi
QT        = core network
CONFIG   += staticlib debug warn_on

SOURCES = \
  connection.cpp \
  qfcgi.cpp \
  record.cpp \
  request.cpp \
  stream.cpp

HEADERS += \
  connection.h \
  qfcgi.h \
  record.h \
  request.h \
  stream.h
