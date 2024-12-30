#include "tst.h"
#include <exception>

using namespace MRC::TST;

void insert_entry(Context& context, Entry entry) {
    // Check if entry exists
    if(context.symbol_table.contains(entry.symbol)) {
        throw new std::exception();
    }

}

void print_context(Context &context) {
    for(const auto& [symbol, entry] : context.symbol_table) {
        std::cout << symbol << ":" << entry.type.to_string() << std::endl;
    }
}
