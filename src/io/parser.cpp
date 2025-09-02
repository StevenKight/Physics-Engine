#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace state;

std::vector<Object> io::parse_phys_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    std::vector<Object> objects;
    Object current_object;
    bool object_in_progress = false;

    std::string line;
    while (std::getline(file, line)) {
        // Remove comments and trim whitespace
        auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::string directive;
        iss >> directive;

        if (directive == "c" || directive == "pos") {
            // 'c' or 'pos' directive starts a new object.
            // If one was already being parsed, save it before starting anew.
            if (object_in_progress) {
                objects.push_back(current_object);
            }
            // Reset to a default object for the new entry
            current_object = Object{};
            iss >> current_object.position.x >> current_object.position.y >>
                current_object.position.z;
            object_in_progress = true;
        } else if (directive == "s") {
            iss >> current_object.scale.x >> current_object.scale.y >>
                current_object.scale.z;
        } else if (directive == "m") {
            iss >> current_object.mass;
        } else if (directive == "iv") {
            iss >> current_object.velocity.x >> current_object.velocity.y >>
                current_object.velocity.z;
        } else if (directive == "rot") {
            iss >> current_object.orientation.w >>
                current_object.orientation.x >> current_object.orientation.y >>
                current_object.orientation.z;
        } else if (directive == "iav" || directive == "av") {
            iss >> current_object.angular_velocity.x >>
                current_object.angular_velocity.y >>
                current_object.angular_velocity.z;
        } else if (directive == "com") {
            iss >> current_object.center_of_mass.x >>
                current_object.center_of_mass.y >>
                current_object.center_of_mass.z;
        } else if (directive == "restitution") {
            iss >> current_object.restitution;
        } else if (directive == "friction") {
            iss >> current_object.friction;
        }
    }

    // Add the last object to the list if it exists
    if (object_in_progress) {
        objects.push_back(current_object);
    }

    return objects;
}