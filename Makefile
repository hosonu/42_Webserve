CXX = c++
#CXXFLAGS = -Wall -Werror -Wextra -std=c++98
DEBUGFLAGS = -D DEBUG

ifeq ($(DEBUG),)
	CXXFLAGS +=
else
	CXXFLAGS += $(DEBUGFLAGS)
endif

SRCS = \
    src/main.cpp \
    src/config/Config.cpp \
    src/config/ServerConfig.cpp \
    src/config/ConfigValidator.cpp \
    src/config/Location.cpp \
    src/core/Server.cpp \
    src/core/Socket.cpp \
    src/core/Client.cpp \
    src/http/Request.cpp \
    src/http/Response.cpp \
    src/http/HttpParse.cpp \
    src/http/RequestValidConf.cpp

TARGET = program

OBJECTS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(TARGET)

re: fclean $(TARGET)

.PHONY: all clean fclean re

# OBJ_DIR = src/build/obj
# DEP_DIR = src/build/dep
# OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
# DEPS = $(SRCS:%.cpp=$(DEP_DIR)/%.d)
# all: $(TARGET)
# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# $(OBJ_DIR)/main.o: src/main.cpp | $(OBJ_DIR)/src
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# $(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)/src/$(dir $*) $(DEP_DIR)/src/$(dir $*)
# 	$(CXX) $(CXXFLAGS) -MMD -MF $(DEP_DIR)/$*.d -c $< -o $@

# $(OBJ_DIR)/src/% $(DEP_DIR)/src/%:
# 	mkdir -p $(OBJ_DIR)/src/$(dir $*) $(DEP_DIR)/src/$(dir $*)

# -include $(DEPS)

# clean:
# 	rm -rf src/build

# fclean: clean
# 	rm -rf $(TARGET)

# re: fclean $(TARGET)

# .PHONY: all clean fclean re