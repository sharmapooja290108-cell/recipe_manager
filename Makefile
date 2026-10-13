# Gourmet Recipe Book Manager - Master Makefile
# Unified Build & Execution for C, C++, and Web Frontend

PYTHON ?= python3
PORT ?= 5050

.PHONY: all c cpp run-c run-cpp web run-web open-web clean help

all: c cpp
	@echo "\033[32m✔ Both C and C++ Recipe Managers compiled successfully into bin/\033[0m"
	@echo "  To run C version:        make run-c"
	@echo "  To run C++ version:      make run-cpp"
	@echo "  To launch Web Frontend:  make run-web (or open frontend/index.html)"

c:
	@echo "\033[36mCompiling C Recipe Manager...\033[0m"
	@$(MAKE) -C c_version

cpp:
	@echo "\033[35mCompiling C++ Recipe Manager...\033[0m"
	@$(MAKE) -C cpp_version

run-c: c
	@./bin/recipe_manager_c

run-cpp: cpp
	@./bin/recipe_manager_cpp

web: run-web

run-web:
	@echo "\033[33mStarting Gourmet Recipe Book Web Server on http://localhost:$(PORT)...\033[0m"
	@$(PYTHON) server.py

open-web:
	@echo "\033[32mLaunching browser at http://localhost:$(PORT)...\033[0m"
	@open http://localhost:$(PORT) || open frontend/index.html
	@$(PYTHON) server.py

clean:
	@$(MAKE) -C c_version clean
	@$(MAKE) -C cpp_version clean
	@rm -rf bin
	@echo "\033[33mCleaned all build artifacts.\033[0m"

help:
	@echo "Gourmet Recipe Book Manager Options:"
	@echo "  make          - Build both C and C++ binaries"
	@echo "  make run-c    - Launch interactive C Recipe Manager in terminal"
	@echo "  make run-cpp  - Launch interactive C++ Recipe Manager in terminal"
	@echo "  make run-web  - Launch modern Web Frontend connected with C/C++ backend"
	@echo "  make open-web - Open Web Frontend directly in your browser"
	@echo "  make clean    - Remove compiled objects and binaries"
