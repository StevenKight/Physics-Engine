#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace state;

std::vector<Object> io::parse_phys_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    std::vector<Object> objects;
    Object current;

    bool has_center = false;
    bool has_scale = false;
    bool has_mass = false;
    bool has_velocity = false;

    std::string line;
    while (std::getline(file, line)) {
        // Remove comments and skip empty lines
        auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) line = line.substr(0, comment_pos);
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string directive;
        iss >> directive;

        if (directive == "c") {
            iss >> current.center.x >> current.center.y >> current.center.z;
            has_center = true;
        } else if (directive == "s") {
            iss >> current.scale.x >> current.scale.y >> current.scale.z;
            has_scale = true;
        } else if (directive == "m") {
            iss >> current.mass;
            has_mass = true;
        } else if (directive == "iv") {
            iss >> current.velocity.x >> current.velocity.y >> current.velocity.z;
            has_velocity = true;
        } else if (directive == "av") {
            iss >> current.acceleration.x >> current.acceleration.y >> current.acceleration.z;
        }

        if (has_center && has_scale && has_mass && has_velocity) {
            objects.push_back(current);
            current = Object{}; // Reset
            has_center = has_scale = has_mass = has_velocity = false;
        }
    }

    return objects;
}
