#include "../include/RecipeBook.hpp"
#include <iostream>
#include <string>
#include <limits>

using namespace Culinary;

static const std::string DATA_FILE = "recipes_cpp.txt";

static void printBanner() {
    std::cout << "\033[1;35m"
              << "╔════════════════════════════════════════════════════════════════════╗\n"
              << "║             ✨ GOURMET RECIPE BOOK MANAGER (C++ OOP)               ║\n"
              << "║                 Modern C++17 Culinary Management                   ║\n"
              << "╚════════════════════════════════════════════════════════════════════╝\n"
              << "\033[0m";
}

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static std::string readLine(const std::string& prompt = "") {
    if (!prompt.empty()) std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

static int readInt(int minVal, int maxVal, const std::string& prompt = "") {
    int val;
    while (true) {
        if (!prompt.empty()) std::cout << prompt;
        if (std::cin >> val && val >= minVal && val <= maxVal) {
            clearInput();
            return val;
        }
        std::cout << "\033[31mInvalid input! Please enter a number between " << minVal << " and " << maxVal << ": \033[0m";
        clearInput();
    }
}

static double readDouble(double minVal, double maxVal, const std::string& prompt = "") {
    double val;
    while (true) {
        if (!prompt.empty()) std::cout << prompt;
        if (std::cin >> val && val >= minVal && val <= maxVal) {
            clearInput();
            return val;
        }
        std::cout << "\033[31mInvalid input! Please enter a number between " << minVal << " and " << maxVal << ": \033[0m";
        clearInput();
    }
}

static void addRecipeFlow(RecipeBook& book) {
    std::cout << "\n\033[1;36m=== 📝 Add New Recipe ===\033[0m\n";

    std::string title = readLine("Recipe Title: ");
    if (title.empty()) {
        std::cout << "\033[31mRecipe title cannot be empty. Cancelled.\033[0m\n";
        return;
    }

    std::string category = readLine("Category (Breakfast, Lunch, Dinner, Dessert, Snack, Beverage): ");
    if (category.empty()) category = "General";

    int prepTime = readInt(1, 720, "Prep Time in minutes (1 - 720): ");
    int cookTime = readInt(0, 720, "Cook Time in minutes (0 - 720): ");
    int servings = readInt(1, 100, "Base Servings (1 - 100): ");

    std::string difficulty = readLine("Difficulty (Easy, Medium, Hard) [Medium]: ");
    if (difficulty.empty()) difficulty = "Medium";

    Recipe r(0, title, category, prepTime, cookTime, servings, difficulty);

    int ingCount = readInt(1, 50, "\nHow many ingredients: ");
    for (int i = 0; i < ingCount; ++i) {
        std::cout << "  Ingredient #" << (i + 1) << " Name: ";
        std::string name = readLine();
        double qty = readDouble(0.01, 10000.0, "  Quantity: ");
        std::string unit = readLine("  Unit (grams, cups, tbsp, pieces, etc.): ");
        r.addIngredient(Ingredient(name, qty, unit));
    }

    int stepCount = readInt(1, 50, "\nHow many preparation steps: ");
    for (int i = 0; i < stepCount; ++i) {
        std::cout << "  Step " << (i + 1) << ": ";
        r.addStep(readLine());
    }

    int id = book.addRecipe(r);
    book.saveToFile(DATA_FILE);
    std::cout << "\033[32m\n✔ Successfully added \"" << title << "\" with ID #" << id << "!\033[0m\n";
}

static void viewRecipeFlow(const RecipeBook& book) {
    if (book.empty()) {
        std::cout << "\033[33mNo recipes available.\033[0m\n";
        return;
    }

    int id = readInt(1, 100000, "\nEnter Recipe ID to view: ");
    auto recipeOpt = book.getRecipeById(id);
    if (!recipeOpt) {
        std::cout << "\033[31mRecipe #" << id << " not found.\033[0m\n";
        return;
    }

    const auto& r = *recipeOpt;
    std::cout << "Enter target servings to dynamically scale (current: " << r.getServings()
              << ", press " << r.getServings() << " for standard): ";
    int target = readInt(1, 500);

    r.displayCard(target);
}

static void editRecipeFlow(RecipeBook& book) {
    if (book.empty()) {
        std::cout << "\033[33mNo recipes available to edit.\033[0m\n";
        return;
    }

    int id = readInt(1, 100000, "\nEnter Recipe ID to edit: ");
    auto recipeOpt = book.getRecipeById(id);
    if (!recipeOpt) {
        std::cout << "\033[31mRecipe #" << id << " not found.\033[0m\n";
        return;
    }

    Recipe updated = *recipeOpt;
    std::cout << "\n\033[1;36mEditing Recipe #" << id << ": " << updated.getTitle() << "\033[0m\n";
    std::cout << "(Press ENTER to keep existing value)\n\n";

    std::string val = readLine("New Title [" + updated.getTitle() + "]: ");
    if (!val.empty()) updated.setTitle(val);

    val = readLine("New Category [" + updated.getCategory() + "]: ");
    if (!val.empty()) updated.setCategory(val);

    val = readLine("New Difficulty [" + updated.getDifficulty() + "]: ");
    if (!val.empty()) updated.setDifficulty(val);

    val = readLine("New Prep Time in mins [" + std::to_string(updated.getPrepTime()) + "]: ");
    if (!val.empty()) updated.setPrepTime(std::stoi(val));

    val = readLine("New Cook Time in mins [" + std::to_string(updated.getCookTime()) + "]: ");
    if (!val.empty()) updated.setCookTime(std::stoi(val));

    val = readLine("New Servings [" + std::to_string(updated.getServings()) + "]: ");
    if (!val.empty()) updated.setServings(std::stoi(val));

    book.updateRecipe(id, updated);
    book.saveToFile(DATA_FILE);
    std::cout << "\033[32m✔ Recipe #" << id << " updated successfully!\033[0m\n";
}

static void deleteRecipeFlow(RecipeBook& book) {
    if (book.empty()) {
        std::cout << "\033[33mNo recipes to delete.\033[0m\n";
        return;
    }

    int id = readInt(1, 100000, "\nEnter Recipe ID to delete: ");
    auto recipeOpt = book.getRecipeById(id);
    if (!recipeOpt) {
        std::cout << "\033[31mRecipe #" << id << " not found.\033[0m\n";
        return;
    }

    int confirm = readInt(0, 1, "\033[31mAre you sure you want to delete \"" + recipeOpt->getTitle() + "\"? (1=Yes, 0=No): \033[0m");
    if (confirm == 1) {
        book.deleteRecipe(id);
        book.saveToFile(DATA_FILE);
        std::cout << "\033[32m✔ Recipe deleted successfully.\033[0m\n";
    } else {
        std::cout << "Deletion aborted.\n";
    }
}

static void searchFilterMenu(const RecipeBook& book) {
    std::cout << "\n\033[1;36m=== 🔍 Search & Filtering ===\033[0m\n";
    std::cout << "1. Search by Name / Keyword\n";
    std::cout << "2. Filter by Category (Breakfast, Dinner, Dessert, etc.)\n";
    std::cout << "3. Filter by Ingredient (Find dishes with what you have!)\n";
    std::cout << "4. Quick Meals (Filter recipes taking less than X minutes)\n";
    std::cout << "0. Back to Main Menu\n";

    int opt = readInt(0, 4, "Choose option [0-4]: ");
    switch (opt) {
        case 1: {
            std::string q = readLine("Enter keyword: ");
            auto matches = book.searchByTitle(q);
            book.displaySummaryTable(matches);
            break;
        }
        case 2: {
            std::string cat = readLine("Enter category: ");
            auto matches = book.filterByCategory(cat);
            book.displaySummaryTable(matches);
            break;
        }
        case 3: {
            std::string ing = readLine("Enter ingredient: ");
            auto matches = book.filterByIngredient(ing);
            book.displaySummaryTable(matches);
            break;
        }
        case 4: {
            int maxMin = readInt(5, 300, "Enter max total preparation + cook time in minutes: ");
            auto matches = book.filterByMaxTime(maxMin);
            book.displaySummaryTable(matches);
            break;
        }
        default:
            break;
    }
}

int main() {
    RecipeBook book;

    if (!book.loadFromFile(DATA_FILE) || book.empty()) {
        book.seedSampleData();
        book.saveToFile(DATA_FILE);
    }

    int choice = -1;
    do {
        printBanner();
        std::cout << "\n\033[1mMain Menu:\033[0m\n";
        std::cout << "  [\033[32m1\033[0m] 📋 List All Recipes\n";
        std::cout << "  [\033[32m2\033[0m] 📖 View Full Recipe & Scale Servings\n";
        std::cout << "  [\033[32m3\033[0m] ➕ Add New Recipe\n";
        std::cout << "  [\033[32m4\033[0m] ✏️  Edit Recipe\n";
        std::cout << "  [\033[32m5\033[0m] 🗑️  Delete Recipe\n";
        std::cout << "  [\033[32m6\033[0m] 🔍 Search & Filter (Ingredient / Quick Meals)\n";
        std::cout << "  [\033[32m7\033[0m] 📊 Recipe Analytics & Stats\n";
        std::cout << "  [\033[32m8\033[0m] 💾 Save to File\n";
        std::cout << "  [\033[32m9\033[0m] 🌱 Reset to Default Sample Recipes\n";
        std::cout << "  [\033[31m0\033[0m] 🚪 Exit\n";

        choice = readInt(0, 9, "\nSelect an option [0-9]: ");

        switch (choice) {
            case 1:
                book.displayAll();
                break;
            case 2:
                viewRecipeFlow(book);
                break;
            case 3:
                addRecipeFlow(book);
                break;
            case 4:
                editRecipeFlow(book);
                break;
            case 5:
                deleteRecipeFlow(book);
                break;
            case 6:
                searchFilterMenu(book);
                break;
            case 7:
                book.displayStatistics();
                break;
            case 8:
                if (book.saveToFile(DATA_FILE)) {
                    std::cout << "\033[32m✔ Saved successfully to " << DATA_FILE << "!\033[0m\n";
                }
                break;
            case 9:
                book.seedSampleData();
                book.saveToFile(DATA_FILE);
                std::cout << "\033[32m✔ Sample culinary recipes reseeded!\033[0m\n";
                break;
            case 0:
                book.saveToFile(DATA_FILE);
                std::cout << "\033[35m\nChef's regards! Have a delicious day! 👨‍🍳✨\n\033[0m\n";
                break;
            default:
                break;
        }

        if (choice != 0) {
            readLine("\nPress ENTER to continue...");
        }

    } while (choice != 0);

    return 0;
}
