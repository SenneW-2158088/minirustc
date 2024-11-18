#include "lexer/Scanner.h"
#include "parser/parser.h"
#include "driver/Driver.h"

int main(int argc, char* argv[]) {

    // read options
    MRC::Driver driver{};
    driver.parse("123");
    driver.parse("123i32");
    driver.parse("123u32");
    driver.parse("123_u32");
    driver.parse("0xff");
    driver.parse("0xff_u8");
    driver.parse("0x01_f32");
    driver.parse("0x01_e3");
    driver.parse("0o70");
    driver.parse("0o70_i16");
    driver.parse("0b1111_1111_1001_0000");
    driver.parse("0b1111_1111_1001_0000i64");

    return 0;
}
