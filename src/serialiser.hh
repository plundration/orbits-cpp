#pragma once

#include <string>
#include "planet.hh"
#include <vector>
#include <yaml-cpp/yaml.h>
#include <raylib.h>

namespace YAML { class Emitter; }

std::vector<CelestialBody> Deserialize(const std::string &path);