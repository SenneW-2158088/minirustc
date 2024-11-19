#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "driver/Driver.h"

#include <cstdio>

int main(int argc, char* argv[]) {

    // read options
    MRC::Driver driver{};
    driver.parse("let a;");
    driver.parse("let a = 30;");
    driver.parse("let a: i32;");
    driver.parse("let a: i32 = 32;");
    return 0;
}
