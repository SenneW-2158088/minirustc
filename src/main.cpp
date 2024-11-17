#include "lexer/Scanner.hpp"
#include "parser/parser.h"
#include "driver/Driver.hpp"

int main(int argc, char* argv[]) {

    // read options
    MRC::Driver driver{};
    driver.parse("test 88 test test test test");

    return 0;
}
