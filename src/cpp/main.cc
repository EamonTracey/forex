#include <iostream>

#include "forex_arbitrage.h"

int main(int argc, char *argv[]) {
    ForexArbitrage forex_arbitrage({"USD", "GBP"});

    std::cout << forex_arbitrage.IsArbitragePossible() << std::endl;
    forex_arbitrage.Update("USD", "GBP", 1);
    std::cout << forex_arbitrage.IsArbitragePossible() << std::endl;
    forex_arbitrage.Update("GBP", "USD", 1.01);
    std::cout << forex_arbitrage.IsArbitragePossible() << std::endl;

    return 0;
}
