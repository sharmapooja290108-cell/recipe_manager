#include "../include/RecipeBook.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <map>

namespace Culinary {

int RecipeBook::addRecipe(Recipe recipe) {
    recipe.setId(nextId_++);
    recipes_.push_back(std::move(recipe));
    return recipes_.back().getId();
}

bool RecipeBook::updateRecipe(int id, const Recipe& updated) {
    for (auto& r : recipes_) {
        if (r.getId() == id) {
            r = updated;
            r.setId(id);
            return true;
        }
    }
    return false;
}

bool RecipeBook::deleteRecipe(int id) {
    auto it = std::remove_if(recipes_.begin(), recipes_.end(), [id](const Recipe& r) {
        return r.getId() == id;
    });
    if (it != recipes_.end()) {
        recipes_.erase(it, recipes_.end());
        return true;
    }
    return false;
}

std::optional<Recipe> RecipeBook::getRecipeById(int id) const {
    for (const auto& r : recipes_) {
        if (r.getId() == id) {
            return r;
        }
    }
    return std::nullopt;
}

std::vector<Recipe> RecipeBook::searchByTitle(const std::string& query) const {
    std::vector<Recipe> result;
    for (const auto& r : recipes_) {
        if (r.matchesSearch(query)) {
            result.push_back(r);
        }
    }
    return result;
}

std::vector<Recipe> RecipeBook::filterByCategory(const std::string& category) const {
    std::vector<Recipe> result;
    for (const auto& r : recipes_) {
        if (r.getCategory() == category || r.matchesSearch(category)) {
            result.push_back(r);
        }
    }
    return result;
}

std::vector<Recipe> RecipeBook::filterByIngredient(const std::string& ingredient) const {
    std::vector<Recipe> result;
    for (const auto& r : recipes_) {
        if (r.hasIngredient(ingredient)) {
            result.push_back(r);
        }
    }
    return result;
}

std::vector<Recipe> RecipeBook::filterByMaxTime(int maxMinutes) const {
    std::vector<Recipe> result;
    for (const auto& r : recipes_) {
        if (r.getTotalTime() <= maxMinutes) {
            result.push_back(r);
        }
    }
    return result;
}

void RecipeBook::displaySummaryTable(const std::vector<Recipe>& list) const {
    if (list.empty()) {
        std::cout << "\033[33m\n  No recipes match the criteria.\033[0m\n";
        return;
    }

    std::cout << "\n\033[1m" << std::left
              << std::setw(5)  << "ID" << " | "
              << std::setw(28) << "Recipe Title" << " | "
              << std::setw(12) << "Category" << " | "
              << std::setw(11) << "Total Time" << " | "
              << std::setw(9)  << "Servings" << " | "
              << std::setw(8)  << "Level" << "\n";
    std::cout << "----------------------------------------------------------------------------------\033[0m\n";

    for (const auto& r : list) {
        std::string timeStr = std::to_string(r.getTotalTime()) + " min";
        std::cout << "\033[32m#" << std::left << std::setw(4) << r.getId() << "\033[0m | "
                  << std::setw(28) << (r.getTitle().length() > 28 ? r.getTitle().substr(0, 25) + "..." : r.getTitle()) << " | "
                  << std::setw(12) << r.getCategory() << " | "
                  << std::setw(11) << timeStr << " | "
                  << std::setw(9)  << r.getServings() << " | "
                  << std::setw(8)  << r.getDifficulty() << "\n";
    }
    std::cout << "----------------------------------------------------------------------------------\n";
    std::cout << "\033[36mTotal: " << list.size() << " recipe(s)\033[0m\n";
}

void RecipeBook::displayAll() const {
    displaySummaryTable(recipes_);
}

void RecipeBook::displayStatistics() const {
    if (recipes_.empty()) {
        std::cout << "\033[33mNo statistics available (Recipe book is empty).\033[0m\n";
        return;
    }

    int totalTime = 0;
    int fastestTime = 999999;
    std::string fastestRecipe;
    std::map<std::string, int> categoryCounts;
    std::map<std::string, int> difficultyCounts;

    for (const auto& r : recipes_) {
        int t = r.getTotalTime();
        totalTime += t;
        if (t < fastestTime) {
            fastestTime = t;
            fastestRecipe = r.getTitle();
        }
        categoryCounts[r.getCategory()]++;
        difficultyCounts[r.getDifficulty()]++;
    }

    double avgTime = static_cast<double>(totalTime) / recipes_.size();

    std::cout << "\n\033[1;36m================ 📊 RECIPE BOOK ANALYTICS ================\033[0m\n";
    std::cout << "  • Total Recipes Loaded:   " << recipes_.size() << "\n";
    std::cout << "  • Average Cooking Time:   " << std::fixed << std::setprecision(1) << avgTime << " minutes\n";
    std::cout << "  • Quickest Dish to Make:  " << fastestRecipe << " (" << fastestTime << " min)\n";

    std::cout << "\n  \033[1mCategories Breakdown:\033[0m\n";
    for (const auto& [cat, count] : categoryCounts) {
        std::cout << "    - " << std::left << std::setw(15) << cat << " : " << count << " recipe(s)\n";
    }

    std::cout << "\n  \033[1mDifficulty Distribution:\033[0m\n";
    for (const auto& [diff, count] : difficultyCounts) {
        std::cout << "    - " << std::left << std::setw(15) << diff << " : " << count << " recipe(s)\n";
    }
    std::cout << "\033[1;36m==========================================================\033[0m\n\n";
}

bool RecipeBook::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) return false;

    out << "# Gourmet RecipeBook C++ Format\n";
    out << "NEXT_ID:" << nextId_ << "\n";
    out << "COUNT:" << recipes_.size() << "\n";

    for (const auto& r : recipes_) {
        out << "RECIPE_START\n";
        out << "id:" << r.getId() << "\n";
        out << "title:" << r.getTitle() << "\n";
        out << "category:" << r.getCategory() << "\n";
        out << "prep_time:" << r.getPrepTime() << "\n";
        out << "cook_time:" << r.getCookTime() << "\n";
        out << "servings:" << r.getServings() << "\n";
        out << "difficulty:" << r.getDifficulty() << "\n";

        const auto& ings = r.getIngredients();
        out << "ingredients_count:" << ings.size() << "\n";
        for (const auto& ing : ings) {
            out << "ing:" << ing.name << "|" << ing.quantity << "|" << ing.unit << "\n";
        }

        const auto& steps = r.getSteps();
        out << "steps_count:" << steps.size() << "\n";
        for (const auto& step : steps) {
            out << "step:" << step << "\n";
        }
        out << "RECIPE_END\n";
    }

    return true;
}

