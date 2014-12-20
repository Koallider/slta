bison -dv -W Simple.y
gcc -c Simple.tab.c
flex Simple.lex
gcc -c lex.yy.c
gcc -o Simple Simple.tab.o lex.yy.o -lm
