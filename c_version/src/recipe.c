#include "../include/recipe.h"
#include <ctype.h>

#define INITIAL_CAPACITY 8

void init_recipe_book(RecipeBook *book) {
  book->count = 0;
  book->capacity = INITIAL_CAPACITY;
  book->next_id = 1;
  book->recipes = (Recipe *)malloc(sizeof(Recipe) * book->capacity);
  if (!book->recipes) {
    fprintf(stderr, ANSI_RED
            "Error: Failed to allocate memory for RecipeBook.\n" ANSI_RESET);
    exit(EXIT_FAILURE);
  }
}

void free_recipe_book(RecipeBook *book) {
  if (book->recipes) {
    free(book->recipes);
    book->recipes = NULL;
  }
  book->count = 0;
  book->capacity = 0;
}

static void ensure_capacity(RecipeBook *book) {
  if (book->count >= book->capacity) {
    int new_capacity = book->capacity * 2;
    Recipe *new_block =
        (Recipe *)realloc(book->recipes, sizeof(Recipe) * new_capacity);
    if (!new_block) {
      fprintf(stderr,
              ANSI_RED "Error: Memory reallocation failed.\n" ANSI_RESET);
      return;
    }
    book->recipes = new_block;
    book->capacity = new_capacity;
  }
}

int add_recipe(RecipeBook *book, const Recipe *recipe) {
  ensure_capacity(book);
  Recipe r = *recipe;
  r.id = book->next_id++;
  book->recipes[book->count++] = r;
  return r.id;
}

Recipe *find_recipe_by_id(const RecipeBook *book, int id) {
  for (int i = 0; i < book->count; i++) {
    if (book->recipes[i].id == id) {
      return &book->recipes[i];
    }
  }
  return NULL;
}

int update_recipe(RecipeBook *book, int id, const Recipe *updated) {
  Recipe *r = find_recipe_by_id(book, id);
  if (!r)
    return 0;

  int original_id = r->id;
  *r = *updated;
  r->id = original_id;
  return 1;
}

int delete_recipe(RecipeBook *book, int id) {
  int target_idx = -1;
  for (int i = 0; i < book->count; i++) {
    if (book->recipes[i].id == id) {
      target_idx = i;
      break;
    }
  }
  if (target_idx == -1)
    return 0;

  for (int i = target_idx; i < book->count - 1; i++) {
    book->recipes[i] = book->recipes[i + 1];
  }
  book->count--;
  return 1;
}

/* String Helper Utilities */
void trim_whitespace(char *str) {
  if (!str)
    return;
  char *start = str;
  while (isspace((unsigned char)*start))
    start++;
  if (*start == 0) {
    *str = 0;
    return;
  }
  char *end = start + strlen(start) - 1;
  while (end > start && isspace((unsigned char)*end))
    end--;
  end[1] = '\0';
  if (start != str) {
    memmove(str, start, strlen(start) + 1);
  }
}

bool str_contains_ignore_case(const char *haystack, const char *needle) {
  if (!haystack || !needle)
    return false;
  if (strlen(needle) == 0)
    return true;

  size_t needle_len = strlen(needle);
  size_t haystack_len = strlen(haystack);
  if (needle_len > haystack_len)
    return false;

  for (size_t i = 0; i <= haystack_len - needle_len; i++) {
    bool match = true;
    for (size_t j = 0; j < needle_len; j++) {
      if (tolower((unsigned char)haystack[i + j]) !=
          tolower((unsigned char)needle[j])) {
        match = false;
        break;
      }
    }
    if (match)
      return true;
  }
  return false;
}

/* Formatted Visual Renderers */
void print_banner(void) {
  printf(ANSI_CYAN ANSI_BOLD);
  printf("╔════════════════════════════════════════════════════════════════════"
         "╗\n");
  printf("║                 🍳 GOURMET RECIPE BOOK MANAGER                     "
         "║\n");
  printf("║                    C & C++ Master Chef Edition                     "
         "║\n");
  printf("╚════════════════════════════════════════════════════════════════════"
         "╝\n");
  printf(ANSI_RESET);
}

