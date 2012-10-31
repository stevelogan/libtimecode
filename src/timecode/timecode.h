/**
   @brief libtimecode - convert A/V timecode and framerate
   @file timecode.h
   @author Robin Gareus <robin@gareus.org>

   Copyright (C) 2006, 2007, 2008, 2012 Robin Gareus <robin@gareus.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef TIMECODE_H
#define TIMECODE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> /* size_t */

#ifndef DOXYGEN_IGNORE
/* libtimecode version */
#define LIBTIMECODE_VERSION "0.5.0"
#define LIBTIMECODE_VERSION_MAJOR  0
#define LIBTIMECODE_VERSION_MINOR  0
#define LIBTIMECODE_VERSION_MICRO  0

/* interface revision number
 * http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
 */
#define LIBTIMECODE_CUR  0
#define LIBTIMECODE_REV  0
#define LIBTIMECODE_AGE  0
#endif

#ifndef int32_t
typedef int int32_t;
#endif

#ifndef int64_t
typedef long long int int64_t;
#endif

/**
 * classical timecode
 */
typedef struct TimecodeTime {
	int32_t hour; ///< timecode hours 0..24
	int32_t minute; ///< timecode minutes 0..59
	int32_t second; ///< timecode seconds 0..59
	int32_t frame; ///< timecode frames 0..fps
	int32_t subframe; ///< timecode subframes 0..
} TimecodeTime;

/**
 * date and timezone
 */
typedef struct TimecodeDate {
	int32_t year; ///< year A.D. 4 digits
	int32_t month; ///< month 1..12
	int32_t day;  ///< day of month 0..31
	int32_t timezone; ///< minutes west of UTC +1245 (Chatham Island) .. -1200 (Kwaialein); LA: -0800, NYC: -0500, Paris: +0100, Bombay: +0530, Tokyo: +0900
} TimecodeDate;


/**
 * define a framerate
 */
typedef struct TimecodeRate {
	int32_t num; ///< fps numerator
	int32_t den; ///< fps denominator
	int drop; ///< 1: use drop-frame timecode (only valid for 2997/100)
	int32_t subframes; ///< number of subframes per frame
} TimecodeRate;

/**
 * complete datetime description
 */
typedef struct Timecode {
	TimecodeTime t; ///< timecode HH:MM:SS:FF.SSS
	TimecodeDate d; ///< date MM/DD/YYYY + Timezone
	//TimecodeRate r; /* XXX ?? might come in handy here ?? */
} Timecode;


/* TODO: subframe default, use 80, 100 or  5760,.. (44.1k, 48k || 24fps, 25fps) or 500000 (MPEG)*/

const TimecodeRate tcfps23976   = {24000, 1001, 0, 80};
const TimecodeRate tcfps24      = {24, 1, 0, 100};
const TimecodeRate tcfps24976   = {25000, 1001, 0, 80};
const TimecodeRate tcfps25      = {25, 1, 0, 80};
const TimecodeRate tcfps2997ndf = {30000, 1001, 0, 100};
const TimecodeRate tcfps2997df  = {30000, 1001, 1, 100};
const TimecodeRate tcfps2997xdf = {30*999, 1000, 1, 100};
const TimecodeRate tcfps30      = {30, 1, 0, 80};
const TimecodeRate tcfpsMS      = {1000, 1, 0, 1000};

#define TCFPS23976 (&tcfps23976)
#define TCFPS24 (&tcfps24)
#define TCFPS24976 (&tcfps24976)
#define TCFPS25 (&tcfps25)
#define TCFPS2997DF (&tcfps2997df)
#define TCFPS30 (&tcfps30)
#define TCFPSMS (&tcfpsMS)

/**
 * convert Timecode to audio sample number
 *
 * NB. this function can also be used to convert integer milli-seconds or
 * micro-seconds by specifying a samplerate of 1000 or 10^6 respectively.
 *
 * When used with samplerate == \ref timecode_rate_to_double this function can also convert
 * timecode to video-frame number.
 *
 * @param t the timecode to convert
 * @param r framerate to use for conversion
 * @param samplerate the sample-rate the sample was taken at
 * @return audio sample number
 */
int64_t timecode_to_sample (TimecodeTime const * const t, TimecodeRate const * const r, const double samplerate);

/**
 * convert audio sample number to timecode
 *
 * NB. this function can also be used to convert integer milli-seconds or
 * micro-seconds by specifying a samplerate of 1000 or 10^6 respectively.
 *
 * When used with samplerate == \ref timecode_rate_to_double this function can also convert
 * video-frame number to timecode.
 *
 * @param t [output] the timecode that corresponds to the sample
 * @param r framerate to use for conversion
 * @param samplerate the sample-rate the sample was taken at
 * @param sample the audio sample number to convert
 */
void timecode_sample_to_time (TimecodeTime * const t, TimecodeRate const * const r, const double samplerate, const int64_t sample);


/* ADD/SUBTRACT timecodes at same framerate */

/**
 */
void timecode_time_add (TimecodeTime * const res, TimecodeRate const * const r, TimecodeTime const * const t1, TimecodeTime const * const t2);
/**
 */
