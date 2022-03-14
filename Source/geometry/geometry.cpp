#include "mcv_platform.h"

VEC2 loadVEC2(const std::string& str) {
  VEC2 v;
  int n = sscanf(str.c_str(), "%f %f", &v.x, &v.y);
  if (n == 2) {
    return v;
  }
  fatal("Invalid str reading VEC2 %s. Only %d values read. Expected 2", str.c_str(), n);

  return VEC2::Zero;
}

VEC2 loadVEC2(const json& j, const char* attr) {

  assert(j.is_object());
  if (j.count(attr)) {
    const std::string& str = j.value(attr, "");
    return loadVEC2(str);
  }

  return VEC2::Zero;
}

VEC2 loadVEC2(const json& j, const char* attr, const VEC2& defaultValue) {
  if (j.count(attr) == 0) {
    return defaultValue;
  }

  VEC2 v = defaultValue;
  auto k = j.value(attr, "0 0");
  sscanf(k.c_str(), "%f %f", &v.x, &v.y);
  return v;
}

VEC3 loadVEC3(const std::string& str) {
  VEC3 v;
  int n = sscanf(str.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
  if (n == 3) {
    return v;
  }
  fatal("Invalid str reading VEC3 %s. Only %d values read. Expected 3", str.c_str(), n);

  return VEC3::Zero;
}

VEC3 loadVEC3(const json& j, const char* attr) {

  assert(j.is_object());
  if (j.count(attr)) {
    const std::string& str = j.value(attr, "");
    return loadVEC3(str);
  }

  return VEC3::Zero;
}

VEC4 loadVEC4(const std::string& str) {
  VEC4 v;
  int n = sscanf(str.c_str(), "%f %f %f %f", &v.x, &v.y, &v.z, &v.w);
  if (n == 4) {
    return v;
  }
  fatal("Invalid str reading VEC4 %s. Only %d values read. Expected 4", str.c_str(), n);

  return VEC4::Zero;
}

VEC4 loadVEC4(const json& j, const char* attr) {
  assert(j.is_object());
  if (j.count(attr)) {
    const std::string& str = j.value(attr, "");
    return loadVEC4(str);
  }

  return VEC4::Zero;
}

Color loadColor(const json& j) {
  Color v;
  const auto& str = j.get<std::string>();
  int n = sscanf(str.c_str(), "%f %f %f %f", &v.x, &v.y, &v.z, &v.w);
  if (n == 4)
    return v;
  fatal("Invalid str reading Color %s. Only %d values read. Expected 4\n", str.c_str(), n);
  return Colors::White;
}

Color loadColor(const json& j, const char* attr) {
  assert(j.is_object());
  if (j.count(attr))
    return loadColor( j[attr] );
  return Colors::White;
}

Color loadColor(const json& j, const char* attr, const Color& defaultValue) {
  if (j.count(attr) <= 0)
    return defaultValue;
  return loadColor(j, attr);
}

QUAT loadQUAT(const json& j, const char* attr) {
  assert(j.is_object());
  if (j.count(attr)) {
    const std::string& str = j.value(attr, "");
    QUAT q;
    int n = sscanf(str.c_str(), "%f %f %f %f", &q.x, &q.y, &q.z, &q.w);
    if (n == 4) {
      return q;
    }
    fatal("Invalid json reading QUAT attr %s. Only %d values read. Expected 4", attr, n);
  }

  return QUAT::Identity;
}
