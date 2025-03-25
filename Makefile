CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = shelljr
SRCS = main.cpp features/*.cpp
INCLUDES = -Iinclude -I/opt/homebrew/opt/readline/include
LIBS = -L/opt/homebrew/opt/readline/lib -lreadline -lhistory -lcurl

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET)
	rm -f .shell_history

run: $(TARGET)
	./$(TARGET) 