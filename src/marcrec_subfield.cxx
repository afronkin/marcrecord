/*
 * Copyright (C) 2011  Alexander Fronkin
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
 */

/* Version: 2.0 (27 Feb 2011) */

#include "marcrec.h"

/*
 * Clear subfield data.
 */
void MarcRecord::Subfield::clear()
{
	id = ' ';
	data.erase();
}

/*
 * Check presence of embedded field.
 */
bool MarcRecord::Subfield::isEmbedded()
{
	return (id == '1' ? true : false);
}

/*
 * Get tag of embedded field.
 */
std::string MarcRecord::Subfield::getEmbeddedTag()
{
	if (id != '1') {
		return "";
	}

	return data.substr(0, 3);
}

/*
 * Get indicator 1 of embedded field.
 */
char MarcRecord::Subfield::getEmbeddedInd1()
{
	if (id != '1' || data.substr(0, 3) < "010") {
		return '?';
	}

	return data[3];
}

/*
 * Get indicator 2 of embedded field.
 */
char MarcRecord::Subfield::getEmbeddedInd2()
{
	if (id != '1' || data.substr(0, 3) < "010") {
		return '?';
	}

	return data[4];
}

/*
 * Get data of embedded field.
 */
std::string MarcRecord::Subfield::getEmbeddedData()
{
	if (id != '1' || data.substr(0, 3) >= "010") {
		return "";
	}

	return data.substr(3);
}

