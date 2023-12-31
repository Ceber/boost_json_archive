/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_json_iarchive.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <istream>

#define BOOST_ARCHIVE_SOURCE
#include "boost/archive/polymorphic_json_iarchive.hpp"
#include <boost/archive/impl/archive_serializer_map.ipp>

namespace boost {
namespace archive {

// explicitly instantiate for this type of json stream
template class detail::archive_serializer_map<polymorphic_json_iarchive>;

} // namespace archive
} // namespace boost