void timecode_time_subtract (TimecodeTime * const res, TimecodeRate const * const r, TimecodeTime const * const t1, TimecodeTime const * const t2);


/* COMPARISON OPERATORS at same framerate */

/**
 * The timecode_time_compare() function compares the two timecodes a and b.
 * It returns an integer less than, equal to, or greater than zero if a is
 * found, respectively, to be later than, to match, or be earlier than b.
 *
 * @param r framerate to use for both a and b
 * @param a Timecode to compare (using framerate r)
 * @param b Timecode to compare (using framerate r)
 * @return +1 if a is later than b, -1 if a is earlier than b, 0 if timecodes are equal
 */
int timecode_time_compare (TimecodeRate const * const r, TimecodeTime const * const a, TimecodeTime const * const b);

/**
 * The timecode_date_compare() function compares the two dates a and b.
 * It returns an integer less than, equal to, or greater than zero if a is
 * found, respectively, to be later than, to match, or be earlier than b.
 *
 * @param a date to compare
 * @param b date to compare
 * @return +1 if a is later than b, -1 if a is earlier than b, 0 if timecodes are equal
 */
int timecode_date_compare (TimecodeDate const * const a, TimecodeDate const * const b);

/**
 * The timecode_datetime_compare() function compares the two datetimes a and b.
 * It returns an integer less than, equal to, or greater than zero if a is
 * found, respectively, to be later than, to match, or be earlier than b.
 *
 * This function is a wrapper around \ref timecode_time_compare and \ref timecode_date_compare
 * it includes additional functionality to handle timezones correctly.
 *
 * @param r framerate to use for both a and b
 * @param a Timecode to compare (using framerate r)
 * @param b Timecode to compare (using framerate r)
 * @return +1 if a is later than b, -1 if a is earlier than b, 0 if timecodes are equal
 */
int timecode_datetime_compare (TimecodeRate const * const r, Timecode const * const a, Timecode const * const b);


/* increment, decrement */

/**
 * increment date by one day.
 * Note: This function honors leap-years.
 * @param d the date to adjust
 */
void timecode_date_increment(TimecodeDate * const d);
/**
 * decrement date by one day.
 * Note: this function honors leap-years.
 * @param d the date to adjust
 */
void timecode_date_decrement (TimecodeDate * const d);
/**
 * increment timecode by one frame.
 * @param t the timecode to modify
 * @param r framerate to use
 * @return 1 if timecode wrapped 24 hours, 0 otherwise
 */
int timecode_time_increment(TimecodeTime * const t, TimecodeRate const * const r);
/**
 * decrement timecode by one frame.
 * @param t the timecode to modify
 * @param r framerate to use
 * @return 1 if timecode wrapped 24 hours, 0 otherwise
 */
int timecode_time_decrement(TimecodeTime * const t, TimecodeRate const * const r);
/**
 * increment datetime by one frame
 * this is a wrapper function around \ref timecode_date_increment and
 * \ref timecode_time_increment
 * @param dt the datetime to modify
 * @param r framerate to use
 * @return 1 if timecode wrapped 24 hours, 0 otherwise
 */
int timecode_datetime_increment (Timecode * const dt, TimecodeRate const * const r);
/**
 * increment datetime by one frame
 * this is a wrapper function around \ref timecode_date_increment and
 * \ref timecode_time_increment
 * @param dt the datetime to modify
 * @param r framerate to use
 * @return 1 if timecode wrapped 24 hours, 0 otherwise
 */
int timecode_datetime_decrement (Timecode * const dt, TimecodeRate const * const r);


/* parse from string, export to string */

/**
 * format timecode as string "MM/DD/YYYY HH:MM:SS:FF.SSS +TZMM"
 * @param tc the datetime to print
 * @param smptestring [output] length of smptestring: 33 bytes (incl terminating zero)
 */
void timecode_datetime_to_string (Timecode const * const tc, char *smptestring);
/**
 * format timecode as string "HH:MM:SS:FF.SSS"
 * @param t the timecode to print
 * @param smptestring [output] length of smptestring: 16 bytes (incl terminating zero)
 */
void timecode_time_to_string (TimecodeTime const * const t, char *smptestring);
/**
 * parse string to timecode time - separators may include ":.;"
 * the format is "[[[HH:]MM:]SS:]FF", subframes are set to 0.
 *
 * @param t [output] the parsed timecode
 * @param r framerate to use
 * @param val the value to parse
 */
void timecode_parse_time (TimecodeTime * const t, TimecodeRate const * const r, const char *val);

/**
 * convert rational framerate to double (r->num/r->den).
 *
 * @param r framerate to convert
 * @return double representation of framerate
 */
double timecode_rate_to_double(TimecodeRate const * const r);


/* TODO, ideas */
// parse from float sec, export to float sec
// add/subtract int frames or float sec
// convert to video frame number and back
// -> use timecode_rate_to_double() as samplerate
// parse date, timezone
//
// convert between framerates
// -> timecode_to_sample() + timecode_sample_to_time()
//
// Bar, Beat, Tick Time (Tempo-Based Time)

#ifdef __cplusplus
}
#endif


#endif
