/**
 * GOURMET RECIPE BOOK MANAGER - WEB APPLICATION ENGINE
 * Features: Serving Scaler, Pantry Matcher, C/C++ File Sync, Full CRUD
 */

(function () {
  'use strict';

  // Fallback initial seeds with local generated imagery
  const INITIAL_RECIPES = [
    {
      id: 1,
      title: "Classic Margherita Pizza",
      category: "Dinner",
      prep_time_min: 20,
      cook_time_min: 12,
      servings: 4,
      difficulty: "Medium",
      image: "assets/images/pizza.jpg",
      ingredients: [
        { name: "Pizza dough", quantity: 1, unit: "portion" },
        { name: "San Marzano tomato sauce", quantity: 0.5, unit: "cups" },
        { name: "Fresh mozzarella", quantity: 200, unit: "grams" },
        { name: "Fresh basil leaves", quantity: 10, unit: "leaves" },
        { name: "Extra virgin olive oil", quantity: 1, unit: "tbsp" }
      ],
      steps: [
        "Stretch out the dough on parchment paper or pizza peel.",
        "Spread tomato sauce evenly, leaving a crust border.",
        "Tear fresh mozzarella over sauce and drizzle with olive oil.",
        "Bake at 500°F (260°C) for 10-12 minutes until bubbly and golden, top with fresh basil."
      ]
    },
    {
      id: 2,
      title: "Authentic Shahi Paneer",
      category: "Dinner",
      prep_time_min: 15,
      cook_time_min: 25,
      servings: 4,
      difficulty: "Medium",
      image: "assets/images/paneer.jpg",
      ingredients: [
        { name: "Paneer cubes", quantity: 300, unit: "grams" },
        { name: "Tomato puree", quantity: 1.5, unit: "cups" },
        { name: "Cashew paste", quantity: 3, unit: "tbsp" },
        { name: "Fresh cream", quantity: 0.5, unit: "cup" },
        { name: "Garam masala", quantity: 1, unit: "tsp" },
        { name: "Kasuri methi", quantity: 1, unit: "tbsp" },
        { name: "Butter", quantity: 2, unit: "tbsp" }
      ],
      steps: [
        "Melt butter in a skillet, sauté whole spices and ginger garlic paste.",
        "Add tomato puree and cook till oil separates.",
        "Stir in cashew paste, cream, and simmer with gentle heat.",
        "Add soft paneer cubes, crushed kasuri methi, and serve hot with naan."
      ]
    },
    {
      id: 3,
      title: "Fluffy Blueberry Pancakes",
      category: "Breakfast",
      prep_time_min: 15,
      cook_time_min: 10,
      servings: 3,
      difficulty: "Easy",
      image: "assets/images/pancakes.jpg",
      ingredients: [
        { name: "All-purpose flour", quantity: 1.5, unit: "cups" },
        { name: "Baking powder", quantity: 3.5, unit: "tsp" },
        { name: "Sugar", quantity: 1, unit: "tbsp" },
        { name: "Milk", quantity: 1.25, unit: "cups" },
        { name: "Egg", quantity: 1, unit: "whole" },
        { name: "Fresh blueberries", quantity: 1, unit: "cup" }
      ],
      steps: [
        "Whisk dry ingredients in a large bowl.",
        "Whisk milk and egg together, then fold into dry ingredients with blueberries.",
        "Pour 1/4 cup batter onto a buttered hot griddle; flip when bubbles appear."
      ]
    },
    {
      id: 4,
      title: "Decadent Chocolate Lava Cake",
      category: "Dessert",
      prep_time_min: 15,
      cook_time_min: 12,
      servings: 2,
      difficulty: "Medium",
      image: "assets/images/lavacake.jpg",
      ingredients: [
        { name: "Dark chocolate (70%)", quantity: 100, unit: "grams" },
        { name: "Unsalted butter", quantity: 60, unit: "grams" },
        { name: "Powdered sugar", quantity: 50, unit: "grams" },
        { name: "Whole eggs", quantity: 2, unit: "whole" },
        { name: "All-purpose flour", quantity: 30, unit: "grams" },
        { name: "Vanilla extract", quantity: 0.5, unit: "tsp" }
      ],
      steps: [
        "Preheat oven to 400°F (200°C) and grease 2 ramekins with butter and cocoa powder.",
        "Melt chocolate and butter together in a heatproof bowl over simmering water.",
        "Whisk eggs and sugar until pale, then fold into melted chocolate with flour.",
        "Divide into ramekins and bake for 12 minutes until edges are firm but center is soft."
      ]
    }
  ];

  // App State
  let recipes = [];
  let activeCategory = 'ALL';
  let activeDifficulty = 'ALL';
  let activeSort = 'default';
  let searchQuery = '';
  let selectedPantryIngredients = new Set();
  let isBackendConnected = false;
  let activeModalRecipe = null;
  let currentScaledServings = 1;

  // DOM Elements
  const recipesGrid = document.getElementById('recipes-grid');
  const emptyState = document.getElementById('empty-state');
  const searchInput = document.getElementById('recipe-search-input');
  const clearSearchBtn = document.getElementById('clear-search-btn');
  const categoryChips = document.querySelectorAll('.category-chip');
  const difficultySelect = document.getElementById('difficulty-select');
  const sortSelect = document.getElementById('sort-select');
  const recipesCountBadge = document.getElementById('recipes-count-badge');

  // Stats Elements
  const statTotal = document.getElementById('stat-total-recipes');
  const statAvgTime = document.getElementById('stat-avg-time');
  const statFastest = document.getElementById('stat-fastest-dish');
  const statCategories = document.getElementById('stat-categories');

  // Modals
  const recipeModal = document.getElementById('recipe-modal');
  const recipeModalContent = document.getElementById('recipe-modal-content');
  const closeRecipeModalBtn = document.getElementById('close-recipe-modal');

  const recipeFormModal = document.getElementById('recipe-form-modal');
  const recipeEditorForm = document.getElementById('recipe-editor-form');
  const closeFormModalBtn = document.getElementById('close-form-modal');
  const btnCancelForm = document.getElementById('btn-cancel-form');
  const btnOpenAddModal = document.getElementById('btn-open-add-modal');
  const formModalTitle = document.getElementById('form-modal-title');

  const syncModal = document.getElementById('sync-modal');
  const btnSyncModal = document.getElementById('btn-sync-modal');
  const closeSyncModalBtn = document.getElementById('close-sync-modal');
  const backendStatusBadge = document.getElementById('backend-status-badge');
  const backendStatusText = document.getElementById('backend-status-text');
  const syncStatusBox = document.getElementById('sync-status-box');

  const pantryPanel = document.getElementById('pantry-panel');
  const btnPantryMode = document.getElementById('btn-pantry-mode');
  const closePantryBtn = document.getElementById('close-pantry-btn');
  const pantryTagList = document.getElementById('pantry-tag-list');
  const pantrySummaryText = document.getElementById('pantry-summary-text');

  // Toast Container
  const toastContainer = document.getElementById('toast-container');

  /* ==========================================================================
     INIT & STORAGE
     ========================================================================== */

  async function init() {
    setupEventListeners();
    await checkBackendConnection();

    if (isBackendConnected) {
      await fetchRecipesFromBackend();
    } else {
      loadFromLocalStorage();
    }

    renderAll();
  }

  function loadFromLocalStorage() {
    const saved = localStorage.getItem('gourmet_recipes_v1');
    if (saved) {
      try {
        recipes = JSON.parse(saved);
      } catch (e) {
        recipes = [...INITIAL_RECIPES];
      }
    } else {
      recipes = [...INITIAL_RECIPES];
      saveToLocalStorage();
    }
  }

  function saveToLocalStorage() {
    localStorage.setItem('gourmet_recipes_v1', JSON.stringify(recipes));
  }

  async function checkBackendConnection() {
    try {
      const res = await fetch('/api/health', { method: 'GET', cache: 'no-store' });
      if (res.ok) {
        isBackendConnected = true;
        backendStatusBadge.className = 'backend-badge connected';
        backendStatusText.textContent = 'Synced with C/C++ File System';
        return;
      }
    } catch (e) {
      // Backend not running (opened static file)
    }
    isBackendConnected = false;
    backendStatusBadge.className = 'backend-badge offline';
    backendStatusText.textContent = 'Local Standalone Mode';
  }

  async function fetchRecipesFromBackend() {
    try {
      const res = await fetch('/api/recipes');
      if (res.ok) {
        const data = await res.json();
        if (Array.isArray(data) && data.length > 0) {
          // Merge images from local map if present
          recipes = data.map(r => {
            if (!r.image) {
              r.image = getDefaultImageForCategory(r.category, r.title);
            }
            return r;
          });
          saveToLocalStorage();
          showToast('Synchronized with C/C++ recipes_data.txt', 'success');
        }
      }
    } catch (e) {
      console.warn('Backend fetch failed, using local storage.');
      loadFromLocalStorage();
    }
  }

  function getDefaultImageForCategory(category, title) {
    const t = (title || '').toLowerCase();
    if (t.includes('pizza')) return 'assets/images/pizza.jpg';
    if (t.includes('paneer') || t.includes('curry')) return 'assets/images/paneer.jpg';
    if (t.includes('pancake') || t.includes('waffle')) return 'assets/images/pancakes.jpg';
    if (t.includes('cake') || t.includes('dessert') || t.includes('chocolate')) return 'assets/images/lavacake.jpg';

    const c = (category || '').toLowerCase();
    if (c === 'breakfast') return 'assets/images/pancakes.jpg';
    if (c === 'dessert') return 'assets/images/lavacake.jpg';
    if (c === 'dinner' || c === 'lunch') return 'assets/images/pizza.jpg';
    return 'assets/images/hero.jpg';
  }

  /* ==========================================================================
     RENDERING & STATS
     ========================================================================== */

  function renderAll() {
    updateStats();
    renderPantryTags();
    renderCards();
  }

  function updateStats() {
    statTotal.textContent = recipes.length;

    if (recipes.length === 0) {
      statAvgTime.textContent = '0m';
      statFastest.textContent = '--';
      statCategories.textContent = '0';
      return;
    }

    let totalTime = 0;
    let fastestTime = Infinity;
    let fastestTitle = '--';
    const categoriesSet = new Set();

    recipes.forEach(r => {
      const time = (r.prep_time_min || 0) + (r.cook_time_min || 0);
      totalTime += time;
      if (time < fastestTime && time > 0) {
        fastestTime = time;
        fastestTitle = r.title;
      }
      if (r.category) categoriesSet.add(r.category.trim());
    });

    const avg = Math.round(totalTime / recipes.length);
    statAvgTime.textContent = `${avg}m`;
    statFastest.textContent = fastestTitle.length > 18 ? fastestTitle.slice(0, 15) + '...' : fastestTitle;
    statCategories.textContent = categoriesSet.size;
  }

  function filterAndSortRecipes() {
    let result = [...recipes];

    // Search Query
    if (searchQuery.trim()) {
      const q = searchQuery.toLowerCase().trim();
      result = result.filter(r => {
        const inTitle = r.title.toLowerCase().includes(q);
        const inCategory = r.category.toLowerCase().includes(q);
        const inIngr = r.ingredients && r.ingredients.some(ing => ing.name.toLowerCase().includes(q));
        return inTitle || inCategory || inIngr;
      });
    }

    // Category Filter
    if (activeCategory === 'QUICK') {
      result = result.filter(r => ((r.prep_time_min || 0) + (r.cook_time_min || 0)) <= 25);
    } else if (activeCategory !== 'ALL') {
      result = result.filter(r => r.category.toLowerCase() === activeCategory.toLowerCase());
    }

    // Difficulty Filter
    if (activeDifficulty !== 'ALL') {
      result = result.filter(r => r.difficulty.toLowerCase() === activeDifficulty.toLowerCase());
    }

    // Pantry Assistant Match Rate
    if (selectedPantryIngredients.size > 0) {
      result.forEach(r => {
        let matchCount = 0;
        if (r.ingredients && r.ingredients.length > 0) {
          r.ingredients.forEach(ing => {
            for (const chosen of selectedPantryIngredients) {
              if (ing.name.toLowerCase().includes(chosen.toLowerCase())) {
                matchCount++;
                break;
              }
            }
          });
          r._matchRate = Math.round((matchCount / r.ingredients.length) * 100);
          r._matchCount = matchCount;
        } else {
          r._matchRate = 0;
          r._matchCount = 0;
        }
      });

      // Filter to recipes that have at least 1 match, and sort by matchRate descending
      result = result.filter(r => (r._matchCount || 0) > 0);
      result.sort((a, b) => b._matchRate - a._matchRate);
    } else {
      result.forEach(r => { delete r._matchRate; delete r._matchCount; });

      // Sorting
      if (activeSort === 'time-asc') {
        result.sort((a, b) => (a.prep_time_min + a.cook_time_min) - (b.prep_time_min + b.cook_time_min));
      } else if (activeSort === 'title-asc') {
        result.sort((a, b) => a.title.localeCompare(b.title));
      } else if (activeSort === 'servings-desc') {
        result.sort((a, b) => (b.servings || 0) - (a.servings || 0));
      }
    }

    return result;
  }

  function renderCards() {
    const list = filterAndSortRecipes();
    recipesGrid.innerHTML = '';

    recipesCountBadge.textContent = `Showing ${list.length} of ${recipes.length} recipes`;

    if (list.length === 0) {
      recipesGrid.style.display = 'none';
      emptyState.style.display = 'block';
      return;
    }

    recipesGrid.style.display = 'grid';
    emptyState.style.display = 'none';

    list.forEach(recipe => {
      const card = createRecipeCardElement(recipe);
      recipesGrid.appendChild(card);
    });
  }

  function createRecipeCardElement(recipe) {
    const card = document.createElement('div');
    card.className = 'recipe-card';
    card.dataset.id = recipe.id;

    const totalTime = (recipe.prep_time_min || 0) + (recipe.cook_time_min || 0);
    const diffClass = (recipe.difficulty || 'Medium').toLowerCase();
    const imgSrc = recipe.image || getDefaultImageForCategory(recipe.category, recipe.title);

    // Pantry match indicator
    let matchPillHtml = '';
    if (recipe._matchRate !== undefined) {
      matchPillHtml = `<span class="match-rate-pill">🥑 ${recipe._matchRate}% Match</span>`;
    }

    // Ingredients sample pills
    const sampleIngs = (recipe.ingredients || []).slice(0, 3).map(i => `<span class="mini-pill">${escapeHtml(i.name)}</span>`).join('');
    const moreCount = (recipe.ingredients || []).length > 3 ? `<span class="mini-pill">+${recipe.ingredients.length - 3}</span>` : '';

    card.innerHTML = `
      <div class="card-media">
        <img src="${imgSrc}" alt="${escapeHtml(recipe.title)}" class="card-img" loading="lazy">
        <span class="card-category-badge">${escapeHtml(recipe.category || 'General')}</span>
        <span class="card-difficulty-badge ${diffClass}">${escapeHtml(recipe.difficulty || 'Medium')}</span>
        ${matchPillHtml}
      </div>
      <div class="card-body">
        <h3 class="card-title">${escapeHtml(recipe.title)}</h3>
        <div class="card-meta-row">
          <div class="meta-icon-item">⏱️ ${totalTime} mins</div>
          <div class="meta-icon-item">🍽️ ${recipe.servings} Servings</div>
        </div>
        <div class="card-ingredients-preview">
          <span class="ingredients-label">Ingredients (${(recipe.ingredients || []).length}):</span>
          <div class="ingredients-pills">
            ${sampleIngs} ${moreCount}
          </div>
        </div>
        <div class="card-footer">
          <span class="card-id-tag">#${recipe.id}</span>
          <div class="card-actions">
            <button class="btn btn-xs btn-primary btn-view-recipe" data-id="${recipe.id}">View & Scale</button>
            <button class="btn btn-xs btn-secondary btn-edit-recipe" data-id="${recipe.id}" title="Edit Recipe">✏️</button>
            <button class="btn btn-xs btn-ghost btn-delete-recipe" data-id="${recipe.id}" title="Delete Recipe">🗑️</button>
          </div>
        </div>
      </div>
    `;

    return card;
  }

  /* ==========================================================================
     RECIPE DETAIL MODAL WITH SERVING SCALER
     ========================================================================== */

  function openRecipeDetail(id) {
    const recipe = recipes.find(r => r.id === Number(id));
    if (!recipe) return;

    activeModalRecipe = recipe;
    currentScaledServings = recipe.servings || 4;

    renderRecipeModalContent();
    recipeModal.style.display = 'flex';
  }

  function renderRecipeModalContent() {
    if (!activeModalRecipe) return;
    const r = activeModalRecipe;
    const scaleFactor = currentScaledServings / (r.servings || 1);
    const totalTime = (r.prep_time_min || 0) + (r.cook_time_min || 0);
    const diffClass = (r.difficulty || 'Medium').toLowerCase();
    const imgSrc = r.image || getDefaultImageForCategory(r.category, r.title);

    let ingredientsListHtml = '';
    (r.ingredients || []).forEach((ing, idx) => {
      const scaledQty = ing.quantity * scaleFactor;
      const formattedQty = scaledQty % 1 === 0 ? scaledQty.toFixed(0) : scaledQty.toFixed(2);
      ingredientsListHtml += `
        <label class="ingredient-check-item">
          <input type="checkbox" id="ing-check-${idx}">
          <span><strong>${formattedQty} ${escapeHtml(ing.unit || '')}</strong> ${escapeHtml(ing.name)}</span>
        </label>
      `;
    });

    let stepsListHtml = '';
    (r.steps || []).forEach((step, idx) => {
      stepsListHtml += `
        <div class="step-card" id="step-card-${idx}">
          <div class="step-num-bubble">${idx + 1}</div>
          <div class="step-text">${escapeHtml(step)}</div>
        </div>
      `;
    });

    recipeModalContent.innerHTML = `
      <div class="detail-banner">
        <img src="${imgSrc}" alt="${escapeHtml(r.title)}" class="detail-img-thumb">
        <div class="detail-meta-group">
          <h2 class="detail-title">${escapeHtml(r.title)}</h2>
          <div class="detail-pills-row">
            <span class="tag-pill highlight">${escapeHtml(r.category)}</span>
            <span class="tag-pill">${escapeHtml(r.difficulty)}</span>
            <span class="tag-pill">⏱️ Prep: ${r.prep_time_min}m | Cook: ${r.cook_time_min}m</span>
            <span class="tag-pill">Total: ${totalTime} mins</span>
          </div>
        </div>
      </div>

      <!-- Interactive Dynamic Serving Scaler -->
      <div class="scaler-box">
        <div class="scaler-info">
          <h4>⚖️ Dynamic Servings Scaler</h4>
          <p>Original portion is ${r.servings} servings. Adjusting automatically recalculates all ingredient weights & measurements.</p>
        </div>
        <div class="scaler-controls">
          <button class="scaler-btn" id="btn-scaler-minus" title="Decrease Servings">−</button>
          <span class="scaler-display" id="scaler-servings-display">${currentScaledServings}</span>
          <button class="scaler-btn" id="btn-scaler-plus" title="Increase Servings">+</button>
        </div>
      </div>

      <div class="detail-columns">
        <div class="detail-section">
          <h4>
            <span>🥗 Ingredients (${(r.ingredients || []).length})</span>
            <small style="font-size:0.75rem; color:var(--text-muted); font-weight:normal;">Check off as you prep</small>
          </h4>
          <div id="modal-ingredients-container">
            ${ingredientsListHtml}
          </div>
        </div>

        <div class="detail-section">
          <h4>
            <span>📝 Instructions (${(r.steps || []).length} steps)</span>
            <small style="font-size:0.75rem; color:var(--text-muted); font-weight:normal;">Click step when finished</small>
          </h4>
          <div id="modal-steps-container">
            ${stepsListHtml}
          </div>
        </div>
      </div>
    `;

    // Attach Scaler listeners
    document.getElementById('btn-scaler-minus').addEventListener('click', () => {
      if (currentScaledServings > 1) {
        currentScaledServings--;
        renderRecipeModalContent();
      }
    });

    document.getElementById('btn-scaler-plus').addEventListener('click', () => {
      if (currentScaledServings < 100) {
        currentScaledServings++;
        renderRecipeModalContent();
      }
    });

    // Checkbox styling listeners
    document.querySelectorAll('.ingredient-check-item input[type="checkbox"]').forEach(chk => {
      chk.addEventListener('change', (e) => {
        const item = e.target.closest('.ingredient-check-item');
        if (e.target.checked) item.classList.add('checked');
        else item.classList.remove('checked');
      });
    });

    // Step cards click listeners
    document.querySelectorAll('.step-card').forEach(card => {
      card.addEventListener('click', () => {
        card.classList.toggle('completed');
      });
    });
  }

  /* ==========================================================================
     PANTRY INGREDIENT MATCHER
     ========================================================================== */

  function renderPantryTags() {
    const allIngs = new Set();
    recipes.forEach(r => {
      (r.ingredients || []).forEach(i => {
        const clean = i.name.toLowerCase().trim();
        if (clean.length > 2) allIngs.add(clean);
      });
    });

    // Top pantry staples
    const sorted = Array.from(allIngs).sort();
    pantryTagList.innerHTML = '';

    sorted.forEach(name => {
      const tag = document.createElement('span');
      tag.className = 'pantry-tag' + (selectedPantryIngredients.has(name) ? ' selected' : '');
      tag.textContent = name;
      tag.addEventListener('click', () => {
        if (selectedPantryIngredients.has(name)) {
          selectedPantryIngredients.delete(name);
        } else {
          selectedPantryIngredients.add(name);
        }
        updatePantryState();
      });
      pantryTagList.appendChild(tag);
    });
  }

  function updatePantryState() {
    renderPantryTags();
    if (selectedPantryIngredients.size > 0) {
      pantrySummaryText.textContent = `Selected ${selectedPantryIngredients.size} ingredient(s): ${Array.from(selectedPantryIngredients).join(', ')}`;
    } else {
      pantrySummaryText.textContent = 'Click tags above to match ingredients.';
    }
    renderCards();
  }

  /* ==========================================================================
     ADD / EDIT RECIPE FORM FLOW
     ========================================================================== */

  function openAddRecipeModal() {
    formModalTitle.textContent = 'Add New Recipe';
    document.getElementById('form-recipe-id').value = '';
    recipeEditorForm.reset();

    const ingList = document.getElementById('form-ingredients-list');
    const stepList = document.getElementById('form-steps-list');
    ingList.innerHTML = '';
    stepList.innerHTML = '';

    // Add 2 initial rows
    addIngredientRow('', '', '');
    addIngredientRow('', '', '');
    addStepRow('');
    addStepRow('');

    recipeFormModal.style.display = 'flex';
  }

  function openEditRecipeModal(id) {
    const r = recipes.find(x => x.id === Number(id));
    if (!r) return;

    formModalTitle.textContent = `Edit Recipe #${r.id}: ${r.title}`;
    document.getElementById('form-recipe-id').value = r.id;
    document.getElementById('form-title').value = r.title;
    document.getElementById('form-category').value = r.category;
    document.getElementById('form-prep-time').value = r.prep_time_min;
    document.getElementById('form-cook-time').value = r.cook_time_min;
    document.getElementById('form-servings').value = r.servings;
    document.getElementById('form-difficulty').value = r.difficulty || 'Medium';

    const ingList = document.getElementById('form-ingredients-list');
    const stepList = document.getElementById('form-steps-list');
    ingList.innerHTML = '';
    stepList.innerHTML = '';

    (r.ingredients || []).forEach(i => addIngredientRow(i.name, i.quantity, i.unit));
    (r.steps || []).forEach(s => addStepRow(s));

    if ((r.ingredients || []).length === 0) addIngredientRow('', '', '');
    if ((r.steps || []).length === 0) addStepRow('');

    recipeFormModal.style.display = 'flex';
  }

  function addIngredientRow(name = '', qty = '', unit = '') {
    const container = document.getElementById('form-ingredients-list');
    const row = document.createElement('div');
    row.className = 'dynamic-row';
    row.innerHTML = `
      <input type="text" class="ing-name" placeholder="Name (e.g. Garlic cloves)" value="${escapeHtml(String(name))}" style="flex:2;" required>
      <input type="number" step="any" min="0" class="ing-qty" placeholder="Qty (e.g. 2)" value="${qty}" style="flex:1;" required>
      <input type="text" class="ing-unit" placeholder="Unit (cups, grams)" value="${escapeHtml(String(unit))}" style="flex:1;">
      <button type="button" class="remove-row-btn" title="Remove">&times;</button>
    `;
    row.querySelector('.remove-row-btn').addEventListener('click', () => row.remove());
    container.appendChild(row);
  }

  function addStepRow(text = '') {
    const container = document.getElementById('form-steps-list');
    const row = document.createElement('div');
    row.className = 'dynamic-row';
    row.innerHTML = `
      <input type="text" class="step-text-input" placeholder="Describe this cooking step..." value="${escapeHtml(String(text))}" style="flex:1;" required>
      <button type="button" class="remove-row-btn" title="Remove">&times;</button>
    `;
    row.querySelector('.remove-row-btn').addEventListener('click', () => row.remove());
    container.appendChild(row);
  }

  async function handleRecipeFormSubmit(e) {
    e.preventDefault();

    const idVal = document.getElementById('form-recipe-id').value;
    const title = document.getElementById('form-title').value.trim();
    const category = document.getElementById('form-category').value.trim() || 'General';
    const prep = parseInt(document.getElementById('form-prep-time').value, 10) || 0;
    const cook = parseInt(document.getElementById('form-cook-time').value, 10) || 0;
    const servings = parseInt(document.getElementById('form-servings').value, 10) || 4;
    const diff = document.getElementById('form-difficulty').value;

    // Collect ingredients
    const ingredients = [];
    document.querySelectorAll('#form-ingredients-list .dynamic-row').forEach(row => {
      const name = row.querySelector('.ing-name').value.trim();
      const qty = parseFloat(row.querySelector('.ing-qty').value) || 1;
      const unit = row.querySelector('.ing-unit').value.trim();
      if (name) ingredients.push({ name, quantity: qty, unit });
    });

    // Collect steps
    const steps = [];
    document.querySelectorAll('#form-steps-list .dynamic-row').forEach(row => {
      const val = row.querySelector('.step-text-input').value.trim();
      if (val) steps.push(val);
    });

    if (idVal) {
      // Editing existing
      const existing = recipes.find(r => r.id === Number(idVal));
      if (existing) {
        existing.title = title;
        existing.category = category;
        existing.prep_time_min = prep;
        existing.cook_time_min = cook;
        existing.servings = servings;
        existing.difficulty = diff;
        existing.ingredients = ingredients;
        existing.steps = steps;
      }
      showToast(`Updated "${title}" successfully!`, 'success');
    } else {
      // Add new
      const nextId = recipes.reduce((max, r) => Math.max(max, r.id || 0), 0) + 1;
      const newRecipe = {
        id: nextId,
        title,
        category,
        prep_time_min: prep,
        cook_time_min: cook,
        servings,
        difficulty: diff,
        image: getDefaultImageForCategory(category, title),
        ingredients,
        steps
      };
      recipes.unshift(newRecipe);
      showToast(`Added "${title}" to your recipe book!`, 'success');
    }

    saveToLocalStorage();
    recipeFormModal.style.display = 'none';
    renderAll();

    // If connected to Python server, sync with backend
    if (isBackendConnected) {
      await syncRecipesToBackend();
    }
  }

  function deleteRecipe(id) {
    const r = recipes.find(x => x.id === Number(id));
    if (!r) return;

    if (confirm(`Are you sure you want to delete "${r.title}"?`)) {
      recipes = recipes.filter(x => x.id !== Number(id));
      saveToLocalStorage();
      renderAll();
      showToast(`Deleted "${r.title}".`, 'success');

      if (isBackendConnected) {
        syncRecipesToBackend();
      }
    }
  }

  /* ==========================================================================
     C & C++ COMPATIBLE EXPORT / IMPORT
     ========================================================================== */

  function generateCFormatText() {
    let out = '# RecipeBook Data File (v1.0)\n';
    const nextId = recipes.reduce((max, r) => Math.max(max, r.id || 0), 0) + 1;
    out += `NEXT_ID:${nextId}\n`;
    out += `COUNT:${recipes.length}\n`;

    recipes.forEach(r => {
      out += 'RECIPE_START\n';
      out += `id:${r.id}\n`;
      out += `title:${r.title}\n`;
      out += `category:${r.category}\n`;
      out += `prep_time:${r.prep_time_min}\n`;
      out += `cook_time:${r.cook_time_min}\n`;
      out += `servings:${r.servings}\n`;
      out += `difficulty:${r.difficulty || 'Medium'}\n`;

      const ings = r.ingredients || [];
      out += `ingredients_count:${ings.length}\n`;
      ings.forEach(i => {
        out += `ing:${i.name}|${i.quantity}|${i.unit}\n`;
      });

      const steps = r.steps || [];
      out += `steps_count:${steps.length}\n`;
      steps.forEach(s => {
        out += `step:${s}\n`;
      });
      out += 'RECIPE_END\n';
    });
    return out;
  }

  function downloadFile(content, filename, contentType) {
    const blob = new Blob([content], { type: contentType });
    const a = document.createElement('a');
    a.href = URL.createObjectURL(blob);
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
  }

  async function syncRecipesToBackend() {
    try {
      const res = await fetch('/api/recipes', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(recipes)
      });
      if (res.ok) {
        syncStatusBox.textContent = '✔ Data successfully synchronized with recipes_data.txt!';
      }
    } catch (e) {
      syncStatusBox.textContent = 'Notice: Server sync offline. Stored safely in local browser memory.';
    }
  }

  function parseCFormatText(text) {
    const lines = text.split('\n').map(l => l.trim()).filter(l => l && !l.startsWith('#'));
    const parsed = [];
    let cur = null;

    lines.forEach(line => {
      if (line === 'RECIPE_START') {
        cur = { ingredients: [], steps: [] };
      } else if (line === 'RECIPE_END') {
        if (cur) {
          cur.image = getDefaultImageForCategory(cur.category, cur.title);
          parsed.push(cur);
          cur = null;
        }
      } else if (cur) {
        if (line.startsWith('id:')) cur.id = parseInt(line.slice(3), 10);
        else if (line.startsWith('title:')) cur.title = line.slice(6);
        else if (line.startsWith('category:')) cur.category = line.slice(9);
        else if (line.startsWith('prep_time:')) cur.prep_time_min = parseInt(line.slice(10), 10);
        else if (line.startsWith('cook_time:')) cur.cook_time_min = parseInt(line.slice(10), 10);
        else if (line.startsWith('servings:')) cur.servings = parseInt(line.slice(9), 10);
        else if (line.startsWith('difficulty:')) cur.difficulty = line.slice(11);
        else if (line.startsWith('ing:')) {
          const parts = line.slice(4).split('|');
          if (parts.length >= 3) {
            cur.ingredients.push({
              name: parts[0],
              quantity: parseFloat(parts[1]) || 1,
              unit: parts[2]
            });
          }
        } else if (line.startsWith('step:')) {
          cur.steps.push(line.slice(5));
        }
      }
    });

    return parsed;
  }

  /* ==========================================================================
     EVENT LISTENERS & HELPERS
     ========================================================================== */

  function setupEventListeners() {
    // Search input
    searchInput.addEventListener('input', (e) => {
      searchQuery = e.target.value;
      clearSearchBtn.style.display = searchQuery ? 'block' : 'none';
      renderCards();
    });

    clearSearchBtn.addEventListener('click', () => {
      searchInput.value = '';
      searchQuery = '';
      clearSearchBtn.style.display = 'none';
      renderCards();
    });

    // Category chips
    categoryChips.forEach(chip => {
      chip.addEventListener('click', () => {
        categoryChips.forEach(c => c.classList.remove('active'));
        chip.classList.add('active');
        activeCategory = chip.dataset.category;
        renderCards();
      });
    });

    // Filters & Sort
    difficultySelect.addEventListener('change', (e) => {
      activeDifficulty = e.target.value;
      renderCards();
    });

    sortSelect.addEventListener('change', (e) => {
      activeSort = e.target.value;
      renderCards();
    });

    // Reset filters
    document.getElementById('reset-filters-btn').addEventListener('click', () => {
      searchInput.value = '';
      searchQuery = '';
      clearSearchBtn.style.display = 'none';
      activeCategory = 'ALL';
      categoryChips.forEach(c => c.classList.toggle('active', c.dataset.category === 'ALL'));
      difficultySelect.value = 'ALL';
      activeDifficulty = 'ALL';
      selectedPantryIngredients.clear();
      updatePantryState();
      renderCards();
    });

    // Delegated card actions
    recipesGrid.addEventListener('click', (e) => {
      const viewBtn = e.target.closest('.btn-view-recipe');
      if (viewBtn) {
        openRecipeDetail(viewBtn.dataset.id);
        return;
      }
      const editBtn = e.target.closest('.btn-edit-recipe');
      if (editBtn) {
        openEditRecipeModal(editBtn.dataset.id);
        return;
      }
      const delBtn = e.target.closest('.btn-delete-recipe');
      if (delBtn) {
        deleteRecipe(delBtn.dataset.id);
        return;
      }
    });

    // Modals close triggers
    closeRecipeModalBtn.addEventListener('click', () => recipeModal.style.display = 'none');
    recipeModal.addEventListener('click', (e) => {
      if (e.target === recipeModal) recipeModal.style.display = 'none';
    });

    closeFormModalBtn.addEventListener('click', () => recipeFormModal.style.display = 'none');
    btnCancelForm.addEventListener('click', () => recipeFormModal.style.display = 'none');
    recipeFormModal.addEventListener('click', (e) => {
      if (e.target === recipeFormModal) recipeFormModal.style.display = 'none';
    });

    // Add recipe trigger
    btnOpenAddModal.addEventListener('click', openAddRecipeModal);
    document.getElementById('btn-add-ing-row').addEventListener('click', () => addIngredientRow());
    document.getElementById('btn-add-step-row').addEventListener('click', () => addStepRow());
    recipeEditorForm.addEventListener('submit', handleRecipeFormSubmit);

    // Pantry panel toggle
    btnPantryMode.addEventListener('click', () => {
      pantryPanel.style.display = pantryPanel.style.display === 'none' ? 'block' : 'none';
    });
    closePantryBtn.addEventListener('click', () => {
      pantryPanel.style.display = 'none';
      selectedPantryIngredients.clear();
      updatePantryState();
    });

    // C/C++ Sync Modal
    btnSyncModal.addEventListener('click', () => syncModal.style.display = 'flex');
    closeSyncModalBtn.addEventListener('click', () => syncModal.style.display = 'none');
    syncModal.addEventListener('click', (e) => {
      if (e.target === syncModal) syncModal.style.display = 'none';
    });

    document.getElementById('btn-export-c-format').addEventListener('click', () => {
      const text = generateCFormatText();
      downloadFile(text, 'recipes_data.txt', 'text/plain');
      showToast('Downloaded recipes_data.txt for C/C++ binaries!', 'success');
    });

    document.getElementById('btn-export-json').addEventListener('click', () => {
      downloadFile(JSON.stringify(recipes, null, 2), 'recipes.json', 'application/json');
      showToast('Exported recipes.json', 'success');
    });

    document.getElementById('btn-fetch-c-file').addEventListener('click', async () => {
      syncStatusBox.textContent = 'Connecting to backend...';
      await checkBackendConnection();
      if (isBackendConnected) {
        await fetchRecipesFromBackend();
        renderAll();
        syncStatusBox.textContent = '✔ Auto-synced with C recipes_data.txt!';
      } else {
        syncStatusBox.textContent = 'Server not running. Run: python3 server.py';
      }
    });

    document.getElementById('file-import-input').addEventListener('change', (e) => {
      const file = e.target.files[0];
      if (!file) return;

      const reader = new FileReader();
      reader.onload = (event) => {
        const text = event.target.result;
        try {
          if (file.name.endsWith('.json')) {
            const data = JSON.parse(text);
            if (Array.isArray(data)) {
              recipes = data;
              saveToLocalStorage();
              renderAll();
              showToast('Successfully imported JSON recipes!', 'success');
            }
          } else {
            const parsed = parseCFormatText(text);
            if (parsed.length > 0) {
              recipes = parsed;
              saveToLocalStorage();
              renderAll();
              showToast(`Imported ${parsed.length} recipes from C data file!`, 'success');
            } else {
              showToast('No recipes detected in file.', 'error');
            }
          }
        } catch (err) {
          showToast('Failed to parse file: ' + err.message, 'error');
        }
      };
      reader.readAsText(file);
    });
  }

  function showToast(message, type = 'success') {
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    toast.innerHTML = `<span>${type === 'success' ? '✔' : '⚠️'}</span><span>${escapeHtml(message)}</span>`;
    toastContainer.appendChild(toast);
    setTimeout(() => {
      toast.style.opacity = '0';
      toast.style.transform = 'translateX(100%)';
      toast.style.transition = 'all 0.3s ease';
      setTimeout(() => toast.remove(), 300);
    }, 3200);
  }

  function escapeHtml(str) {
    if (!str) return '';
    return String(str)
      .replace(/&/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;')
      .replace(/"/g, '&quot;')
      .replace(/'/g, '&#039;');
  }

  // Launch on DOM ready
  document.addEventListener('DOMContentLoaded', init);
})();
