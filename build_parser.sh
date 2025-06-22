rm src/parser/aspcore2_lexer.hpp src/parser/aspcore2_parser.c src/parser/aspcore2_parser.hpp
flex -o src/parser/aspcore2_lexer.hpp src/parser/aspcore2.l
bison -y -d -o src/parser/aspcore2_parser.c src/parser/aspcore2.y
bison -y -o src/parser/aspcore2_parser.hpp src/parser/aspcore2.y