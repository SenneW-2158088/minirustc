#include "lexer/Scanner.hpp"
#include "parser/parser.h"

int main(int argc, char* argv[]) {
    MRC::Scanner scanner;
    MRC::Parser parser(scanner);
    parser();

    return 0;
}
