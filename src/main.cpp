#include "lexer/Scanner.hpp"
#include "parser/Parser.hpp"
int main(int argc, char *argv[]) {
    MRC::Scanner scanner;  // Removed redundant MRC::Scanner
    MRC::Parser parser(scanner);
    parser();
    return 0;
}