bool RecipeBook::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return false;

    recipes_.clear();
    std::string line;
    Recipe current;
    bool inRecipe = false;

    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("NEXT_ID:", 0) == 0) {
            nextId_ = std::stoi(line.substr(8));
        } else if (line == "RECIPE_START") {
            inRecipe = true;
            current = Recipe();
        } else if (line == "RECIPE_END") {
            if (inRecipe) {
                recipes_.push_back(current);
                inRecipe = false;
            }
        } else if (inRecipe) {
            if (line.rfind("id:", 0) == 0) {
                current.setId(std::stoi(line.substr(3)));
            } else if (line.rfind("title:", 0) == 0) {
                current.setTitle(line.substr(6));
            } else if (line.rfind("category:", 0) == 0) {
                current.setCategory(line.substr(9));
            } else if (line.rfind("prep_time:", 0) == 0) {
                current.setPrepTime(std::stoi(line.substr(10)));
            } else if (line.rfind("cook_time:", 0) == 0) {
                current.setCookTime(std::stoi(line.substr(10)));
            } else if (line.rfind("servings:", 0) == 0) {
                current.setServings(std::stoi(line.substr(9)));
            } else if (line.rfind("difficulty:", 0) == 0) {
                current.setDifficulty(line.substr(11));
            } else if (line.rfind("ing:", 0) == 0) {
                std::string payload = line.substr(4);
                size_t p1 = payload.find('|');
                if (p1 != std::string::npos) {
                    std::string name = payload.substr(0, p1);
                    size_t p2 = payload.find('|', p1 + 1);
                    if (p2 != std::string::npos) {
                        double qty = std::stod(payload.substr(p1 + 1, p2 - (p1 + 1)));
                        std::string unit = payload.substr(p2 + 1);
                        current.addIngredient(Ingredient(name, qty, unit));
                    }
                }
            } else if (line.rfind("step:", 0) == 0) {
                current.addStep(line.substr(5));
            }
        }
    }
    return true;
}

