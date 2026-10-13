# 🍳 Gourmet Recipe Book Manager (C, C++ & Modern Web)

A comprehensive, state-of-the-art **Recipe Book Manager System** built across three synchronized technologies:
1. **C (C11 standard)**: Procedural backend with custom memory management, file persistence, and terminal CLI.
2. **C++ (Modern C++17 OOP)**: Object-oriented architecture with STL vectors/maps, dynamic scaling, search algorithms, and culinary analytics.
3. **Web Frontend (HTML5, Vanilla CSS, Modern JavaScript)**: Dark culinary glassmorphism interface, real-time dynamic servings scaler, pantry ingredient matching, recipe creation/editing modal, and real-time synchronization with C & C++ data files.

---

## 📁 Project Architecture

```text
pooja/
├── Makefile                     # Root build automation (C, C++, Web)
├── README.md                    # Documentation & user guide
├── server.py                    # Local Python HTTP bridge (syncs Web UI <-> C/C++ files)
├── recipes_data.txt             # Persistent storage file (used by C binary & Web sync)
├── recipes_cpp.txt              # Persistent storage file (used by C++ binary & Web sync)
│
├── bin/                         # Compiled binaries
│   ├── recipe_manager_c         # C executable
│   └── recipe_manager_cpp       # C++ executable
│
├── c_version/                   # C11 Implementation
│   ├── Makefile
│   ├── include/recipe.h         # Structs, ANSI color definitions, function prototypes
│   └── src/
│       ├── main.c               # Interactive menu loop
│       ├── recipe.c             # CRUD, search, and dynamic portion scaling
│       └── storage.c            # File persistence & sample recipe seeding
│
├── cpp_version/                 # Modern C++17 Implementation
│   ├── Makefile
│   ├── include/
│   │   ├── Ingredient.hpp       # Ingredient class & proportional unit scaling
│   │   ├── Recipe.hpp           # Recipe class with search matching & getters
│   │   └── RecipeBook.hpp       # Manager class, analytics, and file I/O
│   └── src/
│       ├── main.cpp             # Interactive menu & CLI interface
│       ├── Recipe.cpp           # Recipe card rendering & search matches
│       └── RecipeBook.cpp       # Filtering, analytics, and file serialization
│
└── frontend/                    # Web Application
    ├── index.html               # Semantic HTML5 layout & modal overlays
    ├── css/
    │   └── style.css            # Dark glassmorphism, responsive grid, animations
    ├── js/
    │   └── app.js               # Reactive state, serving scaler, pantry matcher, API sync
    └── assets/
        └── images/              # High-resolution food photography assets
            ├── hero.jpg
            ├── pizza.jpg
            ├── paneer.jpg
            ├── pancakes.jpg
            └── lavacake.jpg
```

---

## 🚀 How to Start the Project

### 1. Launch the Web Frontend (Recommended)
You can start the web frontend with one command:

```bash
make run-web
```
Or:
```bash
python3 server.py
```

On Windows PowerShell, `make` may not be installed. From the project folder, start the server with:

```powershell
py -3 server.py
```

If the Python launcher (`py`) is unavailable, use `python server.py` instead.
Then open **[http://localhost:5050](http://localhost:5050)** in your browser.

> [!TIP]
> You can also double-click `frontend/index.html` directly in Finder to open it in standalone offline mode (uses `localStorage` + file import/export).

---

### 2. Run the C Terminal Application
```bash
make run-c
```
*Or execute directly:*
```bash
./bin/recipe_manager_c
```

**Main Menu Options:**
- `1` 📋 List All Recipes (tabular format)
- `2` 📖 View Full Recipe & Scale Servings (dynamically recalculates portions)
- `3` ➕ Add New Recipe (interactive guided prompts)
- `4` ✏️ Edit Recipe
- `5` 🗑️ Delete Recipe
- `6` 🔍 Search by Name, Category, or Ingredient
- `7` 💾 Save / Reload
- `8` 🌱 Reset & Seed Sample Recipes
- `0` 🚪 Exit

---

### 3. Run the C++ Terminal Application
```bash
make run-cpp
```
*Or execute directly:*
```bash
./bin/recipe_manager_cpp
```

Includes all C features plus:
- **Culinary Analytics**: Average cooking time, quickest dish to prepare, category breakdown.
- **Quick Meals Filter**: Search for meals that take $\le X$ minutes.

---

### 4. Build or Clean Everything
```bash
make        # Compiles both C and C++ binaries
make clean  # Cleans build artifacts
```

---

## 🌟 Web Frontend Features

1. **Dynamic Servings Scaler**:
   - Open any recipe modal and click `+` or `−` to scale servings (e.g. from 4 to 6 or 8).
   - Every ingredient measurement updates in real-time.
2. **Interactive Cooking Checklist**:
   - Check off ingredients as you gather them in your kitchen.
   - Click instruction steps to track progress with visual badges.
3. **Pantry Assistant ("What Can I Cook?")**:
   - Click **"Pantry Matcher"** in the top navbar.
   - Select ingredients you have in your fridge/pantry (e.g., `garlic`, `eggs`, `flour`).
   - The app filters recipes and ranks them with a match percentage badge (e.g., `100% Match`, `75% Match`).
4. **Live Search & Category Filtering**:
   - Instant search across recipe title, category, and ingredients.
   - Filter chips for *Breakfast*, *Dinner*, *Dessert*, and *Under 25 Mins*.
5. **Real-Time C/C++ Sync**:
   - When running via `server.py`, recipes added or edited in the web interface are saved directly into `recipes_data.txt` and `recipes_cpp.txt`.
   - Running `./bin/recipe_manager_c` in the terminal immediately reflects your changes!
   - You can also export or import `.txt` data files directly from the **C/C++ Sync** modal.
