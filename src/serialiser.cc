#include "serialiser.hh"
#include "planet.hh"
#include <iostream>
#include <string>

namespace YAML {
	template<>
	struct convert<Color> {
		static bool decode(const Node &node, Color &c) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			c.r = node[0].as<unsigned char>();
			c.g = node[1].as<unsigned char>();
			c.b = node[2].as<unsigned char>();
            c.a = node[3].as<unsigned char>();
			return true;
		}
	};

	template<>
	struct convert<Vector2> {
		static bool decode(const Node &node, Vector2 &v) {
			if (!node.IsSequence() || node.size() != 2)
				return false;

			v.x = node[0].as<float>();
			v.y = node[1].as<float>();
			return true;
		}
	};
}

using namespace std;

std::vector<CelestialBody> Deserialize(const string &path) {
    YAML::Node data = YAML::LoadFile(path);

    std::vector<CelestialBody> bodies{};

    if (data) {
        for (auto planet : data) {
            CelestialBody body{};

            body.position = planet["position"].as<Vector2>();
            body.velocity = planet["velocity"].as<Vector2>();
            body.color = planet["color"].as<Color>();
            body.radius = planet["radius"].as<float>();
            body.mass = planet["mass"].as<float>();
			
			if (planet["type"]) {
				auto type = planet["type"].as<string>();
				if (type == "star") body.type = CelestialBody::Type::Star;
				else if (type == "planet") body.type = CelestialBody::Type::Planet;
				else if (type == "moon") body.type = CelestialBody::Type::Moon;
				else if (type == "comet") body.type = CelestialBody::Type::Comet;
				else body.type = CelestialBody::Type::Planet;
			}
            
            bodies.push_back(body);
        }
    }

    return bodies;
}