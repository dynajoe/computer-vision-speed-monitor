#include <placeholder.h>
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("1 + 1 = {}", placeholder::add(1, 1));
    
    return 0;
}