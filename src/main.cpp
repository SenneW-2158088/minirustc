#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "driver/Driver.h"

int main(int argc, char* argv[]) {

    // read options
    MRC::Driver driver{};
    driver.parse("test 88 test test test test");

    return 0;
}