void list_all_recipes(const RecipeBook *book) {
  if (book->count == 0) {
    printf(ANSI_YELLOW "\n  No recipes available. Try adding one or seeding "
                       "samples!\n" ANSI_RESET);
    return;
  }

  printf(ANSI_BOLD "\n%-4s | %-26s | %-12s | %-10s | %-9s | %-8s\n", "ID",
         "Recipe Title", "Category", "Total Time", "Servings", "Level");
  printf("---------------------------------------------------------------------"
         "----------\n" ANSI_RESET);

  for (int i = 0; i < book->count; i++) {
    const Recipe *r = &book->recipes[i];
    char time_str[32];
    snprintf(time_str, sizeof(time_str), "%d min",
             r->prep_time_min + r->cook_time_min);
    printf(ANSI_GREEN "#%-3d" ANSI_RESET
                      " | %-26.26s | %-12.12s | %-10s | %-9d | %-8s\n",
           r->id, r->title, r->category, time_str, r->servings, r->difficulty);
  }
  printf("---------------------------------------------------------------------"
         "----------\n");
  printf(ANSI_CYAN "Total Recipes: %d\n" ANSI_RESET, book->count);
}

void display_recipe_card(const Recipe *recipe, int target_servings) {
  if (!recipe)
    return;

  double scale = 1.0;
  if (target_servings > 0 && recipe->servings > 0) {
    scale = (double)target_servings / (double)recipe->servings;
  } else {
    target_servings = recipe->servings;
  }

  printf("\n" ANSI_CYAN "======================================================"
                        "================\n" ANSI_RESET);
  printf(ANSI_BOLD ANSI_YELLOW "  📖 %s (#%d)\n" ANSI_RESET, recipe->title,
         recipe->id);
  printf(ANSI_CYAN "==========================================================="
                   "===========\n" ANSI_RESET);

  printf(ANSI_BOLD "Category:   " ANSI_RESET "%-18s " ANSI_BOLD
                   "Difficulty: " ANSI_RESET "%s\n",
         recipe->category, recipe->difficulty);
  printf(ANSI_BOLD "Prep Time:  " ANSI_RESET "%-18d " ANSI_BOLD
                   "Cook Time:  " ANSI_RESET "%d min\n",
         recipe->prep_time_min, recipe->cook_time_min);

  if (scale != 1.0) {
    printf(ANSI_BOLD
           "Servings:   " ANSI_RESET "%d " ANSI_MAGENTA
           "(Scaled from original %d servings, factor: %.2fx)\n" ANSI_RESET,
           target_servings, recipe->servings, scale);
  } else {
    printf(ANSI_BOLD "Servings:   " ANSI_RESET "%d\n", recipe->servings);
  }

  printf(ANSI_BOLD ANSI_CYAN "\n--- 🥗 Ingredients (%d items) ---\n" ANSI_RESET,
         recipe->ingredient_count);
  for (int i = 0; i < recipe->ingredient_count; i++) {
    const Ingredient *ing = &recipe->ingredients[i];
    double scaled_qty = ing->quantity * scale;
    if (scaled_qty == (int)scaled_qty) {
      printf("  • %-32s : %.0f %s\n", ing->name, scaled_qty, ing->unit);
    } else {
      printf("  • %-32s : %.2f %s\n", ing->name, scaled_qty, ing->unit);
    }
  }

  printf(ANSI_BOLD ANSI_CYAN
         "\n--- 📝 Preparation Steps (%d steps) ---\n" ANSI_RESET,
         recipe->step_count);
  for (int i = 0; i < recipe->step_count; i++) {
    printf("  " ANSI_BOLD "%2d." ANSI_RESET " %s\n", i + 1, recipe->steps[i]);
  }
  printf(ANSI_CYAN "==========================================================="
                   "===========\n\n" ANSI_RESET);
}

