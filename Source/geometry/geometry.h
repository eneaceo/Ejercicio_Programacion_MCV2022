#pragma once

#include "SimpleMath.h"

using MAT44 = DirectX::SimpleMath::Matrix;
using QUAT = DirectX::SimpleMath::Quaternion;
using VEC2 = DirectX::SimpleMath::Vector2;
using VEC3 = DirectX::SimpleMath::Vector3;
using VEC4 = DirectX::SimpleMath::Vector4;
using Color = VEC4;
using AABB = DirectX::BoundingBox;

VEC2 loadVEC2(const std::string& str);
VEC2 loadVEC2(const json& j, const char* attr);
VEC2 loadVEC2(const json& j, const char* attr, const VEC2& defaultValue);
VEC3 loadVEC3(const std::string& str);
VEC3 loadVEC3(const json& j, const char* attr);
VEC4 loadVEC4(const std::string& str);
VEC4 loadVEC4(const json& j, const char* attr);
QUAT loadQUAT(const json& j, const char* attr);
Color loadColor(const json& j);
Color loadColor(const json& j, const char* attr);
Color loadColor(const json& j, const char* attr, const Color& defaultValue);

#include "angular.h"
#include "camera.h"
#include "transform.h"
#include "interpolators.h"

