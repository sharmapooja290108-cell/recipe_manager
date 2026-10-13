CXX = clang++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -Iinclude
TARGET = ../bin/recipe_manager_cpp
SRCS = src/main.cpp src/Recipe.cpp src/RecipeBook.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p ../bin
	$(CXX) $(CXXFLAGS) -o $@ $^

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	@$(TARGET)

.PHONY: all clean run
