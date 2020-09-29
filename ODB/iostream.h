/* Arduino SdFat Library
 * Copyright (C) 2012 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef iostream_h
#define iostream_h
/**
 * \file
 * \brief \ref iostream class
 */
#include "istream.h"
#include "ostream.h""
/** Skip white space
 * \param[in] is the Stream
 * \return The stream
 */
inline istream& ws(istream& is) {
	is.skipWhite();
	return is;
}
/** insert endline
 * \param[in] os The Stream
 * \return The stream
 */
inline ostream& endl(ostream& os) {
	os.put('\n');
#if ENDL_CALLS_FLUSH
	os.flush();
#endif  // ENDL_CALLS_FLUSH
	return os;
}
/** flush manipulator
 * \param[in] os The stream
 * \return The stream
 */
inline ostream& flush(ostream& os) {
	os.flush();
	return os;
}
/**
 * \struct setfill
 * \brief type for setfill manipulator
 */
struct setfill {
	/** fill character */
	char c;
	/** constructor
	 *
	 * \param[in] arg new fill character
	 */
	explicit setfill(char arg) : c(arg) {}
};
/** setfill manipulator
 * \param[in] os the stream
 * \param[in] arg set setfill object
 * \return the stream
 */
inline ostream &operator<< (ostream &os, const setfill &arg) {
	os.fill(arg.c);
	return os;
}
/** setfill manipulator
 * \param[in] obj the stream
 * \param[in] arg set setfill object
 * \return the stream
 */
inline istream &operator>>(istream &obj, const setfill &arg) {
	obj.fill(arg.c);
	return obj;
}
//------------------------------------------------------------------------------
/** \struct setprecision
 * \brief type for setprecision manipulator
 */
struct setprecision {
	/** precision */
	unsigned int p;
	/** constructor
	 * \param[in] arg new precision
	 */
	explicit setprecision(unsigned int arg) : p(arg) {}
};
/** setprecision manipulator
 * \param[in] os the stream
 * \param[in] arg set setprecision object
 * \return the stream
 */
inline ostream &operator<< (ostream &os, const setprecision &arg) {
	os.precision(arg.p);
	return os;
}
/** setprecision manipulator
 * \param[in] is the stream
 * \param[in] arg set setprecision object
 * \return the stream
 */
inline istream &operator>>(istream &is, const setprecision &arg) {
	is.precision(arg.p);
	return is;
}
//------------------------------------------------------------------------------
/** \struct setw
 * \brief type for setw manipulator
 */
struct setw {
	/** width */
	unsigned w;
	/** constructor
	 * \param[in] arg new width
	 */
	explicit setw(unsigned arg) : w(arg) {}
};
/** setw manipulator
 * \param[in] os the stream
 * \param[in] arg set setw object
 * \return the stream
 */
inline ostream &operator<< (ostream &os, const setw &arg) {
	os.width(arg.w);
	return os;
}
/** setw manipulator
 * \param[in] is the stream
 * \param[in] arg set setw object
 * \return the stream
 */
inline istream &operator>>(istream &is, const setw &arg) {
	is.width(arg.w);
	return is;
}
//==============================================================================
/**
 * \class iostream
 * \brief Input/Output stream
 */
class iostream : public istream, public ostream {
};
#endif  /* iostream_h */
