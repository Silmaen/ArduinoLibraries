/**
 * \author argawaen
 * \date 18/01/2021
 *
 */

#pragma once
#include "base_config.h"

namespace astd {

    template<class T, class container>
    struct iterator {
        using value_type    = T;
        using pointer       = value_type *;
        using reference     = value_type &;
        using size_type     = a_size_t;
        using contener_type = container;

    protected:
        contener_type *_ref;
    };

} // namespace astd
