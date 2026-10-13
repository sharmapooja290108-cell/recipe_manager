#pragma once

#include <string>
#include <sstream>
#include <iomanip>

namespace Culinary {

class Ingredient {
public:
    std::string name;
    double quantity;
    std::string unit;

    Ingredient() : name(""), quantity(0.0), unit("") {}
    Ingredient(std::string n, double q, std::string u)
        : name(std::move(n)), quantity(q), unit(std::move(u)) {}

    Ingredient scaled(double factor) const {
        return Ingredient(name, quantity * factor, unit);
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << name << " : ";
        if (quantity == static_cast<int>(quantity)) {
            oss << static_cast<int>(quantity);
        } else {
            oss << std::fixed << std::setprecision(2) << quantity;
        }
        if (!unit.empty()) {
            oss << " " << unit;
        }
        return oss.str();
    }
};

} // namespace Culinary
