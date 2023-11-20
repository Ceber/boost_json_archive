//  Compile with:
// g++ -std=c++17 ... -lboost_serialization -lboost_container -lboost_json -lboost_json_archive
//  (inspired from: https://www.boost.org/doc/libs/1_80_0/libs/serialization/doc/tutorial.html )

#include <fstream>
#include <iostream>

#include <boost/serialization/access.hpp>
#ifdef TEXT
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#define SERIALIZE(X) X
#else
#include <boost/archive/json_iarchive.hpp>
#include <boost/archive/json_oarchive.hpp>
#define SERIALIZE(X) BOOST_SERIALIZATION_NVP(X)
#endif

class gps_position {
private:
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
    ar &SERIALIZE(degrees);
    ar &SERIALIZE(minutes);
    ar &SERIALIZE(seconds);
  }
  int degrees;
  int minutes;
  float seconds;

public:
  gps_position(){};
  gps_position(int deg, int min, float sec) : degrees(deg), minutes(min), seconds(sec) {}
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <file>.";
#ifdef TEXT
    std::cout << "txt" << std::endl;
#else
    std::cout << "json" << std::endl;
#endif
    return EXIT_SUCCESS;
  }

  try {
    std::ofstream ofs(argv[1]);
    const gps_position g(32, 75, 22.567f);
#ifdef TEXT
    boost::archive::text_oarchive oa(ofs);
#else
    boost::archive::json_oarchive oa(ofs);
#endif
    oa << SERIALIZE(g);
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
