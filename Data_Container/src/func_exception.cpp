/**
 * \author argawaen
 * \date 20/01/2021
 *
 */

#include "cstdlib"
#include "func_exception"

namespace astd {

#ifdef ASTD_EXCEPTION_SUPPORT__

#    include <stdexcept>

    void __throw_bad_alloc() { throw bad_alloc(); }

    void __throw_out_of_range(const char *message) {
        if (message == 0) {
            throw out_of_range();
        }
        throw out_of_range(message);
    }

    void __throw_overflow_error(const char *message) {
        if (message == 0) {
            throw overflow_error();
        }
        throw overflow_error(message);
    }

    void __throw_length_error(const char *message) {
        if (message == 0) {
            throw length_error();
        }
        throw length_error(message);
    }

    void __throw_invalid_argument(const char *message) {
        if (message == 0) {
            throw invalid_argument();
        }
        throw invalid_argument(message);
    }

#else

    void __throw_bad_alloc() { abort(); }

    void __throw_out_of_range(const char * /*unused*/) { abort(); }

    void __throw_overflow_error(const char * /*unused*/) { abort(); }

    void __throw_length_error(const char * /*unused*/) { abort(); }

    void __throw_invalid_argument(const char * /*unused*/) { abort(); }

#endif

} // namespace astd
