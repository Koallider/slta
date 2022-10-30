cd ./src || exit
bison -dv -W ./SimpleTranslator.y
gcc -c SimpleTranslator.tab.c
flex ./SimpleTranslator.lex
gcc -c lex.yy.c
gcc -o SimpleTranslator SimpleTranslator.tab.o lex.yy.o -lm
rm SimpleTranslator.tab.*
rm SimpleTranslator.output
rm lex.yy.*
mv SimpleTranslator ../SimpleTranslator
cd ..