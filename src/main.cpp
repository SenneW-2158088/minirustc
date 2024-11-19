#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "driver/Driver.h"

#include <cstdio>

int main(int argc, char* argv[]) {

    // read options
    MRC::Driver driver{};
    driver.parse("123");
    driver.parse("0.");
    driver.parse("0.29");
    driver.parse(".0"); // should not work
    return 0;
}
