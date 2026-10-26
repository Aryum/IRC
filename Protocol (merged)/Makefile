NAME := IRC

MAIN := main.cpp
HDR_DIR = .

ALL_ARC :=	./Server/server.a \
			./Client/client.a \
			./Channel/channel.a \

MAKEFLAGS += --no-print-directory

CC=c++

FLAGS := -Wall -Wextra -Werror

RM := rm -f

blank :=
define NEWLINE

$(blank)
endef

COLOUR_GREEN := \033[1;32m
COLOUR_RED := \033[1;31m
COLOUR_YELLOW := \033[1;33m
COLOUR_BLUE := \033[1;34m
COLOR_WHITE := \033[0m

all: $(NAME)

$(NAME): $(ALL_ARC) $(MAIN)
	@$(CC) $(FLAGS) -o $@ -I$(HDR_DIR) $(MAIN) $(ALL_ARC)
	@echo "$(COLOUR_GREEN)Made main file$(COLOR_WHITE)"

$(ALL_ARC): FORCE
	@echo "$(COLOUR_BLUE)Updating archives of $@$(COLOR_WHITE)"
	-@cd $(dir $@ ) && (!(make -q) && make) || echo "$(COLOUR_YELLOW)	Already updated$(COLOR_WHITE)"

clean:
	@echo "$(COLOUR_YELLOW)Cleaning archives$(COLOR_WHITE)"
	@$(foreach dir,$(dir $(ALL_ARC)), cd $(dir) && make fclean && echo$(NEWLINE)$(NEWLINE))


fclean: 
	@echo "$(COLOUR_YELLOW)Cleaning everything$(COLOR_WHITE)"
	@$(RM) $(NAME)
	@$(foreach dir,$(dir $(ALL_ARC)), cd $(dir) && make fclean && echo$(NEWLINE)$(NEWLINE))

re: clean all
	

.PHONY: all clean fclean re FORCE