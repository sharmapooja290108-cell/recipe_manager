#include "../include/recipe.h"

/*
 * Storage format specification (plain human-readable structured text):
 * ---
 * RECIPE_START
 * id:1
 * title:Classic Margherita Pizza
 * category:Dinner
 * prep_time:20
 * cook_time:15
 * servings:4
 * difficulty:Medium
 * ingredients_count:5
 * ing:Pizza Dough|1.0|ball
 * ing:Tomato Sauce|0.5|cups
 * ...
 * steps_count:4
 * step:Preheat oven to 475°F (245°C).
 * ...
 * RECIPE_END
 */

int save_recipes_to_file(const RecipeBook *book, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open file for writing");
        return 0;
    }

    fprintf(fp, "# RecipeBook Data File (v1.0)\n");
    fprintf(fp, "NEXT_ID:%d\n", book->next_id);
    fprintf(fp, "COUNT:%d\n", book->count);

    for (int i = 0; i < book->count; i++) {
        const Recipe *r = &book->recipes[i];
        fprintf(fp, "RECIPE_START\n");
        fprintf(fp, "id:%d\n", r->id);
        fprintf(fp, "title:%s\n", r->title);
        fprintf(fp, "category:%s\n", r->category);
        fprintf(fp, "prep_time:%d\n", r->prep_time_min);
        fprintf(fp, "cook_time:%d\n", r->cook_time_min);
        fprintf(fp, "servings:%d\n", r->servings);
        fprintf(fp, "difficulty:%s\n", r->difficulty);
        fprintf(fp, "ingredients_count:%d\n", r->ingredient_count);

        for (int j = 0; j < r->ingredient_count; j++) {
            fprintf(fp, "ing:%s|%.2f|%s\n",
                    r->ingredients[j].name,
                    r->ingredients[j].quantity,
                    r->ingredients[j].unit);
        }

        fprintf(fp, "steps_count:%d\n", r->step_count);
        for (int k = 0; k < r->step_count; k++) {
            fprintf(fp, "step:%s\n", r->steps[k]);
        }
        fprintf(fp, "RECIPE_END\n");
    }

    fclose(fp);
    return 1;
}

int load_recipes_from_file(RecipeBook *book, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return 0; /* File does not exist yet */
    }

    char line[512];
    Recipe current;
    bool in_recipe = false;

    /* Reset existing */
    book->count = 0;

    while (fgets(line, sizeof(line), fp)) {
        trim_whitespace(line);
        if (line[0] == '\0' || line[0] == '#') continue;

        if (strncmp(line, "NEXT_ID:", 8) == 0) {
            book->next_id = atoi(line + 8);
        } else if (strcmp(line, "RECIPE_START") == 0) {
            in_recipe = true;
            memset(&current, 0, sizeof(Recipe));
        } else if (strcmp(line, "RECIPE_END") == 0) {
            if (in_recipe) {
                // Manually push to preserve saved ID
                if (book->count >= book->capacity) {
                    book->capacity *= 2;
                    book->recipes = (Recipe *)realloc(book->recipes, sizeof(Recipe) * book->capacity);
                }
                book->recipes[book->count++] = current;
                in_recipe = false;
            }
        } else if (in_recipe) {
            if (strncmp(line, "id:", 3) == 0) {
                current.id = atoi(line + 3);
            } else if (strncmp(line, "title:", 6) == 0) {
                strncpy(current.title, line + 6, sizeof(current.title) - 1);
            } else if (strncmp(line, "category:", 9) == 0) {
                strncpy(current.category, line + 9, sizeof(current.category) - 1);
            } else if (strncmp(line, "prep_time:", 10) == 0) {
                current.prep_time_min = atoi(line + 10);
            } else if (strncmp(line, "cook_time:", 10) == 0) {
                current.cook_time_min = atoi(line + 10);
            } else if (strncmp(line, "servings:", 9) == 0) {
                current.servings = atoi(line + 9);
            } else if (strncmp(line, "difficulty:", 11) == 0) {
                strncpy(current.difficulty, line + 11, sizeof(current.difficulty) - 1);
            } else if (strncmp(line, "ingredients_count:", 18) == 0) {
                current.ingredient_count = atoi(line + 18);
                if (current.ingredient_count > MAX_INGREDIENTS) current.ingredient_count = MAX_INGREDIENTS;
            } else if (strncmp(line, "ing:", 4) == 0) {
                char *content = line + 4;
                char *first_bar = strchr(content, '|');
                if (first_bar) {
                    *first_bar = '\0';
                    char *second_bar = strchr(first_bar + 1, '|');
                    if (second_bar) {
                        *second_bar = '\0';
                        int idx = -1;
                        for (int i = 0; i < MAX_INGREDIENTS; i++) {
                            if (current.ingredients[i].name[0] == '\0') {
                                idx = i;
                                break;
                            }
                        }
                        if (idx != -1) {
                            strncpy(current.ingredients[idx].name, content, MAX_INGR_NAME_LEN - 1);
                            current.ingredients[idx].quantity = atof(first_bar + 1);
                            strncpy(current.ingredients[idx].unit, second_bar + 1, MAX_UNIT_LEN - 1);
                        }
                    }
                }
            } else if (strncmp(line, "steps_count:", 12) == 0) {
                current.step_count = atoi(line + 12);
                if (current.step_count > MAX_STEPS) current.step_count = MAX_STEPS;
            } else if (strncmp(line, "step:", 5) == 0) {
                int s_idx = -1;
                for (int i = 0; i < MAX_STEPS; i++) {
                    if (current.steps[i][0] == '\0') {
                        s_idx = i;
                        break;
                    }
                }
                if (s_idx != -1) {
                    strncpy(current.steps[s_idx], line + 5, MAX_STEP_LEN - 1);
                }
            }
        }
    }

    fclose(fp);
    return 1;
}

