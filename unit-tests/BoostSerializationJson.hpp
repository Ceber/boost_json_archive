#include "gcout.hpp"
#include "structs.hpp"
// C++ Standard Library
#include <optional>
#include <sstream>
#include <string>

// // GTest
#include <gtest/gtest.h>

// // Boost Archive JSON
#include <boost/archive/json_iarchive.hpp>
#include <boost/archive/json_oarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/unordered_map.hpp>

class BoostSerializationJsonTest : public ::testing::Test {
protected:
  const std::string red = "\033[0;31m";
  const std::string green = "\033[0;32m";
  const std::string reset = "\033[0m";

  BoostSerializationJsonTest() = default;
  virtual ~BoostSerializationJsonTest() = default;
};

TEST_F(BoostSerializationJsonTest, CreateDeleteJsonInArchive) {
  std::stringstream ss;
  ss << "{}";
  boost::archive::json_iarchive ia{ss};
}

TEST_F(BoostSerializationJsonTest, CreateDeleteJsonOutArchive) {
  std::stringstream ss;
  ss << "{}";
  boost::archive::json_oarchive oa{ss};
}

TEST_F(BoostSerializationJsonTest, CreateDeleteJsonInArchiveThrowOnEmpty) {
  std::stringstream ss;
  ss << "";
  ASSERT_THROW(boost::archive::json_iarchive ia{ss}, std::runtime_error);
}

TEST_F(BoostSerializationJsonTest, DeserializeThrowOnMissing) {
  {
    std::stringstream ss;
    ss << "{\"bool\":true}";
    ASSERT_THROW(
        {
          boost::archive::json_iarchive ia{ss};
          bool loaded_o;
          ia >> boost::make_nvp("not_bool", loaded_o);
        },
        std::runtime_error);
  }
  {
    std::stringstream ss;
    ss << "{\"bool_vec\":[true,false,true,false]}";
    ASSERT_THROW(
        {
          boost::archive::json_iarchive ia{ss};
          bool loaded_o;
          ia >> boost::make_nvp("not_bool_vec", loaded_o);
        },
        std::runtime_error);
  }
}

