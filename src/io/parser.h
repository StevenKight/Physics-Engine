#pragma once

#include <string>
#include <vector>

#include <object.h>

namespace io {

std::vector<state::Object> parse_phys_file(const std::string &filename);

}