void seed_sample_recipes(RecipeBook *book) {
    // 1. Margherita Pizza
    Recipe r1 = {
        .title = "Classic Margherita Pizza",
        .category = "Dinner",
        .prep_time_min = 20,
        .cook_time_min = 12,
        .servings = 4,
        .difficulty = "Medium",
        .ingredient_count = 5,
        .ingredients = {
            {"Pizza dough", 1.0, "portion"},
            {"San Marzano tomato sauce", 0.5, "cups"},
            {"Fresh mozzarella", 200.0, "grams"},
            {"Fresh basil leaves", 10.0, "leaves"},
            {"Extra virgin olive oil", 1.0, "tbsp"}
        },
        .step_count = 4,
        .steps = {
            "Stretch out the dough on parchment paper or pizza peel.",
            "Spread tomato sauce evenly, leaving a crust border.",
            "Tear fresh mozzarella over sauce and drizzle with olive oil.",
            "Bake at 500°F (260°C) for 10-12 minutes until bubbly and golden, top with fresh basil."
        }
    };
    add_recipe(book, &r1);

    // 2. Creamy Garlic Alfredo Pasta
    Recipe r2 = {
        .title = "Creamy Garlic Fettuccine Alfredo",
        .category = "Dinner",
        .prep_time_min = 10,
        .cook_time_min = 15,
        .servings = 2,
        .difficulty = "Easy",
        .ingredient_count = 6,
        .ingredients = {
            {"Fettuccine pasta", 250.0, "grams"},
            {"Unsalted butter", 50.0, "grams"},
            {"Heavy cream", 1.0, "cup"},
            {"Minced garlic cloves", 4.0, "cloves"},
            {"Grated Parmesan cheese", 1.0, "cup"},
            {"Black pepper & salt", 1.0, "pinch"}
        },
        .step_count = 4,
        .steps = {
            "Boil fettuccine in salted water until al dente.",
            "Melt butter in a pan over medium heat and sauté garlic for 1 minute.",
            "Stir in heavy cream and bring to a gentle simmer for 3 minutes.",
            "Stir in Parmesan until creamy and toss pasta in sauce. Season with black pepper."
        }
    };
    add_recipe(book, &r2);

    // 3. Fluffy Blueberry Pancakes
    Recipe r3 = {
        .title = "Fluffy Blueberry Pancakes",
        .category = "Breakfast",
        .prep_time_min = 15,
        .cook_time_min = 10,
        .servings = 3,
        .difficulty = "Easy",
        .ingredient_count = 6,
        .ingredients = {
            {"All-purpose flour", 1.5, "cups"},
            {"Baking powder", 3.5, "tsp"},
            {"Sugar", 1.0, "tbsp"},
            {"Milk", 1.25, "cups"},
            {"Egg", 1.0, "whole"},
            {"Fresh blueberries", 1.0, "cup"}
        },
        .step_count = 3,
        .steps = {
            "Whisk dry ingredients in a large bowl.",
            "Whisk milk and egg together, then fold into dry ingredients with blueberries.",
            "Pour 1/4 cup batter onto a buttered hot griddle; flip when bubbles appear."
        }
    };
    add_recipe(book, &r3);

    // 4. Vibrant Berry Smoothie Bowl
    Recipe r4 = {
        .title = "Vibrant Berry Smoothie Bowl",
        .category = "Breakfast",
        .prep_time_min = 8,
        .cook_time_min = 0,
        .servings = 1,
        .difficulty = "Easy",
        .ingredient_count = 5,
        .ingredients = {
            {"Frozen mixed berries", 1.5, "cups"},
            {"Frozen banana", 1.0, "whole"},
            {"Almond milk", 0.5, "cup"},
            {"Chia seeds", 1.0, "tbsp"},
            {"Granola", 0.25, "cup"}
        },
        .step_count = 2,
        .steps = {
            "Blend frozen berries, banana, and almond milk on high until thick and smooth.",
            "Pour into a bowl, arrange chia seeds and granola on top, and serve immediately."
        }
    };
    add_recipe(book, &r4);
}
