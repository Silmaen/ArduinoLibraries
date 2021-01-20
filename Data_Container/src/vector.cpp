/**
 * \author argawaen
 * \date 18/01/2021
 *
 */

#include "vector.h"

namespace astd {

    template<class T>
    vector<T>::~vector() {
        delete data[_allocated];
    }

    template<class T>
    void vector<T>::shrink_to_fit() {}

} // namespace astd
