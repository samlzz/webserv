
NAME =		test
CC =		c++
CFLAGS =	-Wall -Wextra -std=c++98 -Wunused -Wunused-const-variable

HEADERS =	HttpConnection.hpp \
			Request/HttpRequest.hpp \
			Response/HttpResponse.hpp

SRCS =		HttpConnection.cpp \
			Request/HttpRequest.hpp \
			Response/HttpResponse.hpp \
			main.cpp

OBJS =	$(SRCS:.cpp=.o)

all: $(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS): $(HEADERS)

clean:
	rm -rf $(OBJS)

fclean:	clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re