template <typename T> void stdSerialize(std::string name, T value) {
  std::stringstream ss;
  T val = value;
  {
    boost::archive::json_oarchive oa{ss, true};
    oa << boost::make_nvp(name.c_str(), val);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::stringstream sss;
  boost::json::object o;
  o[name] = val;
  sss << o;
  GTEST_COUT << sss.str() << GTEST_ENDL;

  EXPECT_EQ(ss.str(), sss.str());

  std::istringstream iss(ss.str().c_str());

  T loaded_o;
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp(name.c_str(), loaded_o);

  EXPECT_EQ(val, loaded_o);
}

TEST_F(BoostSerializationJsonTest, Serialize_StdBool) { stdSerialize<bool>("bool", true); }

TEST_F(BoostSerializationJsonTest, Serialize_StdBoolOldArray) {
  bool val[] = {true, false, true};
  std::stringstream ss;
  {
    boost::archive::json_oarchive oa{ss, true};
    oa << boost::make_nvp("bool_c_array", val);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::istringstream iss(ss.str().c_str());

  bool loaded_o[3] = {false, true, false};

  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("bool_c_array", loaded_o);

  EXPECT_TRUE(memcmp(val, loaded_o, sizeof(bool[3])) == 0);
}

TEST_F(BoostSerializationJsonTest, Serialize_StdInt) { stdSerialize<int>("int", -66); }

TEST_F(BoostSerializationJsonTest, Serialize_StdUIntLowerThanIntMax) { stdSerialize<uint64_t>("uint", uint64_t(66)); }

TEST_F(BoostSerializationJsonTest, Serialize_StdUInt) { stdSerialize<uint64_t>("uint", uint64_t(ULONG_MAX - 1)); }

TEST_F(BoostSerializationJsonTest, Serialize_StdDouble) { stdSerialize<double>("double", -666.6); }

TEST_F(BoostSerializationJsonTest, Serialize_StdString) { stdSerialize<std::string>("string", "Test !!!"); }

template <typename T> void stdVectorSerialize(std::string name, std::vector<T> value) {
  std::stringstream ss;
  std::vector<T> &val = value;
  {
    boost::archive::json_oarchive oa{ss, true};
    oa << boost::make_nvp(name.c_str(), val);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::vector<T> loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp(name.c_str(), loaded_o);

  if constexpr (std::is_pointer<T>::value || boost::archive::detail::is_shared_ptr<T>::value ||
                boost::archive::detail::is_unique_ptr<T>::value) {
    ASSERT_EQ(val.size(), loaded_o.size());
    for (unsigned int i = 0; i < val.size(); i++) {
      EXPECT_EQ(*val[i], *loaded_o[i]);
      if constexpr (!boost::archive::detail::is_shared_ptr<T>::value && !boost::archive::detail::is_unique_ptr<T>::value) {
        delete loaded_o[i];
      }
    }
  } else {
    EXPECT_EQ(val, loaded_o);
  }
}

TEST_F(BoostSerializationJsonTest, Serialize_StdBoolVector) { stdVectorSerialize<bool>("bool_vec", {true, false, true, false}); }

TEST_F(BoostSerializationJsonTest, Serialize_StdBigBoolVector) {
  std::vector<bool> v(1000);
  std::generate(v.begin(), v.end(), []() { return (std::rand() % 2) == 0; });
  stdVectorSerialize<bool>("bool_vec", v);
}

TEST_F(BoostSerializationJsonTest, Serialize_StdIntVector) { stdVectorSerialize<int>("int_vec", {0, 1, 2, 3}); }

TEST_F(BoostSerializationJsonTest, Serialize_StdUIntVector) {
  stdVectorSerialize<uint64_t>("uint_vec", {ULONG_MAX, ULONG_MAX - 1, ULONG_MAX - 2, ULONG_MAX - 3});
}

TEST_F(BoostSerializationJsonTest, Serialize_StdDoubleVector) {
  stdVectorSerialize<double>("double_vec", {-123.345, 234.35, 324234.2342, 344.3334});
}

TEST_F(BoostSerializationJsonTest, Serialize_StdStringVector) {
  stdVectorSerialize<std::string>("string_vec", {"true", "false", "true", "false"});
}

template <typename T, int size> void stdArraySerialize(std::string name, std::array<T, size> value) {
  std::stringstream ss;
  std::array<T, size> &val = value;
  {
    boost::archive::json_oarchive oa{ss, true};
    oa << boost::make_nvp(name.c_str(), val);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::array<T, size> loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp(name.c_str(), loaded_o);

  if constexpr (std::is_pointer<T>::value || boost::archive::detail::is_shared_ptr<T>::value) {
    ASSERT_EQ(val.size(), loaded_o.size());
    for (unsigned int i = 0; i < val.size(); i++) {
      EXPECT_EQ(*val[i], *loaded_o[i]);
      if constexpr (!boost::archive::detail::is_shared_ptr<T>::value && !boost::archive::detail::is_unique_ptr<T>::value) {
        delete loaded_o[i];
      }
    }
  } else {
    EXPECT_EQ(val, loaded_o);
  }
}

TEST_F(BoostSerializationJsonTest, Serialize_StdBoolArray) {
  stdArraySerialize<bool, 4>("bool_array", {true, false, true, false});
}

TEST_F(BoostSerializationJsonTest, Serialize_StdIntArray) { stdArraySerialize<int, 4>("int_array", {0, 1, 2, 3}); }

TEST_F(BoostSerializationJsonTest, Serialize_StdUIntArray) {
  stdArraySerialize<uint64_t, 4>("uint_array", {ULONG_MAX, ULONG_MAX - 1, ULONG_MAX - 2, ULONG_MAX - 3});
}

TEST_F(BoostSerializationJsonTest, Serialize_StdDoubleArray) {
  stdArraySerialize<double, 4>("double_array", {-123.345, 234.35, 324234.2342, 344.3334});
}

TEST_F(BoostSerializationJsonTest, Serialize_StdStringArray) {
  stdArraySerialize<std::string, 4>("string_array", {"true", "false", "true", "false"});
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassWithBools) {
  std::stringstream ss;
  BoolsObject booboo("booboo", true);
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  BoolsObject loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("BooBoo", loaded_o);

  EXPECT_TRUE(loaded_o.get_a());
  EXPECT_FALSE(loaded_o.get_b());
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassWithBoolsVector) {
  stdVectorSerialize<BoolsObject>("Booboo_vec", {BoolsObject("booboo_0", true, false), BoolsObject("booboo_1", false),
                                                 BoolsObject("booboo_2", true), BoolsObject("booboo_3", false)});
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassWithBoolsArray) {
  stdArraySerialize<BoolsObject, 4>("Booboo_array", {BoolsObject("booboo_0", true, false), BoolsObject("booboo_1", false),
                                                     BoolsObject("booboo_2", true), BoolsObject("booboo_3", false)});
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassPtrWithBools) {
  std::stringstream ss;
  BoolsObject *booboo = new BoolsObject("boobooPtr", true);
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;
  delete booboo;

  BoolsObject *o = nullptr;
  boost::archive::json_iarchive ia{ss};
  ia >> boost::make_nvp("BooBoo", o);

  GTEST_COUT << o << GTEST_ENDL;
  EXPECT_TRUE(o->get_a());
  EXPECT_FALSE(o->get_b());
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassWithBoolsPtrVector) {
  std::vector<BoolsObject *> vec = {new BoolsObject("booboo_0", true, false), new BoolsObject("booboo_1", false),
                                    new BoolsObject("booboo_2", true), new BoolsObject("booboo_3", false)};
  stdVectorSerialize<BoolsObject *>("Booboo_vec", vec);

  for (auto ptr : vec) {
    delete ptr;
  }
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassWithBoolsPtrArray) {
  std::array<BoolsObject *, 4> array = {new BoolsObject("booboo_0", true, false), new BoolsObject("booboo_1", false),
                                        new BoolsObject("booboo_2", true), new BoolsObject("booboo_3", false)};
  stdArraySerialize<BoolsObject *, 4>("Booboo_array", array);

  for (auto ptr : array) {
    delete ptr;
  }
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassSptrWithBools) {
  std::stringstream ss;
  auto booboo = std::make_shared<BoolsObject>("boobooSptr", true);
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::shared_ptr<BoolsObject> o = nullptr;
  boost::archive::json_iarchive ia{ss};
  ia >> boost::make_nvp("BooBoo", o);
  GTEST_COUT << o << GTEST_ENDL;
  EXPECT_TRUE(o->get_a());
  EXPECT_FALSE(o->get_b());
}

static auto static_booboo = std::make_shared<BoolsObject>("boobooSptr", true);
TEST_F(BoostSerializationJsonTest, Serialize_StaticSptrWithBools) {
  std::stringstream ss;
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", static_booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::shared_ptr<BoolsObject> o = nullptr;
  boost::archive::json_iarchive ia{ss};
  ia >> boost::make_nvp("BooBoo", o);
  GTEST_COUT << o << GTEST_ENDL;
  EXPECT_TRUE(o->get_a());
  EXPECT_FALSE(o->get_b());
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassWithBoolsSptrVector) {
  std::vector<std::shared_ptr<BoolsObject>> vec = {
      std::make_shared<BoolsObject>("booboo_0", true, false), std::make_shared<BoolsObject>("booboo_1", false),
      std::make_shared<BoolsObject>("booboo_2", true), std::make_shared<BoolsObject>("booboo_3", false)};
  stdVectorSerialize<std::shared_ptr<BoolsObject>>("Booboo_vec", vec);
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassWithBoolsUptrVector) {
  std::vector<std::unique_ptr<BoolsObject>> vec;
  vec.push_back(std::make_unique<BoolsObject>("booboo_0", true, false));
  vec.push_back(std::make_unique<BoolsObject>("booboo_1", false));
  vec.push_back(std::make_unique<BoolsObject>("booboo_2", true));
  vec.push_back(std::make_unique<BoolsObject>("booboo_3", false));
  stdVectorSerialize<std::unique_ptr<BoolsObject>>("Booboo_vec", std::move(vec));
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassWithBoolsSptrArray) {
  std::array<std::shared_ptr<BoolsObject>, 4> array = {
      std::make_shared<BoolsObject>("booboo_0", true, false), std::make_shared<BoolsObject>("booboo_1", false),
      std::make_shared<BoolsObject>("booboo_2", true), std::make_shared<BoolsObject>("booboo_3", false)};
  stdArraySerialize<std::shared_ptr<BoolsObject>, 4>("Booboo_array", array);
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassPolymorphPtrWithBools) {
  std::stringstream ss;
  Object *booboo = new BoolsObject("boobooPtr", true);
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;
  delete booboo;

  Object *o = nullptr;
  boost::archive::json_iarchive ia{ss};
  ia >> boost::make_nvp("BooBoo", o);

  GTEST_COUT << dynamic_cast<BoolsObject *>(o) << GTEST_ENDL;
  EXPECT_TRUE(dynamic_cast<BoolsObject *>(o)->get_a());
  EXPECT_FALSE(dynamic_cast<BoolsObject *>(o)->get_b());
  delete o;
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassPolymorphWithBoolsPtrVector) {
  std::vector<Object *> vec = {new BoolsObject("booboo_0", true, false), new BoolsObject("booboo_1", false),
                               new BoolsObject("booboo_2", true), new BoolsObject("booboo_3", false)};
  stdVectorSerialize<Object *>("Booboo_vec", vec);

  for (auto ptr : vec) {
    delete ptr;
  }
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassPolymorphWithBoolsPtrArray) {
  std::array<Object *, 4> array = {new BoolsObject("booboo_0", true, false), new BoolsObject("booboo_1", false),
                                   new BoolsObject("booboo_2", true), new BoolsObject("booboo_3", false)};
  stdArraySerialize<Object *, 4>("Booboo_rray", array);

  for (auto ptr : array) {
    delete ptr;
  }
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassNestedBoolsObjects) {
  std::stringstream ss;
  NestedBoolObjects booboo(true, false, true);
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  NestedBoolObjects loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("BooBoo", loaded_o);

  EXPECT_TRUE(loaded_o.get_a());
  EXPECT_FALSE(loaded_o.get_b());
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassSptrNestedBoolsObjects) {
  std::stringstream ss;
  auto booboo = std::make_shared<NestedBoolObjects>(true, false, true);
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::shared_ptr<NestedBoolObjects> loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("BooBoo", loaded_o);

  EXPECT_TRUE(loaded_o->get_a());
  EXPECT_FALSE(loaded_o->get_b());
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassSptrNestedBoolsObjectsPtr) {
  std::stringstream ss;
  auto booboo = std::make_shared<NestedBoolObjectsPtrs>();
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::shared_ptr<NestedBoolObjectsPtrs> loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("BooBoo", loaded_o);

  EXPECT_TRUE(loaded_o->get_a());
  EXPECT_FALSE(loaded_o->get_b());
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassNestedBoolsObjectsVector) {
  stdVectorSerialize<NestedBoolObjects>("Booboo_vector",
                                        {NestedBoolObjects(), NestedBoolObjects(), NestedBoolObjects(), NestedBoolObjects()});
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassNestedBoolsObjectsArray) {
  stdArraySerialize<NestedBoolObjects, 4>("Booboo_array",
                                          {NestedBoolObjects(), NestedBoolObjects(), NestedBoolObjects(), NestedBoolObjects()});
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassNestedBoolObjectsPtrs) {
  std::stringstream ss;
  NestedBoolObjectsPtrs booboo;
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  NestedBoolObjectsPtrs loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("BooBoo", loaded_o);

  EXPECT_TRUE(loaded_o.get_a());
  EXPECT_FALSE(loaded_o.get_b());
  EXPECT_TRUE(loaded_o.get_c());
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassNestedBoolsObjectsPtrsVector) {
  stdVectorSerialize<NestedBoolObjectsPtrs>("Booboo_vector",
                                            {NestedBoolObjectsPtrs(false, false, false), NestedBoolObjectsPtrs(false, false),
                                             NestedBoolObjectsPtrs(false), NestedBoolObjectsPtrs(false, true)});
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassNestedBoolsObjectsPtrsArray) {
  stdArraySerialize<NestedBoolObjectsPtrs, 4>(
      "Booboo_array", {NestedBoolObjectsPtrs(), NestedBoolObjectsPtrs(), NestedBoolObjectsPtrs(), NestedBoolObjectsPtrs()});
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassNestedPolymorphObjectsPtrs) {
  std::stringstream ss;
  NestedObjectsPtrs booboo;
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  NestedObjectsPtrs loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("BooBoo", loaded_o);

  EXPECT_TRUE(loaded_o.get_a());
  EXPECT_FALSE(loaded_o.get_b());
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassNestedObjectsPtrsVector) {
  stdVectorSerialize<NestedObjectsPtrs>("Booboo_vector",
                                        {NestedObjectsPtrs(), NestedObjectsPtrs(), NestedObjectsPtrs(), NestedObjectsPtrs()});
}

TEST_F(BoostSerializationJsonTest, NestedObjectsPtrsSptrVector) {
  stdVectorSerialize<std::shared_ptr<NestedObjectsPtrs>>(
      "Booboo_sptr_vector",
      {std::make_shared<NestedObjectsPtrs>(true, false, true), std::make_shared<NestedObjectsPtrs>(false, false),
       std::make_shared<NestedObjectsPtrs>(false, true, false), std::make_shared<NestedObjectsPtrs>()});
}

TEST_F(BoostSerializationJsonTest, NestedObjectsPtrsSameSptrVector) {
  auto sptr = std::make_shared<NestedObjectsPtrs>();
  stdVectorSerialize<std::shared_ptr<NestedObjectsPtrs>>("Booboo_sptr_vector", {sptr, sptr, sptr, sptr});
}

static auto sptr_1 = std::make_shared<NestedObjectsPtrs>(true, false, true);
static auto sptr_2 = std::make_shared<NestedObjectsPtrs>(false, true, false);
TEST_F(BoostSerializationJsonTest, NestedObjectsPtrsSameStaticSptrVector) {
  stdVectorSerialize<std::shared_ptr<NestedObjectsPtrs>>("Booboo_static_sptr_vector", {sptr_1, sptr_2, sptr_1, sptr_2});
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassNestedObjectsPtrsArray) {
  stdArraySerialize<NestedObjectsPtrs, 4>("Booboo_array",
                                          {NestedObjectsPtrs(), NestedObjectsPtrs(), NestedObjectsPtrs(), NestedObjectsPtrs()});
}

template <typename Key, typename Value, typename MapType = std::map<Key, Value>>
void stdMapSerialize(std::string name, MapType value) {
  std::stringstream ss;
  MapType val = value;
  std::cout << boost::archive::detail::is_std_map<std::map<Key, Value>>::value << std::endl;
  {
    boost::archive::json_oarchive oa{ss, true};
    oa << boost::make_nvp(name.c_str(), val);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::map<Key, Value> loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp(name.c_str(), loaded_o);

  ASSERT_EQ(val.size(), loaded_o.size());
  if constexpr (std::is_pointer<Value>::value || boost::archive::detail::is_shared_ptr<Value>::value) {
    for (auto &p : value) {
      EXPECT_EQ(*(p.second), *(loaded_o[p.first]));
    }
  } else {
    for (auto &p : value) {
      EXPECT_EQ((p.second), (loaded_o[p.first]));
    }
  }
}

TEST_F(BoostSerializationJsonTest, Serialize_StdMap_String_Bool) {
  stdMapSerialize<std::string, bool>("string_bool_map", {{"Key_0", true}, {"Key_1", false}, {"Key_2", true}, {"Key_3", false}});
}

TEST_F(BoostSerializationJsonTest, Serialize_StdMap_String_Int) {
  stdMapSerialize<std::string, int>("string_int_map", {{"Key_0", 0}, {"Key_1", 1}, {"Key_2", 2}, {"Key_3", 3}});
}

TEST_F(BoostSerializationJsonTest, Serialize_StdMap_Int_String) {
  stdMapSerialize<int, std::string>("int_string_map", {{0, "Key_0"}, {1, "Key_1"}, {2, "Key_2"}, {3, "Key_3"}});
}

TEST_F(BoostSerializationJsonTest, Serialize_StdMap_String_BoolsObject) {
  stdMapSerialize<std::string, BoolsObject>("string_booboo_map", {{"Key_0", BoolsObject("b1", false, false, false)},
                                                                  {"Key_1", BoolsObject("b2", false, false, true)},
                                                                  {"Key_2", BoolsObject("b3", false, true, false)},
                                                                  {"Key_3", BoolsObject("b4", true, false, false)}});
}

TEST_F(BoostSerializationJsonTest, Serialize_StdMap_String_BoolsObjectSptrs) {
  stdMapSerialize<std::string, std::shared_ptr<BoolsObject>>(
      "string_booboo_ptrs_map", {{"Key_0", std::make_shared<BoolsObject>("b1", false, false, false)},
                                 {"Key_1", std::make_shared<BoolsObject>("b2", false, false, true)},
                                 {"Key_2", std::make_shared<BoolsObject>("b3", false, true, false)},
                                 {"Key_3", std::make_shared<BoolsObject>("b4", true, false, false)}});
}

TEST_F(BoostSerializationJsonTest, Serialize_ClassWithObjectPtrsList) {
  std::stringstream ss;
  ObjectsPtrsList booboo;
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  ObjectsPtrsList loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("BooBoo", loaded_o);

  EXPECT_TRUE(loaded_o.get_a());
  EXPECT_FALSE(loaded_o.get_b());
}

TEST_F(BoostSerializationJsonTest, Serialize_SptrClassWithUIntList) {
  std::stringstream ss;
  std::shared_ptr<ObjectWithUIntList> booboo = std::make_shared<ObjectWithUIntList>(std::vector<uint32_t>({4, 5, 6, 7}));
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("ObjectWithUIntList", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::shared_ptr<ObjectWithUIntList> loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("ObjectWithUIntList", loaded_o);

  EXPECT_EQ(booboo->get(), loaded_o->get());
}

TEST_F(BoostSerializationJsonTest, Serialize_SptrClassWithObjectPtrsList) {
  std::stringstream ss;
  std::shared_ptr<ObjectsPtrsList> booboo = std::make_shared<ObjectsPtrsList>();
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("BooBoo", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::shared_ptr<ObjectsPtrsList> loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("BooBoo", loaded_o);

  EXPECT_TRUE(loaded_o->get_a());
  EXPECT_FALSE(loaded_o->get_b());
}

TEST_F(BoostSerializationJsonTest, Serialize_ObjectWithEnum) {
  std::stringstream ss;
  ObjectWithEnum nunum(ObjectWithEnum::THREE);
  EXPECT_EQ(nunum.get_enum(), ObjectWithEnum::THREE);
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("MyObjWithEnum", nunum);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  ObjectWithEnum loaded_o;
  EXPECT_EQ(loaded_o.get_enum(), ObjectWithEnum::ONE);
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("MyObjWithEnum", loaded_o);

  EXPECT_EQ(nunum.get_enum(), loaded_o.get_enum());
}

TEST_F(BoostSerializationJsonTest, Serialize_ObjectSptrWithEnum) {
  std::stringstream ss;
  std::shared_ptr<ObjectWithEnum> nunum = std::make_shared<ObjectWithEnum>(ObjectWithEnum::THREE);
  EXPECT_EQ(nunum->get_enum(), ObjectWithEnum::THREE);
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("MyObjPtrWithEnum", nunum);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::shared_ptr<ObjectWithEnum> loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("MyObjPtrWithEnum", loaded_o);

  EXPECT_EQ(nunum->get_enum(), loaded_o->get_enum());
}

TEST_F(BoostSerializationJsonTest, Serialize_ObjectSptrWithStruct) {
  std::stringstream ss;
  std::shared_ptr<ObjectWithStruct> booboo = std::make_shared<ObjectWithStruct>(TestStruct({4, 5, 6, 7}));
  {
    boost::archive::json_oarchive oa{ss, 0, true};
    oa << boost::make_nvp("ObjectWithStruct", booboo);
  }
  GTEST_COUT << ss.str() << GTEST_ENDL;

  std::shared_ptr<ObjectWithStruct> loaded_o;
  std::istringstream iss(ss.str().c_str());
  boost::archive::json_iarchive ia{iss};
  ia >> boost::make_nvp("ObjectWithStruct", loaded_o);

  EXPECT_EQ(booboo->get_struct(), loaded_o->get_struct());
}

// TEST_F(BoostSerializationJsonTest, Serialize_WeakPtrObject) { FAIL(); }
