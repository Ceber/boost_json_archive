/*
 * main.cpp
 *
 *  Created on: 10 oct. 2021
 *      Author: ceber
 */

#include "BoostSerializationJson.hpp"
#include "structs.hpp"
#include <gtest/gtest.h>

BOOST_CLASS_EXPORT_IMPLEMENT(Object)
BOOST_CLASS_EXPORT_IMPLEMENT(BoolsObject)
BOOST_CLASS_EXPORT_IMPLEMENT(NestedBoolObjects)
BOOST_CLASS_EXPORT_IMPLEMENT(NestedBoolObjectsPtrs)

int main(int argc, char **argv) {
  boost::serialization::void_cast_register<BoolsObject, Object>(static_cast<BoolsObject *>(NULL), static_cast<Object *>(NULL));
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
