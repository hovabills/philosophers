CC=cc
NAME=philo
FLAGS = -Wall -Wextra -Werror
HEADER = philo.h
INCLUDE	= -lpthread

SRCS := philo.c philo_init.c utils.c
OBJS := $(SRCS:.c=.o)

$(NAME): $(OBJS)
	$(CC) $(INCLUDE)  -o $(NAME) $(OBJS)

%.o: %.c $(HEADER)
	$(CC) -c $< -o  

all:	$(NAME)

clean:
	rm -f $(OBJS) 

fclean: clean
	rm -f $(NAME) 

re: fclean all

.PHONY:	clean fclean re