void search_recipes_by_title(const RecipeBook *book, const char *query) {
  int found = 0;
  printf("\n" ANSI_BOLD
         "--- Search Results for Title containing \"%s\" ---\n" ANSI_RESET,
         query);
  for (int i = 0; i < book->count; i++) {
    if (str_contains_ignore_case(book->recipes[i].title, query)) {
      printf(ANSI_GREEN "#%d" ANSI_RESET " - %s (%s, %d servings)\n",
             book->recipes[i].id, book->recipes[i].title,
             book->recipes[i].category, book->recipes[i].servings);
      found++;
    }
  }
  if (found == 0) {
    printf(ANSI_YELLOW "No matching recipes found.\n" ANSI_RESET);
  } else {
    printf(ANSI_CYAN "Found %d match(es).\n" ANSI_RESET, found);
  }
}

void search_recipes_by_category(const RecipeBook *book, const char *category) {
  int found = 0;
  printf("\n" ANSI_BOLD "--- Recipes in Category \"%s\" ---\n" ANSI_RESET,
         category);
  for (int i = 0; i < book->count; i++) {
    if (str_contains_ignore_case(book->recipes[i].category, category)) {
      printf(ANSI_GREEN "#%d" ANSI_RESET " - %s (%s, Time: %d min)\n",
             book->recipes[i].id, book->recipes[i].title,
             book->recipes[i].difficulty,
             book->recipes[i].prep_time_min + book->recipes[i].cook_time_min);
      found++;
    }
  }
  if (found == 0) {
    printf(ANSI_YELLOW "No matching recipes found for category.\n" ANSI_RESET);
  } else {
    printf(ANSI_CYAN "Found %d match(es).\n" ANSI_RESET, found);
  }
}

void search_recipes_by_ingredient(const RecipeBook *book,
                                  const char *ingredient_name) {
  int found = 0;
  printf("\n" ANSI_BOLD "--- Recipes using Ingredient \"%s\" ---\n" ANSI_RESET,
         ingredient_name);
  for (int i = 0; i < book->count; i++) {
    const Recipe *r = &book->recipes[i];
    bool has_ing = false;
    for (int j = 0; j < r->ingredient_count; j++) {
      if (str_contains_ignore_case(r->ingredients[j].name, ingredient_name)) {
        has_ing = true;
        break;
      }
    }
    if (has_ing) {
      printf(ANSI_GREEN "#%d" ANSI_RESET " - %s [%s]\n", r->id, r->title,
             r->category);
      found++;
    }
  }
  if (found == 0) {
    printf(ANSI_YELLOW "No recipes found using that ingredient.\n" ANSI_RESET);
  } else {
    printf(ANSI_CYAN "Found %d match(es).\n" ANSI_RESET, found);
  }
}

/* Robust Input Helper Functions */
void safe_read_string(char *buffer, int max_len) {
  if (!fgets(buffer, max_len, stdin)) {
    buffer[0] = '\0';
    return;
  }
  size_t len = strlen(buffer);
  if (len > 0 && buffer[len - 1] == '\n') {
    buffer[len - 1] = '\0';
  }
  trim_whitespace(buffer);
}

int safe_read_int(int min_val, int max_val) {
  char line[128];
  int value;
  while (1) {
    if (!fgets(line, sizeof(line), stdin))
      return min_val;
    if (sscanf(line, "%d", &value) == 1 && value >= min_val &&
        value <= max_val) {
      return value;
    }
    printf(
        ANSI_RED
        "Invalid input! Please enter an integer between %d and %d: " ANSI_RESET,
        min_val, max_val);
  }
}

double safe_read_double(double min_val, double max_val) {
  char line[128];
  double value;
  while (1) {
    if (!fgets(line, sizeof(line), stdin))
      return min_val;
    if (sscanf(line, "%lf", &value) == 1 && value >= min_val &&
        value <= max_val) {
      return value;
    }
    printf(ANSI_RED "Invalid input! Please enter a number between %.2f and "
                    "%.2f: " ANSI_RESET,
           min_val, max_val);
  }
}
