#include <string>
#include <vector>

#include "casche_arbitrage.h"

std::vector<std::vector<std::string>> CYCLES = {{"USD/GBP", "GBP/USD"}};

int main(int argc, char *argv[]) {
    CascheArbitrage casche_arbitrage(CYCLES);
}
