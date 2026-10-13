#pragma once

#include "Recipe.hpp"
#include <vector>
#include <string>
#include <optional>
#include <map>

namespace Culinary {

class RecipeBook {
private:
    std::vector<Recipe> recipes_;
    int nextId_{1};

public:
    RecipeBook() = default;

    int addRecipe(Recipe recipe);
    bool updateRecipe(int id, const Recipe& updated);
    bool deleteRecipe(int id);

    std::optional<Recipe> getRecipeById(int id) const;
    const std::vector<Recipe>& getAllRecipes() const { return recipes_; }
    size_t size() const { return recipes_.size(); }
    bool empty() const { return recipes_.empty(); }

    std::vector<Recipe> searchByTitle(const std::string& query) const;
    std::vector<Recipe> filterByCategory(const std::string& category) const;
    std::vector<Recipe> filterByIngredient(const std::string& ingredient) const;
    std::vector<Recipe> filterByMaxTime(int maxMinutes) const;

    void displaySummaryTable(const std::vector<Recipe>& list) const;
    void displayAll() const;
    void displayStatistics() const;

    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    void seedSampleData();
};

} // namespace Culinary