void RecipeBook::seedSampleData() {
    recipes_.clear();
    nextId_ = 1;

    // 1. Shahi Paneer / Butter Chicken
    Recipe r1(1, "Authentic Shahi Paneer", "Dinner", 15, 25, 4, "Medium");
    r1.addIngredient(Ingredient("Paneer cubes", 300, "grams"));
    r1.addIngredient(Ingredient("Tomato puree", 1.5, "cups"));
    r1.addIngredient(Ingredient("Cashew paste", 3.0, "tbsp"));
    r1.addIngredient(Ingredient("Fresh cream", 0.5, "cup"));
    r1.addIngredient(Ingredient("Garam masala", 1.0, "tsp"));
    r1.addIngredient(Ingredient("Kasuri methi", 1.0, "tbsp"));
    r1.addIngredient(Ingredient("Butter", 2.0, "tbsp"));
    r1.addStep("Melt butter in a skillet, sauté whole spices and ginger garlic paste.");
    r1.addStep("Add tomato puree and cook till oil separates.");
    r1.addStep("Stir in cashew paste, cream, and simmer with gentle heat.");
    r1.addStep("Add soft paneer cubes, crushed kasuri methi, and serve hot with naan.");
    addRecipe(r1);

    // 2. Japanese Chicken Teriyaki
    Recipe r2(2, "Japanese Chicken Teriyaki Bowl", "Dinner", 15, 15, 2, "Easy");
    r2.addIngredient(Ingredient("Boneless chicken thighs", 400, "grams"));
    r2.addIngredient(Ingredient("Soy sauce", 3.0, "tbsp"));
    r2.addIngredient(Ingredient("Mirin or honey", 2.0, "tbsp"));
    r2.addIngredient(Ingredient("Sake or rice vinegar", 1.0, "tbsp"));
    r2.addIngredient(Ingredient("Grated fresh ginger", 1.0, "tsp"));
    r2.addIngredient(Ingredient("Steamed Jasmine rice", 2.0, "bowls"));
    r2.addIngredient(Ingredient("Toasted sesame seeds & scallions", 1.0, "tbsp"));
    r2.addStep("Pan-fry seasoned chicken skin-side down until golden brown and crispy.");
    r2.addStep("Mix soy sauce, mirin, sake, and ginger into a glaze.");
    r2.addStep("Pour glaze into the pan; reduce over medium heat until chicken is coated in sticky sauce.");
    r2.addStep("Slice chicken, arrange over steamed jasmine rice, and garnish with sesame and scallions.");
    addRecipe(r2);

    // 3. Mediterranean Avocado Toast
    Recipe r3(3, "Mediterranean Avocado & Poached Egg Toast", "Breakfast", 10, 5, 2, "Easy");
    r3.addIngredient(Ingredient("Artisan sourdough bread slices", 2.0, "slices"));
    r3.addIngredient(Ingredient("Ripe avocados", 2.0, "whole"));
    r3.addIngredient(Ingredient("Fresh eggs", 2.0, "eggs"));
    r3.addIngredient(Ingredient("Crumbled feta cheese", 30.0, "grams"));
    r3.addIngredient(Ingredient("Chili flakes & lime juice", 1.0, "tsp"));
    r3.addStep("Toast the sourdough slices until golden and crisp.");
    r3.addStep("Mash avocados with lime juice, salt, and black pepper; spread generously on toast.");
    r3.addStep("Poach two fresh eggs in simmering water for 3 minutes.");
    r3.addStep("Top toast with poached egg, crumbled feta cheese, and chili flakes.");
    addRecipe(r3);

    // 4. Molten Chocolate Lava Cake
    Recipe r4(4, "Decadent Chocolate Lava Cake", "Dessert", 15, 12, 2, "Medium");
    r4.addIngredient(Ingredient("Dark chocolate (70%)", 100.0, "grams"));
    r4.addIngredient(Ingredient("Unsalted butter", 60.0, "grams"));
    r4.addIngredient(Ingredient("Powdered sugar", 50.0, "grams"));
    r4.addIngredient(Ingredient("Whole eggs", 2.0, "whole"));
    r4.addIngredient(Ingredient("All-purpose flour", 30.0, "grams"));
    r4.addIngredient(Ingredient("Vanilla extract", 0.5, "tsp"));
    r4.addStep("Preheat oven to 400°F (200°C) and grease 2 ramekins with butter and cocoa powder.");
    r4.addStep("Melt chocolate and butter together in a heatproof bowl over simmering water.");
    r4.addStep("Whisk eggs and sugar until pale, then fold into melted chocolate with flour.");
    r4.addStep("Divide into ramekins and bake for 12 minutes until edges are firm but center is soft.");
    r4.addStep("Invert onto serving plates, dust with powdered sugar, and serve with vanilla ice cream.");
    addRecipe(r4);
}

} // namespace Culinary
