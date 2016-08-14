// Ryzom - MMORPG Framework <http://dev.ryzom.com/projects/ryzom/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef UTILS_H
#define UTILS_H

#include <nel/misc/ucstring.h>

#include <string>

/**
 * Utils functions
 *
 * \author Cedric 'Kervala' OCHS
 * \date 2016
 */

QString qBytesToHumanReadable(qint64 bytes);

qint64 getDirectorySize(const QString &directory, bool recursize);

// Convert a UTF-8 string to QString
QString qFromUtf8(const std::string &str);

// Convert a QString to UTF-8 string
std::string qToUtf8(const QString &str);

// Convert a UTF-16 string to QString
QString qFromUtf16(const ucstring &str);

// Convert a QString to UTF-16 string
ucstring qToUtf16(const QString &str);

QString qFromWide(const wchar_t *str);

wchar_t* qToWide(const QString &str);

bool createLink(const QString &link, const QString &name, const QString &executable, const QString &arguments, const QString &icon, const QString &workingDir);
bool resolveLink(const QWidget &window, const QString &pathLink, QString &pathObj);

#endif
