#pragma once

#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>

class Object {
protected:
  std::string name = "ObjectName";
  int test = 1;

public:
  virtual ~Object() = default;

  virtual std::string getName() { return name; }

  template <typename ArchiveT> void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(test);
  }

  virtual bool operator==(const Object &rhs) const { return (name == rhs.name); }
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Object)
BOOST_CLASS_EXPORT_KEY(Object)

class BoolsObject : public Object {

private:
  std::string name = "";
  bool a = false;
  bool b = false;
  bool c = false;

public:
  BoolsObject(std::string name = "", bool a = false, bool b = false, bool c = false) : name(name), a(a), b(b), c(c) {}
  virtual ~BoolsObject() = default;

  bool get_a() const { return a; }
  bool get_b() const { return b; }
  bool get_c() const { return c; }

  template <typename ArchiveT> inline void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(test);
    ar &BOOST_SERIALIZATION_NVP(name);
    ar &BOOST_SERIALIZATION_NVP(a);
    ar &BOOST_SERIALIZATION_NVP(b);
    ar &BOOST_SERIALIZATION_NVP(c);
  }

  bool operator==(const BoolsObject &rhs) const {
    return Object::operator==(rhs) && (a == rhs.a) && (b == rhs.b) && (c == rhs.c);
  }
};

BOOST_CLASS_EXPORT_KEY(BoolsObject)

class NestedBoolObjects {
private:
  BoolsObject ba{"ba", true};
  BoolsObject bb;
  BoolsObject bc;

public:
  NestedBoolObjects(bool a = true, bool b = true, bool c = true) : ba("ba", a, a, a), bb("bb", b, b, b), bc("bc", c, c, c) {}

  bool get_a() const { return ba.get_a(); }
  bool get_b() const { return bb.get_b(); }
  bool get_c() const { return bc.get_c(); }

  template <typename ArchiveT> inline void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(ba);
    ar &BOOST_SERIALIZATION_NVP(bb);
    ar &BOOST_SERIALIZATION_NVP(bc);
  }

  bool operator==(const NestedBoolObjects &rhs) const { return (ba == rhs.ba) && (bb == rhs.bb) && (bc == rhs.bc); }
};

BOOST_CLASS_EXPORT_KEY(NestedBoolObjects)

class NestedBoolObjectsPtrs {
private:
  std::shared_ptr<BoolsObject> ba = nullptr;
  std::shared_ptr<BoolsObject> bb = nullptr;
  std::shared_ptr<BoolsObject> bc = nullptr;

public:
  NestedBoolObjectsPtrs(bool a = true, bool b = false, bool c = true)
      : ba(new BoolsObject{"ba", a, !a, a}), bb(new BoolsObject("bb", b, b, b)), bc(new BoolsObject{"bc", c, false, true}) {}

  ~NestedBoolObjectsPtrs() = default;

  bool get_a() const { return std::dynamic_pointer_cast<BoolsObject>(ba)->get_a(); }
  bool get_b() const { return std::dynamic_pointer_cast<BoolsObject>(bb)->get_b(); }
  bool get_c() const { return std::dynamic_pointer_cast<BoolsObject>(bc)->get_c(); }

  template <typename ArchiveT> inline void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(ba);
    ar &BOOST_SERIALIZATION_NVP(bb);
    ar &BOOST_SERIALIZATION_NVP(bc);
  }

  bool operator==(const NestedBoolObjectsPtrs &rhs) const {
    return ((ba == rhs.ba) || (ba && !rhs.ba) || (!ba && rhs.ba) || *ba == *rhs.ba) &&
           ((bb == rhs.bb) || (bb && !rhs.bb) || (!bb && rhs.bb) || *bb == *rhs.bb) &&
           ((bc == rhs.bc) || (bc && !rhs.bc) || (!bc && rhs.bc) || *bc == *rhs.bc);
  }
};

BOOST_CLASS_EXPORT_KEY(NestedBoolObjectsPtrs)

class NestedObjectsPtrs {
private:
  std::shared_ptr<Object> ba = nullptr;
  std::shared_ptr<Object> bb = nullptr;
  std::shared_ptr<Object> bc = nullptr;

public:
  NestedObjectsPtrs(bool a = true, bool b = true, bool c = true)
      : ba(new BoolsObject{"ba", a}), bb(new BoolsObject("bb", b)), bc(new BoolsObject("bc", c)) {}

  ~NestedObjectsPtrs() = default;

