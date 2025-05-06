#include <cmath>
#include <string>
#include <vector>

#include "casche_arbitrage.h"

CascheArbitrage::CascheArbitrage(std::vector<std::vector<std::string>> cycles) {
    uint32_t pair_id = 0;
    for (auto &cycle : cycles) {
        for (auto &pair : cycle) {
            if (pair_to_id_.count(pair) == 0) {
                pair_to_id_[pair] = pair_id;
                id_to_pair_.push_back(pair);
                ++pair_id;
            }
        }
    }
    n_pairs_ = pair_id;

    // Populate the pair to cycles map. This maps a pair to all the cycles in
    // which it appears.
    for (uint32_t _ = 0; _ < n_pairs_; ++_)
        pair_to_cycles_.push_back({});
    for (uint32_t _ = 0; _ < n_cycles_; ++_)
        cycle_to_pairs_.push_back({});
    uint32_t cycle_id = 0;
    for (auto &cycle : cycles) {
        for (auto &pair : cycle) {
            uint32_t pair_id = pair_to_id_.at(pair);
            pair_to_cycles_[pair_id].push_back(cycle_id);
            cycle_to_pairs_[cycle_id].push_back(pair_id);
        }
        ++cycle_id;
    }
    n_cycles_ = cycle_id;

    // Instantiate NAN exchange rate for every pair.
    for (uint32_t _ = 0; _ < n_pairs_; ++_)
        pair_to_rate_.push_back(NAN);

    // Instantiate NAN profits for every cycle.
    for (uint32_t _ = 0; _ < n_cycles_; ++_)
        cycle_to_profit_.push_back(NAN);
}

CascheArbitrage::~CascheArbitrage() {
}

void CascheArbitrage::Update(std::string pair, double exchange_rate) {
    uint32_t pair_id = pair_to_id_.at(pair);

    pair_to_rate_[pair_id] = exchange_rate;

    for (uint32_t cycle : pair_to_cycles_[pair_id]) {
        double profit = 1;
        for (uint32_t pair : cycle_to_pairs_[cycle]) {
            double rate = pair_to_rate_[pair];
            if (isnan(rate)) {
                profit = NAN;
                break;
            }
            profit *= rate;
        }
        cycle_to_profit_[cycle] = profit;
    }
}

bool CascheArbitrage::NewArbitrage(std::string pair, double exchange_rate) {
    // TODO: Implement.
    return false;
}
