#include "../include/recipe.h"

static void input_new_recipe(RecipeBook *book) {
    Recipe r;
    memset(&r, 0, sizeof(Recipe));

    printf("\n" ANSI_BOLD ANSI_CYAN "=== 📝 Add New Recipe ===" ANSI_RESET "\n");

    printf("Recipe Title: ");
    safe_read_string(r.title, sizeof(r.title));
    if (strlen(r.title) == 0) {
        printf(ANSI_RED "Title cannot be empty. Aborted.\n" ANSI_RESET);
        return;
    }

    printf("Category (e.g. Breakfast, Lunch, Dinner, Dessert, Snack): ");
    safe_read_string(r.category, sizeof(r.category));
    if (strlen(r.category) == 0) strcpy(r.category, "General");

    printf("Prep Time in minutes (1 - 720): ");
    r.prep_time_min = safe_read_int(1, 720);

    printf("Cook Time in minutes (0 - 720): ");
    r.cook_time_min = safe_read_int(0, 720);

    printf("Number of Servings (1 - 100): ");
    r.servings = safe_read_int(1, 100);

    printf("Difficulty (Easy, Medium, Hard): ");
    safe_read_string(r.difficulty, sizeof(r.difficulty));
    if (strlen(r.difficulty) == 0) strcpy(r.difficulty, "Medium");

    printf("\nHow many ingredients (1 - %d): ", MAX_INGREDIENTS);
    r.ingredient_count = safe_read_int(1, MAX_INGREDIENTS);

    for (int i = 0; i < r.ingredient_count; i++) {
        printf("  Ingredient #%d Name: ", i + 1);
        safe_read_string(r.ingredients[i].name, sizeof(r.ingredients[i].name));

        printf("  Ingredient #%d Quantity (e.g. 1.5): ", i + 1);
        r.ingredients[i].quantity = safe_read_double(0.01, 10000.0);

        printf("  Ingredient #%d Unit (e.g. cups, grams, tbsp, pieces): ", i + 1);
        safe_read_string(r.ingredients[i].unit, sizeof(r.ingredients[i].unit));
    }

    printf("\nHow many preparation steps (1 - %d): ", MAX_STEPS);
    r.step_count = safe_read_int(1, MAX_STEPS);

    for (int i = 0; i < r.step_count; i++) {
        printf("  Step %d: ", i + 1);
        safe_read_string(r.steps[i], sizeof(r.steps[i]));
    }

    int new_id = add_recipe(book, &r);
    save_recipes_to_file(book, DEFAULT_RECIPES_FILE);
    printf(ANSI_GREEN "\n✔ Recipe \"%s\" successfully added with ID #%d!\n" ANSI_RESET, r.title, new_id);
}

static void view_recipe_details(const RecipeBook *book) {
    if (book->count == 0) {
        printf(ANSI_YELLOW "No recipes to view.\n" ANSI_RESET);
        return;
    }
    printf("\nEnter Recipe ID to view: ");
    int id = safe_read_int(1, 100000);
    Recipe *r = find_recipe_by_id(book, id);
    if (!r) {
        printf(ANSI_RED "Recipe with ID #%d not found.\n" ANSI_RESET, id);
        return;
    }

    printf("Enter target servings to dynamically scale (current: %d, enter %d to keep unchanged): ",
           r->servings, r->servings);
    int servings = safe_read_int(1, 500);

    display_recipe_card(r, servings);
}

