
NAME = infiniteStatisticalMineSweeper

all: SFML SDL

re: reSFML reSDL

run: SFML SDL
	cd buildSDL && ./$(NAME) & cd buildSFML && ./$(NAME)


reSFML:
	rm -rf buildSFML
	mkdir buildSFML; cd buildSFML && cmake -D SDL_DISPLAY=OFF .. && make -j6
	cp -r ./font ./buildSFML

SFML:
	cd buildSFML && rm -f $(NAME)
	cd buildSFML && make -j6 || clear && make

runSFML: SFML
	cd buildSFML && ./$(NAME)

SFMLValgrind: SFML
	##########################################################
	#                       VALGRIND                         #
	##########################################################
	cd buildSFML && valgrind --track-origins=yes ./$(NAME)

reSDL:
	rm -rf buildSDL
	mkdir buildSDL; cd buildSDL && cmake -D SDL_DISPLAY=ON .. && make -j6
	cp -r ./font ./buildSDL

SDL:
	cd buildSDL
	cd buildSDL && make -j6 || clear && make

runSDL: SDL
	cd buildSDL && ./$(NAME)

SDLValgrind: SDL
	##########################################################
	#                       VALGRIND                         #
	##########################################################
	cd buildSDL && valgrind --track-origins=yes ./$(NAME)



hellgrind: fclean
	clear
	g++ -g -o $(NAME) $(SRC) $(FLAGS)
	valgrind --tool=helgrind --read-var-info=yes ./$(NAME) | cat -e #&> valgrind_log

callgrind: fclean
	rm -f callgrind.*
	clear
	g++ -g -o $(NAME) $(SRC) $(FLAGS)
	-valgrind --tool=callgrind ./$(NAME)
	-kcachegrind callgrind.*


callgrindO6: fclean
	rm -f callgrind.*
	clear
	g++ -g -o $(NAME) $(SRC) $(FLAGS) -O6
	-valgrind --tool=callgrind ./$(NAME)
	-kcachegrind callgrind.*
