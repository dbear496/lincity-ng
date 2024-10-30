/* ---------------------------------------------------------------------- *
 * src/lincity/xmlloadsave.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2024 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __xmlloadsave_h__
#define __xmlloadsave_h__

#include <string>    // for string, basic_string

#define XML_LOADSAVE_VERSION 1328

namespace xml_loadsave {

extern void saveXMLfile(std::string xml_file_name);
extern void loadXMLfile(std::string xml_file_name);

extern void writeArray(xmlTextWriterPtr xmlWriter, int *ary, int len);
extern void readArray(xmlpp::TextReader& xmlReader, int *ary, int len);

}; /* namespace xml_loadsave */

#endif /* __xmlloadsave_h__ */

/** @file lincity/loadsave.h */
