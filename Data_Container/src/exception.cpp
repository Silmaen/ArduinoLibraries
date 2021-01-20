/**
 * \author argawaen
 * \date 20/01/2021
 *
 */

#include "exception"

// We can't do this yet because gcc is too stupid to be able to handle
// different implementations of exception class.

#undef ASTD_EXCEPTION_SUPPORT

#ifdef ASTD_EXCEPTION_SUPPORT

namespace astd {
    static char *__std_exception_what_value = "exception";

    // We are providing our own versions to be sneaky

    exception::~exception() ASTD_USE_NOEXCEPT  = default;

    const char *exception::what() const ASTD_USE_NOEXCEPT { return __std_exception_what_value; }

    bad_exception::~bad_exception() ASTD_USE_NOEXCEPT  = default;

} // namespace astd

#else

namespace astd {

    exception::~exception() ASTD_USE_NOEXCEPT = default;

    const char *exception::what() const ASTD_USE_NOEXCEPT { return ""; }

    bad_exception::~bad_exception() ASTD_USE_NOEXCEPT = default;

} // namespace astd

#endif
