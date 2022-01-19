
NAME = infiniteStatisticalMineSweeper

all:
	cd build && make -j 8 || clear && make

client: all
	./build/bin/$(NAME)

_clear:
	clear

valgrindclient: _clear all
	valgrind --leak-check=full --track-origins=yes ./build/bin/$(NAME)

SUPP = --suppressions=./valSup.supp #--gen-suppressions=all

valgrind: fclean
	clear
	clear
	g++ -g -o $(NAME) $(SRC) $(FLAGS)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=definite $(SUPP) ./$(NAME) | cat -e #&> valgrind_log

hellgrind: fclean
	clear
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
