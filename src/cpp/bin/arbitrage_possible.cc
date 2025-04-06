#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "forex_arbitrage.h"

const std::vector<std::string> CURRENCIES = {"USD", "EUR", "JPY", "GBP", "CNH",
                                             "AUD", "CAD", "CHF", "HKD", "SGD"};

bool parse_line(std::ifstream &csv, std::string &base_currency,
                std::string &quote_currency, double &ask_price,
                double &bid_price, unsigned long int &timestamp);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0]
                  << " <path to Polygon.io CSV forex data>" << std::endl;
        return 1;
    }

    std::ifstream csv(argv[1]);
    std::string _;
    std::getline(csv, _);

    ForexArbitrage forex_arbitrage(CURRENCIES);
    const auto &id_to_currency = forex_arbitrage.id_to_currency();
    struct arbitrage_opportunity arbitrage_opportunity;

    std::string base_currency;
    std::string quote_currency;
    double ask_price;
    double bid_price;
    unsigned long int timestamp;
    unsigned long int last_timestamp = 0;
    while (parse_line(csv, base_currency, quote_currency, ask_price, bid_price,
                      timestamp)) {
        if (timestamp != last_timestamp) {
            last_timestamp = timestamp;
            if (forex_arbitrage.FindArbitrageOpportunity(
                    arbitrage_opportunity)) {
                std::cout << arbitrage_opportunity.profit;
                for (auto &currency : arbitrage_opportunity.currencies)
                    std::cout << " " << id_to_currency.at(currency);
                std::cout << " " << timestamp << std::endl;
                return 1;
            }
        }
        forex_arbitrage.Update(base_currency, quote_currency, bid_price);
    }
    forex_arbitrage.FindArbitrageOpportunity(arbitrage_opportunity);

    return 0;
}

bool parse_line(std::ifstream &csv, std::string &base_currency,
                std::string &quote_currency, double &ask_price,
                double &bid_price, unsigned long int &timestamp) {
    std::string line;
    std::string field;
    std::vector<std::string> fields;

    if (!std::getline(csv, line))
        return false;

    std::stringstream ss(line);
    while (std::getline(ss, field, ','))
        fields.push_back(field);
    if (fields.size() != 5) {
        std::cerr << "Error: The CSV does not have the expected number of "
                     "columns."
                  << std::endl;
        return false;
    }

    std::string ticker;
    try {
        ticker = fields[1];
        ask_price = std::stod(fields[2]);
        bid_price = std::stod(fields[3]);
        timestamp = std::stoul(fields[4]);
    } catch (const std::exception &e) {
        std::cerr << "Error: Failed to convert to numeric types." << std::endl;
        return false;
    }

    if (ticker.size() != 9) {
        std::cerr << "Error: Ticker symbol is malformed." << std::endl;
        return false;
    }

    base_currency = ticker.substr(2, 3);
    quote_currency = ticker.substr(6, 3);

    return true;
}
