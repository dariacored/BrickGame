###############################################################################
# Основные переменные
###############################################################################
PROJECT_NAME := BrickGame_by_dariacored
VERSION := 2.0 

OS 		:= $(shell uname)
CC 		:= gcc
CXX		:= g++
CFLAGS 	:= -Wall -Wextra -Werror -std=c11
CPFLAGS := -Wall -Wextra -Werror -std=c++20
LFLAGS	:= -lncurses
SQLFLAGS := -lsqlite3
COVERAGE_FLAGS := -fprofile-arcs -ftest-coverage

ifeq ($(OS), Darwin)
	BREW_PREFIX := $(shell brew --prefix)
	CFLAGS += -I$(BREW_PREFIX)/include
	LDFLAGS := -L$(BREW_PREFIX)/lib -lgtest -lgtest_main -lpthread -lm -lgcov
	RPATH_FLAG := -Wl,-rpath,.
	LIB_EXT := .dylib
else
	LDFLAGS := -lgtest -lgtest_main -lpthread -lm -lgcov
	RPATH_FLAG := -Wl,-rpath=.
	LIB_EXT := .so
endif

# Имена для сборки
# tetris
LIB_NAME_TETRIS    := brickgame_tetris
LIB_FULL_NAME_TETRIS := lib$(LIB_NAME_TETRIS)$(LIB_EXT)
# snake
LIB_NAME_SNAKE   := brickgame_snake
LIB_FULL_NAME_SNAKE := lib$(LIB_NAME_SNAKE)$(LIB_EXT)

# Имена бинарников
EXEC_NAME_CLI_TETRIS := cli_tetris
EXEC_NAME_CLI_SNAKE  := cli_snake
EXEC_NAME_DESKTOP_TETRIS := desktop_tetris
EXEC_NAME_DESKTOP_SNAKE := desktop_snake
EXEC_DESKTOP := desktop_exec
EXEC_TEST := snake_tests

# Директории проекта
SRC_DIR     := .
TETRIS_DIR 	:= $(SRC_DIR)/brick_game/tetris
SNAKE_DIR 	:= $(SRC_DIR)/brick_game/snake
CLI_DIR		:= $(SRC_DIR)/gui/cli
DESKTOP_DIR	:= $(SRC_DIR)/gui/desktop
TEST_DIR 	:= $(SRC_DIR)/tests
COV_DIR     := $(SRC_DIR)/coverage
DVI_DIR		:= $(SRC_DIR)/dvi
CMAKE_DIR	:= $(SRC_DIR)/cmake_build
DIST_DIR	:= $(SRC_DIR)/$(PROJECT_NAME)_$(VERSION)
INSTALL_DIR ?= $(SRC_DIR)/$(PROJECT_NAME)

