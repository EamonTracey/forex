#ifndef CASCHE_ARBITRAGE_H
#define CASCHE_ARBITRAGE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class CascheArbitrage {
  public:
    // CascheArbitrage accepts a list of currency pair cycles to track. The
    // object tracks the profitability of these cycles such that it can quickly
    // determine if an arbitrage opportunity arises given new market data.
    // Further, it can output a table of currency pairs, rate thresholds, and
    // cycles such that if the exchange rate for a currency pair exceeds a
    // threshold, the corresponding cycle should be traded immediately. This
    // paradigm pairs well with an FPGA.
    CascheArbitrage(std::vector<std::vector<std::string>> cycles);

    // Destructor.
    ~CascheArbitrage();

    // Update the state to reflect a new exchange rate for a currency pair.
    void Update(std::string pair, double rate);

    // Determine if an arbitrage opportunity arises given a new exchange rate
    // for a currency pair. This will always return false if the previous
    // exchange rate is NAN.
    bool NewArbitrage(std::string pair, double rate);

    const std::unordered_map<std::string, uint32_t> &string_to_pair() const;
    const std::vector<std::string> &pair_to_string() const;
    const std::vector<std::vector<uint32_t>> &pair_to_cycles() const;
    const std::vector<std::vector<uint32_t>> &cycle_to_pairs() const;
    const std::vector<double> &pair_to_rate() const;
    const std::vector<double> &cycle_to_profit() const;

  private:
    // The number of cycles to track.
    uint32_t n_cycles_;
    // The number of pairs within tracked cycles.
    uint32_t n_pairs_;

    std::unordered_map<std::string, uint32_t> string_to_pair_;
    std::vector<std::string> pair_to_string_;

    std::vector<std::vector<uint32_t>> pair_to_cycles_;
    std::vector<std::vector<uint32_t>> cycle_to_pairs_;

    std::vector<double> pair_to_rate_;
    std::vector<double> cycle_to_profit_;
};

#endif
