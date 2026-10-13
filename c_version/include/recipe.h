#ifndef RECIPE_H
#define RECIPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INGREDIENTS 30
#define MAX_STEPS 25
#define MAX_TITLE_LEN 100
#define MAX_CATEGORY_LEN 40
#define MAX_STEP_LEN 256
#define MAX_UNIT_LEN 32
#define MAX_INGR_NAME_LEN 64
#define DEFAULT_RECIPES_FILE "recipes_data.txt"

/* ANSI Color codes for styled terminal output */
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_WHITE   "\x1b[37m"
#define ANSI_BG_DARK "\x1b[48;5;236m"

/* Ingredient Structure */
typedef struct {
    char name[MAX_INGR_NAME_LEN];
    double quantity;
    char unit[MAX_UNIT_LEN];
} Ingredient;

/* Recipe Structure */
typedef struct {
    int id;
    char title[MAX_TITLE_LEN];
    char category[MAX_CATEGORY_LEN];
    int prep_time_min;
    int cook_time_min;
    int servings;
    char difficulty[20];
    int ingredient_count;
    Ingredient ingredients[MAX_INGREDIENTS];
    int step_count;
    char steps[MAX_STEPS][MAX_STEP_LEN];
} Recipe;

/* RecipeBook Collection Manager */
typedef struct {
    Recipe *recipes;
    int count;
    int capacity;
    int next_id;
} RecipeBook;

/* Memory & Initialization */
void init_recipe_book(RecipeBook *book);
void free_recipe_book(RecipeBook *book);

/* Core CRUD Operations */
int add_recipe(RecipeBook *book, const Recipe *recipe);
int update_recipe(RecipeBook *book, int id, const Recipe *updated);
int delete_recipe(RecipeBook *book, int id);
Recipe* find_recipe_by_id(const RecipeBook *book, int id);

/* Search & Filter */
void search_recipes_by_title(const RecipeBook *book, const char *query);
void search_recipes_by_category(const RecipeBook *book, const char *category);
void search_recipes_by_ingredient(const RecipeBook *book, const char *ingredient_name);

/* Display & Formatting */
void display_recipe_card(const Recipe *recipe, int target_servings);
void list_all_recipes(const RecipeBook *book);
void print_banner(void);

/* Persistence */
int save_recipes_to_file(const RecipeBook *book, const char *filename);
int load_recipes_from_file(RecipeBook *book, const char *filename);
void seed_sample_recipes(RecipeBook *book);

/* Utility / Input Helpers */
void trim_whitespace(char *str);
bool str_contains_ignore_case(const char *haystack, const char *needle);
void safe_read_string(char *buffer, int max_len);
int safe_read_int(int min_val, int max_val);
double safe_read_double(double min_val, double max_val);

#endif /* RECIPE_H */