# Исходные файлы библиотек
# tetris
LIB_SRC_FILES_TETRIS := $(wildcard $(TETRIS_DIR)/*.c)
LIB_OBJECTS_TETRIS   := $(LIB_SRC_FILES_TETRIS:.c=.o)
LIB_SRC_FILES_TETRIS_ADAPTER := $(wildcard $(TETRIS_DIR)/*.cc)
LIB_OBJECTS_TETRIS_ADAPTER   := $(LIB_SRC_FILES_TETRIS_ADAPTER:.cc=.o)
#snake
LIB_SRC_FILES_SNAKE := $(wildcard $(SNAKE_DIR)/*.cc)
LIB_OBJECTS_SNAKE   := $(LIB_SRC_FILES_SNAKE:.cc=.o)

# Исходные файлы интерфейсов
# cli
GUI_CLI_SRC := $(CLI_DIR)/frontend.c
GUI_CLI_OBJ   := $(GUI_CLI_SRC:.c=.o)
GUI_CLI_MAIN_TETRIS_SRC := $(CLI_DIR)/cli_tetris.c
GUI_CLI_MAIN_TETRIS_OBJ   := $(GUI_CLI_MAIN_TETRIS_SRC:.c=.o)
GUI_CLI_MAIN_SNAKE_OBJ := $(CLI_DIR)/cli_snake.cc
GUI_CLI_OBJ_SNAKE   := $(GUI_CLI_MAIN_SNAKE_OBJ:.cc=.o)
# desktop

# Тестовые файлы
TEST_MAIN     := $(TEST_DIR)/snake_tests.cc
TEST_MAIN_OBJ := $(TEST_MAIN:.cc=.o)
TEST_FILES    := $(filter-out $(TEST_MAIN), $(wildcard $(TEST_DIR)/*.cc))
TEST_OBJ_FILES:= $(TEST_FILES:.cc=.o)

###############################################################################
# Основные цели
###############################################################################

.PHONY: all info install uninstall clean dvi dist test gcov_report run_tetris run_snake

all: info

info:
	@echo "Проект: $(PROJECT_NAME) версия $(VERSION)"
	@echo "Доступные цели:"
	@echo "  install         - Установка игры в систему"
	@echo "  uninstall       - Удаление игры из системы"
	@echo "  clean           - Очистка проекта от артефактов сборки"
	@echo "  dvi             - Генерация документации"
	@echo "  dist            - Создание дистрибутива (архив tar.gz)"
	@echo "  test            - Запуск unit-тестов"
	@echo "  gcov_report     - Генерация отчета о покрытии кода"
	@echo "  select          - Запуск меню выбора исполняемого файла"
	@echo "  run_cli_tetris  - Запуск Тетриса в консольном режиме"
	@echo "  run_cli_snake   - Запуск Змейки в консольном режиме"
	@echo "  run_desktop_tetris - Запуск Тетриса в десктопном режиме (Qt)"
	@echo "  run_desktop_snake  - Запуск Змейки в десктопном режиме (Qt)"
	@echo "  mem_check       - Проверка на утечки памяти"
	@echo "  format_check    - Проверка стиля кода"
	@echo "  format          - Автоформатирование кода"

select:
	@choice=$$(whiptail --title "BrickGame Selection" --menu "Choose game and interface:" 12 36 5 \
	"1" "CLI Snake" \
	"2" "CLI Tetris" \
	"3" "DESKTOP Snake" \
	"4" "DESKTOP Tetris" \
	3>&1 1>&2 2>&3); \
	if [ $$? -eq 0 ]; then \
		if [ "$$choice" = "1" ]; then \
			$(MAKE) run_cli_snake; \
		elif [ "$$choice" = "2" ]; then \
			$(MAKE) run_cli_tetris; \
		elif [ "$$choice" = "3" ]; then \
			$(MAKE) run_desktop_snake; \
		elif [ "$$choice" = "4" ]; then \
			$(MAKE) run_desktop_tetris; \
		fi; \
	else \
		echo "Selection interrupted"; \
	fi

install: $(EXEC_NAME_CLI_TETRIS) $(EXEC_NAME_CLI_SNAKE) $(EXEC_DESKTOP) $(LIB_FULL_NAME_TETRIS) $(LIB_FULL_NAME_SNAKE)
	@mkdir -p $(INSTALL_DIR)
	@mv $(EXEC_NAME_CLI_TETRIS) $(INSTALL_DIR)
	@mv $(EXEC_NAME_CLI_SNAKE) $(INSTALL_DIR)
	@mv $(EXEC_NAME_DESKTOP_TETRIS) $(INSTALL_DIR)
	@mv $(EXEC_NAME_DESKTOP_SNAKE) $(INSTALL_DIR)
	@cp $(LIB_FULL_NAME_TETRIS) $(INSTALL_DIR)
	@cp $(LIB_FULL_NAME_SNAKE) $(INSTALL_DIR)
	@echo "Установка завершена в $(INSTALL_DIR)"

uninstall:
	@echo "Удаление сборки игры..."
	@rm -rf $(INSTALL_DIR)
	@echo "Сборка удалена"

clean: remove_artifacts uninstall remove_dist
	@echo "Очистка завершена"

dvi:
	@echo "Генерация документации..."
	@cd dvi && makeinfo brickgame.texi
	@cd ..
	@info $(DVI_DIR)/brickgame.info
	@echo "Документация сохранена в $(DVI_DIR)."

dist:
	@echo "Создание дистрибутива..."
	@mkdir -p $(DIST_DIR)
	@cp -r $(SRC_DIR)/brick_game $(DIST_DIR)
	@cp -r $(SRC_DIR)/gui $(DIST_DIR)
	@cp $(SRC_DIR)/*.h $(DIST_DIR)
	@cp $(SRC_DIR)/Makefile $(DIST_DIR)
	@tar -czf "$(PROJECT_NAME)_$(VERSION).tar.gz" $(DIST_DIR)
	@rm -rf $(DIST_DIR)
	@echo "Дистрибутив создан: $(PROJECT_NAME)_$(VERSION).tar.gz."


test: $(LIB_FULL_NAME_SNAKE) $(EXEC_TEST)
	@echo "Запуск тестов snake библиотеки..."
	@./$(EXEC_TEST)
	@echo "Тесты snake библиотеки завершены."

gcov_report: clean test
	@echo "Запуск тестов для сбора данных покрытия..."
	@-./$(EXEC_TEST)
	@echo "Генерация отчёта..."
	@mkdir -p $(COV_DIR)
	@gcovr -r . \
		--exclude "$(TEST_DIR)/*" \
		--html --html-details -o $(COV_DIR)/index.html \
		--print-summary
	@echo "Отчёт создан: $(COV_DIR)/index.html"

run_cli_tetris:
	./$(INSTALL_DIR)/$(EXEC_NAME_CLI_TETRIS)

run_cli_snake:
	./$(INSTALL_DIR)/$(EXEC_NAME_CLI_SNAKE)

run_desktop_tetris:
	$(INSTALL_DIR)/$(EXEC_NAME_DESKTOP_TETRIS)

run_desktop_snake:
	./$(INSTALL_DIR)/$(EXEC_NAME_DESKTOP_SNAKE)

###############################################################################
# Вспомогательные цели
###############################################################################

.PHONY: $(LIB_FULL_NAME_TETRIS) $(LIB_FULL_NAME_SNAKE) $(EXEC_TEST) $(EXEC_NAME_CLI) $(EXEC_NAME_DESKTOP)

$(LIB_FULL_NAME_TETRIS): $(LIB_OBJECTS_TETRIS) $(LIB_OBJECTS_TETRIS_ADAPTER)
	$(CC) -shared -o $@ $^ $(SQLFLAGS)

$(LIB_FULL_NAME_SNAKE): $(LIB_OBJECTS_SNAKE)
	$(CXX) -shared -o $@ $^ $(SQLFLAGS) $(COVERAGE_FLAGS)

$(EXEC_TEST): $(TEST_MAIN_OBJ) $(TEST_OBJ_FILES) $(LIB_FULL_NAME_SNAKE)
	$(CXX) -o $@ $(TEST_MAIN_OBJ) $(TEST_OBJ_FILES) -L. -l$(LIB_NAME_SNAKE) $(LDFLAGS) $(COVERAGE_FLAGS) $(RPATH_FLAG)

$(EXEC_NAME_CLI_TETRIS): $(GUI_CLI_OBJ) $(GUI_CLI_MAIN_TETRIS_OBJ) $(LIB_FULL_NAME_TETRIS)
	$(CC) -o $@ $(GUI_CLI_OBJ) $(GUI_CLI_MAIN_TETRIS_OBJ) -L. -l$(LIB_NAME_TETRIS) $(LFLAGS) $(SQLFLAGS) $(RPATH_FLAG)

$(EXEC_NAME_CLI_SNAKE): $(GUI_CLI_OBJ) $(GUI_CLI_OBJ_SNAKE) $(LIB_FULL_NAME_SNAKE)
	$(CXX) -o $@ $(GUI_CLI_OBJ) $(GUI_CLI_OBJ_SNAKE) -L. -l$(LIB_NAME_SNAKE) $(LFLAGS) $(SQLFLAGS) $(RPATH_FLAG)

$(EXEC_DESKTOP):
	@mkdir $(CMAKE_DIR)
	@cd $(CMAKE_DIR) && cmake ../$(DESKTOP_DIR) && make
	@cp $(CMAKE_DIR)/$(EXEC_NAME_DESKTOP_TETRIS) $(SRC_DIR)/$(EXEC_NAME_DESKTOP_TETRIS)
	@cp $(CMAKE_DIR)/$(EXEC_NAME_DESKTOP_SNAKE) $(SRC_DIR)/$(EXEC_NAME_DESKTOP_SNAKE)

remove_artifacts:
	@echo "Удаление артефактов сборки..."
	@find . \( -name "*.o" -o -name "*.gcno" -o -name "*.gcda" -o -name "*.db" -o -name "*.info" \
	          -o -name "$(LIB_FULL_NAME_TETRIS)" \
	          -o -name "$(LIB_FULL_NAME_SNAKE)" \
	          -o -name "$(EXEC_NAME_CLI_TETRIS)" \
	          -o -name "$(EXEC_NAME_CLI_SNAKE)" \
	          -o -name "$(EXEC_NAME_DESKTOP)" \
	          -o -name "$(EXEC_TEST)" \
			  -o -name "$(EXEC_NAME_DESKTOP_SNAKE)" \
	          -o -name "$(EXEC_NAME_DESKTOP_TETRIS)" \) -exec rm -f {} +
	@rm -rf $(COV_DIR) $(CMAKE_DIR) $(DIST_DIR)
	@echo "Артефакты сборки удалены"

remove_dist:
	@echo "Удаление дистрибутива..."
	@rm -f "$(PROJECT_NAME)_$(VERSION).tar.gz"
	@echo "Дистрибутив удалён"

###############################################################################
# Правила компиляции объектных файлов
###############################################################################

$(TETRIS_DIR)/%.o: $(TETRIS_DIR)/%.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(TETRIS_DIR)/%.o: $(TETRIS_DIR)/%.cc
	$(CXX) $(CPFLAGS) -fPIC -c $< -o $@

$(SNAKE_DIR)/%.o: $(SNAKE_DIR)/%.cc
	$(CXX) $(CPFLAGS) -fPIC $(COVERAGE_FLAGS) -c $< -o $@

$(CLI_DIR)/%.o: $(CLI_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DESKTOP_DIR)/%.o: $(DESKTOP_DIR)/%.cc
	$(CXX) $(CPFLAGS) -c $< -o $@

$(TEST_DIR)/%.o: $(TEST_DIR)/%.cc
	$(CXX) $(CPFLAGS) -c $< -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CPFLAGS) -c $< -o $@

###############################################################################
# Вспомогательные цели
###############################################################################

.PHONY: mem_check format_check format

# Проверка на утечки
mem_check:
	@echo "Тест на корректную работу с памятью запущен..."
	valgrind --tool=memcheck --leak-check=yes ./$(EXEC_TEST)
	@echo "Тест на корректную работу с памятью завершен."

# Стилевые тесты
format_check:
	@echo "Стилевая проверка кода..."
	cp ../materials/linters/.clang-format .clang-format
	find . -name "*.cc" -o -name "*.h" | xargs clang-format-18 -n --Werror
	rm .clang-format
	@echo "Стилевая проверка завершена."

# Форматирование кода
format:
	@echo "Форматирование кода..."
	cp ../materials/linters/.clang-format .clang-format
	find . -name "*.cc" -o -name "*.h" | xargs clang-format-18 -i --Werror
	rm .clang-format
	@echo "Форматирование кода завершено."