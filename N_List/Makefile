# Dùng shell mặc định (cmd.exe)
SHELL = cmd.exe

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Iinclude 
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include
TARGET = $(BIN_DIR)\output.exe

FILES_TO_TRY = accidents retail chess OnlineRetail BMS-POS Sale_Fact_sync mushroom connect kosarak PAMP POWERC pumsb SUSY500k SUSY1000K SUSY2000k SUSY3000k SUSY4000k SUSY5000k

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDE_DIR)/%.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q $(BIN_DIR)\*.o $(TARGET)

run: $(TARGET)
	@for %%f in ($(FILES_TO_TRY)) do ( \
		echo.Trying with %%f... & \
		$(TARGET) %%f && echo.Success with %%f || echo.Failed with %%f \
	)
	@echo Done trying all files.

.PHONY: clean run