CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
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
    src/http/RequestValidConf.cpp \
    src/handler/CGIHandler.cpp \
    src/utlis/Utils.cpp

TARGET = webserv

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
