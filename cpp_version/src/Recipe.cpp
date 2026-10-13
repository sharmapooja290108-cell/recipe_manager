#include "../include/Recipe.hpp"
#include <algorithm>
#include <iomanip>

namespace Culinary {

static std::string toLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return str;
}

bool Recipe::matchesSearch(const std::string& query) const {
    if (query.empty()) return true;
    std::string qLower = toLower(query);
    return toLower(title_).find(qLower) != std::string::npos ||
           toLower(category_).find(qLower) != std::string::npos;
}

bool Recipe::hasIngredient(const std::string& ingName) const {
    if (ingName.empty()) return true;
    std::string qLower = toLower(ingName);
    for (const auto& ing : ingredients_) {
        if (toLower(ing.name).find(qLower) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void Recipe::displayCard(int targetServings) const {
    double scale = 1.0;
    if (targetServings > 0 && servings_ > 0) {
        scale = static_cast<double>(targetServings) / static_cast<double>(servings_);
    } else {
        targetServings = servings_;
    }

    std::cout << "\n\033[36m======================================================================\033[0m\n";
    std::cout << "\033[1;33m  📖 " << title_ << " (#" << id_ << ")\033[0m\n";
    std::cout << "\033[36m======================================================================\033[0m\n";

    std::cout << "\033[1mCategory:   \033[0m" << std::left << std::setw(18) << category_
              << "\033[1mDifficulty: \033[0m" << difficulty_ << "\n";
    std::cout << "\033[1mPrep Time:  \033[0m" << std::left << std::setw(18) << (std::to_string(prepTimeMin_) + " min")
              << "\033[1mCook Time:  \033[0m" << cookTimeMin_ << " min (Total: " << getTotalTime() << " min)\n";

    if (scale != 1.0) {
        std::cout << "\033[1mServings:   \033[0m" << targetServings
                  << " \033[35m(Scaled dynamically from original " << servings_
                  << " servings, factor: " << std::fixed << std::setprecision(2) << scale << "x)\033[0m\n";
    } else {
        std::cout << "\033[1mServings:   \033[0m" << servings_ << "\n";
    }

    std::cout << "\n\033[1;36m--- 🥗 Ingredients (" << ingredients_.size() << " items) ---\033[0m\n";
    for (const auto& ing : ingredients_) {
        auto scaledIng = ing.scaled(scale);
        std::cout << "  • " << std::left << std::setw(32) << scaledIng.name << " : ";
        if (scaledIng.quantity == static_cast<int>(scaledIng.quantity)) {
            std::cout << static_cast<int>(scaledIng.quantity);
        } else {
            std::cout << std::fixed << std::setprecision(2) << scaledIng.quantity;
        }
        std::cout << " " << scaledIng.unit << "\n";
    }

    std::cout << "\n\033[1;36m--- 📝 Preparation Steps (" << steps_.size() << " steps) ---\033[0m\n";
    for (size_t i = 0; i < steps_.size(); ++i) {
        std::cout << "  \033[1m" << std::setw(2) << (i + 1) << ".\033[0m " << steps_[i] << "\n";
    }
    std::cout << "\033[36m======================================================================\033[0m\n\n";
}

} // namespace Culinary
