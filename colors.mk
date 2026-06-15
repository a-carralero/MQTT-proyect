GREEN  := \033[0;32m
BLUE   := \033[0;34m
RESET  := \033[0m
define print_green
	@printf "$(GREEN)%s$(RESET)\n" "$(1)"
endef
define print_blue
	@printf "$(BLUE)%s$(RESET)\n" "$(1)"
endef