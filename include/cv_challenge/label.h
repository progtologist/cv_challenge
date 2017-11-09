#pragma once

#include <string>

namespace cv_challenge {

enum class Label {
  furniture = 0,
  floor = 1,
  walls = 2,
  ceiling = 2,
  female = 3,
  male = 4
};

inline std::string to_string(Label label) {
#define CASE(s) case Label::s: return #s
  switch (label) {
    CASE(furniture);
    CASE(floor);
    CASE(walls);
    CASE(female);
    CASE(male);
  }
#undef CASE
}

}
