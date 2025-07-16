all: 
	@echo Building tinky...
	@cd tinky & nmake
	@echo Building winkey...
	@cd winkey & nmake

tinky:
	@echo Building tinky...
	@cd tinky & nmake

winkey:
	@echo Building winkey...
	@cd winkey & nmake

clean:
	@echo Cleaning up...
	@cd tinky & nmake clean
	@cd winkey & nmake clean

fclean: 
	@echo Performing full clean...
	@cd tinky & nmake fclean
	@cd winkey & nmake fclean

re: fclean all
	@echo Rebuilding everything...

.PHONY: all tinky winkey clean fclean re
