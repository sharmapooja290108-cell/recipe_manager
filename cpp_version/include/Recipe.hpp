#pragma once

#include "Ingredient.hpp"
#include <string>
#include <vector>
#include <iostream>

namespace Culinary {

class Recipe {
private:
    int id_{0};
    std::string title_;
    std::string category_;
    int prepTimeMin_{0};
    int cookTimeMin_{0};
    int servings_{1};
    std::string difficulty_{"Medium"};
    std::vector<Ingredient> ingredients_;
    std::vector<std::string> steps_;

public:
    Recipe() = default;
    Recipe(int id, std::string title, std::string category,
           int prepTime, int cookTime, int servings, std::string difficulty)
        : id_(id), title_(std::move(title)), category_(std::move(category)),
          prepTimeMin_(prepTime), cookTimeMin_(cookTime), servings_(servings),
          difficulty_(std::move(difficulty)) {}

    // Getters
    int getId() const { return id_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getCategory() const { return category_; }
    int getPrepTime() const { return prepTimeMin_; }
    int getCookTime() const { return cookTimeMin_; }
    int getTotalTime() const { return prepTimeMin_ + cookTimeMin_; }
    int getServings() const { return servings_; }
    const std::string& getDifficulty() const { return difficulty_; }
    const std::vector<Ingredient>& getIngredients() const { return ingredients_; }
    const std::vector<std::string>& getSteps() const { return steps_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setTitle(std::string t) { title_ = std::move(t); }
    void setCategory(std::string c) { category_ = std::move(c); }
    void setPrepTime(int p) { prepTimeMin_ = p; }
    void setCookTime(int c) { cookTimeMin_ = c; }
    void setServings(int s) { servings_ = s; }
    void setDifficulty(std::string d) { difficulty_ = std::move(d); }

    void addIngredient(const Ingredient& ing) { ingredients_.push_back(ing); }
    void addStep(const std::string& step) { steps_.push_back(step); }
    void clearIngredients() { ingredients_.clear(); }
    void clearSteps() { steps_.clear(); }

    // Business Logic
    void displayCard(int targetServings = 0) const;
    bool matchesSearch(const std::string& query) const;
    bool hasIngredient(const std::string& ingName) const;
};

} // namespace Culinary
