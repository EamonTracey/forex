#ifndef FOREX_ARBITRAGE_H
#define FOREX_ARBITRAGE_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class ForexArbitrage {
  public:
    ForexArbitrage(std::vector<std::string> currencies);
    ~ForexArbitrage();

    void Update(std::string currency1, std::string currency2,
                double exchange_rate);
    bool IsArbitragePossible();

  private:
    uint32_t n_currencies_;
    uint32_t n_nodes_;
    double **graph_;
    double *distance_;
    double *predecessor_;

    std::unordered_map<std::string, uint32_t> currency_to_id_;
};

#endif