static void edit_recipe_flow(RecipeBook *book) {
    if (book->count == 0) {
        printf(ANSI_YELLOW "No recipes to edit.\n" ANSI_RESET);
        return;
    }
    printf("\nEnter Recipe ID to edit: ");
    int id = safe_read_int(1, 100000);
    Recipe *r = find_recipe_by_id(book, id);
    if (!r) {
        printf(ANSI_RED "Recipe with ID #%d not found.\n" ANSI_RESET, id);
        return;
    }

    Recipe updated = *r;
    char buffer[256];

    printf("\n" ANSI_CYAN "Editing Recipe #%d: %s" ANSI_RESET "\n", id, r->title);
    printf("(Press ENTER without typing to keep current value)\n\n");

    printf("New Title [%s]: ", r->title);
    safe_read_string(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strncpy(updated.title, buffer, sizeof(updated.title) - 1);

    printf("New Category [%s]: ", r->category);
    safe_read_string(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strncpy(updated.category, buffer, sizeof(updated.category) - 1);

    printf("New Difficulty [%s]: ", r->difficulty);
    safe_read_string(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strncpy(updated.difficulty, buffer, sizeof(updated.difficulty) - 1);

    printf("New Prep Time in mins [%d]: ", r->prep_time_min);
    safe_read_string(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) updated.prep_time_min = atoi(buffer);

    printf("New Cook Time in mins [%d]: ", r->cook_time_min);
    safe_read_string(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) updated.cook_time_min = atoi(buffer);

    printf("New Servings [%d]: ", r->servings);
    safe_read_string(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) updated.servings = atoi(buffer);

    update_recipe(book, id, &updated);
    save_recipes_to_file(book, DEFAULT_RECIPES_FILE);
    printf(ANSI_GREEN "✔ Recipe #%d updated successfully!\n" ANSI_RESET, id);
}

static void delete_recipe_flow(RecipeBook *book) {
    if (book->count == 0) {
        printf(ANSI_YELLOW "No recipes to delete.\n" ANSI_RESET);
        return;
    }
    printf("\nEnter Recipe ID to delete: ");
    int id = safe_read_int(1, 100000);
    Recipe *r = find_recipe_by_id(book, id);
    if (!r) {
        printf(ANSI_RED "Recipe with ID #%d not found.\n" ANSI_RESET, id);
        return;
    }

    printf(ANSI_RED "Are you sure you want to delete \"%s\"? (1 = Yes, 0 = No): " ANSI_RESET, r->title);
    int confirm = safe_read_int(0, 1);
    if (confirm == 1) {
        delete_recipe(book, id);
        save_recipes_to_file(book, DEFAULT_RECIPES_FILE);
        printf(ANSI_GREEN "✔ Recipe deleted successfully.\n" ANSI_RESET);
    } else {
        printf("Deletion cancelled.\n");
    }
}

static void search_menu(const RecipeBook *book) {
    char query[128];
    printf("\n" ANSI_BOLD ANSI_CYAN "=== 🔍 Search Recipes ===" ANSI_RESET "\n");
    printf("1. Search by Recipe Name\n");
    printf("2. Filter by Category\n");
    printf("3. Search by Ingredient\n");
    printf("0. Back to Main Menu\n");
    printf("Enter choice (0-3): ");
    int choice = safe_read_int(0, 3);

    switch (choice) {
        case 1:
            printf("Enter recipe name keyword: ");
            safe_read_string(query, sizeof(query));
            search_recipes_by_title(book, query);
            break;
        case 2:
            printf("Enter category (e.g. Breakfast, Dinner, Dessert): ");
            safe_read_string(query, sizeof(query));
            search_recipes_by_category(book, query);
            break;
        case 3:
            printf("Enter ingredient name (e.g. garlic, chicken, milk): ");
            safe_read_string(query, sizeof(query));
            search_recipes_by_ingredient(book, query);
            break;
        default:
            break;
    }
}

int main(void) {
    RecipeBook book;
    init_recipe_book(&book);

    /* Try loading existing data; if missing or empty, seed sample recipes */
    if (!load_recipes_from_file(&book, DEFAULT_RECIPES_FILE) || book.count == 0) {
        seed_sample_recipes(&book);
        save_recipes_to_file(&book, DEFAULT_RECIPES_FILE);
    }

    int choice;
    do {
        print_banner();
        printf("\n" ANSI_BOLD "Main Menu:" ANSI_RESET "\n");
        printf("  [" ANSI_GREEN "1" ANSI_RESET "] 📋 List All Recipes\n");
        printf("  [" ANSI_GREEN "2" ANSI_RESET "] 📖 View Full Recipe & Scale Servings\n");
        printf("  [" ANSI_GREEN "3" ANSI_RESET "] ➕ Add New Recipe\n");
        printf("  [" ANSI_GREEN "4" ANSI_RESET "] ✏️  Edit Recipe\n");
        printf("  [" ANSI_GREEN "5" ANSI_RESET "] 🗑️  Delete Recipe\n");
        printf("  [" ANSI_GREEN "6" ANSI_RESET "] 🔍 Search / Filter Recipes\n");
        printf("  [" ANSI_GREEN "7" ANSI_RESET "] 💾 Save & Reload\n");
        printf("  [" ANSI_GREEN "8" ANSI_RESET "] 🌱 Reset & Seed Sample Recipes\n");
        printf("  [" ANSI_RED   "0" ANSI_RESET "] 🚪 Exit\n");
        printf("\nSelect an option [0-8]: ");

        choice = safe_read_int(0, 8);

        switch (choice) {
            case 1:
                list_all_recipes(&book);
                break;
            case 2:
                view_recipe_details(&book);
                break;
            case 3:
                input_new_recipe(&book);
                break;
            case 4:
                edit_recipe_flow(&book);
                break;
            case 5:
                delete_recipe_flow(&book);
                break;
            case 6:
                search_menu(&book);
                break;
            case 7:
                save_recipes_to_file(&book, DEFAULT_RECIPES_FILE);
                printf(ANSI_GREEN "✔ Recipes successfully saved to %s\n" ANSI_RESET, DEFAULT_RECIPES_FILE);
                break;
            case 8:
                free_recipe_book(&book);
                init_recipe_book(&book);
                seed_sample_recipes(&book);
                save_recipes_to_file(&book, DEFAULT_RECIPES_FILE);
                printf(ANSI_GREEN "✔ Sample recipes successfully seeded!\n" ANSI_RESET);
                break;
            case 0:
                save_recipes_to_file(&book, DEFAULT_RECIPES_FILE);
                printf(ANSI_CYAN "\nThank you for using Gourmet Recipe Book Manager. Bon Appétit! 👨‍🍳\n\n" ANSI_RESET);
                break;
            default:
                break;
        }

        if (choice != 0) {
            printf("\nPress ENTER to continue...");
            char enter_buf[16];
            safe_read_string(enter_buf, sizeof(enter_buf));
        }

    } while (choice != 0);

    free_recipe_book(&book);
    return 0;
}
