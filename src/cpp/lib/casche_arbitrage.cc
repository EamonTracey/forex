#include <cmath>
#include <string>
#include <vector>

#include "casche_arbitrage.h"

#include <spdlog/spdlog.h>

CascheArbitrage::CascheArbitrage(
    std::vector<std::vector<std::string>> string_cycles) {
    // Bijectively map the string pairs to numeric IDs in the range [0,
    // n_pairs). The pairs to track are all pairs present in the cycles.
    uint32_t pair = 0;
    for (const auto &string_cycle : string_cycles) {
        for (const auto &string : string_cycle) {
            if (string_to_pair_.count(string) == 0) {
                string_to_pair_[string] = pair;
                pair_to_string_.push_back(string);
                ++pair;
            }
        }
    }

    // Set the number of pairs and number of cycles. It is reasonable to limit
    // these to 32-bit unsigned integers.
    n_pairs_ = pair;
    n_cycles_ = static_cast<uint32_t>(string_cycles.size());

    // Construct two maps: pair-to-cycles and cycle-to-pairs. The
    // pair-to-cycles map tracks to which cycles a given pair belongs. The
    // cycle-to-pairs map tracks the ordered sequence of pairs that belong to a
    // given cycle.
    uint32_t cycle = 0;
    for (uint32_t _ = 0; _ < n_pairs_; ++_)
        pair_to_cycles_.push_back({});
    for (uint32_t _ = 0; _ < n_cycles_; ++_)
        cycle_to_pairs_.push_back({});
    for (const auto &string_cycle : string_cycles) {
        for (const auto &string : string_cycle) {
            uint32_t pair = string_to_pair_.at(string);
            pair_to_cycles_[pair].push_back(cycle);
            cycle_to_pairs_[cycle].push_back(pair);
        }
        ++cycle;
    }

    // Initialize NAN exchange rate for every pair.
    for (uint32_t _ = 0; _ < n_pairs_; ++_)
        pair_to_rate_.push_back(NAN);

    // Initialize NAN profit for every cycle.
    for (uint32_t _ = 0; _ < n_cycles_; ++_)
        cycle_to_profit_.push_back(NAN);
}

CascheArbitrage::~CascheArbitrage() {
}

const std::unordered_map<std::string, uint32_t> &
CascheArbitrage::string_to_pair() const {
    return string_to_pair_;
}

const std::vector<std::string> &CascheArbitrage::pair_to_string() const {
    return pair_to_string_;
}

const std::vector<std::vector<uint32_t>> &
CascheArbitrage::pair_to_cycles() const {
    return pair_to_cycles_;
}

const std::vector<std::vector<uint32_t>> &
CascheArbitrage::cycle_to_pairs() const {
    return cycle_to_pairs_;
}

const std::vector<double> &CascheArbitrage::pair_to_rate() const {
    return pair_to_rate_;
}

const std::vector<double> &CascheArbitrage::cycle_to_profit() const {
    return cycle_to_profit_;
}

void CascheArbitrage::Update(std::string string_pair, double rate) {
    uint32_t pair = string_to_pair_[string_pair];

    // Update the exchange rate for the pair.
    pair_to_rate_[pair] = rate;

    // For each cycle to which the pair belongs, update the cycle profit. This
    // involves multiplying the exchange rates of all pairs in each cycle. The
    // profit must be NAN if any exchange rate is NAN.
    for (uint32_t cycle : pair_to_cycles_[pair]) {
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

bool CascheArbitrage::NewArbitrage(std::string string_pair, double rate) {
    uint32_t pair = string_to_pair_[string_pair];
    double old_rate = pair_to_rate_[pair];
    if (isnan(old_rate))
        return false;

    // If a cycle has profit p, then an arbitrage opportunity exists iff p > 1.
    // If we update a currency pair's exchange rate in the cycle, then the
    // resulting profit equals p * new_rate / old_rate.
    double new_old_ratio = rate / old_rate;

    for (uint32_t cycle : pair_to_cycles_[pair]) {
        double profit = cycle_to_profit_[cycle];
        if (isnan(profit))
            continue;
        if (profit * new_old_ratio > 1)
            return true;
    }

    return false;
}