  bool get_a() const { return std::dynamic_pointer_cast<BoolsObject>(ba)->get_a(); }
  bool get_b() const { return std::dynamic_pointer_cast<BoolsObject>(bb)->get_b(); }
  bool get_c() const { return std::dynamic_pointer_cast<BoolsObject>(bc)->get_c(); }

  template <typename ArchiveT> inline void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(ba);
    ar &BOOST_SERIALIZATION_NVP(bb);
    ar &BOOST_SERIALIZATION_NVP(bc);
  }

  bool operator==(const NestedObjectsPtrs &rhs) const {
    return ((ba == rhs.ba) || (ba && !rhs.ba) || (!ba && rhs.ba) || *ba == *rhs.ba) &&
           ((bb == rhs.bb) || (bb && !rhs.bb) || (!bb && rhs.bb) || *bb == *rhs.bb) &&
           ((bc == rhs.bc) || (bc && !rhs.bc) || (!bc && rhs.bc) || *bc == *rhs.bc);
  }
};

BOOST_CLASS_EXPORT_KEY(NestedObjectsPtrs)

class ObjectWithUIntList {
private:
  std::vector<uint32_t> m_list;

public:
  ObjectWithUIntList(std::vector<uint32_t> list = {1, 2, 3, 4}) : m_list(list) {}

  ~ObjectWithUIntList() = default;

  const std::vector<uint32_t> get() const { return m_list; }

  template <typename ArchiveT> inline void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(m_list);
  }

  bool operator==(const ObjectWithUIntList &rhs) const { return m_list == rhs.m_list; }
};

BOOST_CLASS_EXPORT_KEY(ObjectWithUIntList)

class ObjectsPtrsList {
private:
  std::vector<std::shared_ptr<Object>> m_list;

public:
  ObjectsPtrsList()
      : m_list({std::make_shared<BoolsObject>("ba", true), std::make_shared<BoolsObject>(), std::make_shared<BoolsObject>()}) {}

  ~ObjectsPtrsList() = default;

  bool get_a() const { return std::dynamic_pointer_cast<BoolsObject>(m_list.at(0))->get_a(); }
  bool get_b() const { return std::dynamic_pointer_cast<BoolsObject>(m_list.at(1))->get_b(); }
  bool get_c() const { return std::dynamic_pointer_cast<BoolsObject>(m_list.at(2))->get_c(); }

  template <typename ArchiveT> inline void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(m_list);
  }

  bool operator==(const NestedObjectsPtrs &rhs) const {
    return get_a() == rhs.get_a() && get_b() == rhs.get_b() && get_c() == rhs.get_c();
  }
};

BOOST_CLASS_EXPORT_KEY(ObjectsPtrsList)

class ObjectWithEnum {
public:
  enum MyEnum { ONE = 1, TWO = 2, THREE = 3, VIVA = 77 };

private:
  MyEnum m_enum;

public:
  ObjectWithEnum(MyEnum nunum = ONE) : m_enum(nunum) {}

  ~ObjectWithEnum() = default;

  MyEnum get_enum() const { return m_enum; }

  template <typename ArchiveT> inline void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(m_enum);
  }

  bool operator==(const ObjectWithEnum &rhs) const { return get_enum() == rhs.get_enum(); }
};

BOOST_CLASS_EXPORT_KEY(ObjectWithEnum)

struct TestStruct {
  int a, b, c, d;

  template <typename ArchiveT> inline void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(a);
    ar &BOOST_SERIALIZATION_NVP(b);
    ar &BOOST_SERIALIZATION_NVP(c);
    ar &BOOST_SERIALIZATION_NVP(d);
  }

  bool operator==(const TestStruct &rhs) const { return a == rhs.a && b == rhs.b && c == rhs.c && d == rhs.d; }
};

class ObjectWithStruct {
private:
  TestStruct m_struct;

public:
  ObjectWithStruct(TestStruct _struct = TestStruct({1, 2, 3, 4})) : m_struct(_struct) {}

  ~ObjectWithStruct() = default;

  const TestStruct &get_struct() const { return m_struct; }

  template <typename ArchiveT> inline void serialize(ArchiveT &ar, [[maybe_unused]] const unsigned int file_version) {
    ar &BOOST_SERIALIZATION_NVP(m_struct);
  }

  bool operator==(const ObjectWithStruct &rhs) const { return m_struct == rhs.m_struct; }
};

BOOST_CLASS_EXPORT_KEY(TestStruct)
BOOST_CLASS_EXPORT_KEY(ObjectWithStruct)
