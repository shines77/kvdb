
#ifndef KVDB_UTILS_OPTIONAL_H
#define KVDB_UTILS_OPTIONAL_H

#include <boost/optional.hpp>
#include <boost/utility/in_place_factory.hpp>

 //! Optional helper class to wrap optional values within.
template <typename T>
using Optional = boost::optional<T>;

#endif // KVDB_UTILS_OPTIONAL_H
