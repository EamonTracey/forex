#include <cmath>
#include <string>
#include <vector>

#include "forex_arbitrage.h"

#include <iostream>

ForexArbitrage::ForexArbitrage(std::vector<std::string> currencies) {
    n_currencies_ = static_cast<uint32_t>(currencies.size());
    n_nodes_ = n_currencies_ + 1;

    // Instantiate the graph such that graph_[currency1][currency2] is the
    // negative logarithm of the exchange rate between currency1 and currency2.
    graph_ = new double *[n_nodes_];

    // Include a sentinel node in the graph with a weight 0 edge to every other
    // node. We do this because the Bellman-Ford algorithm finds the shortest
    // distance from a source node to every other connected node, but we only
    // care about finding a negative cycle, so the source node should have
    // access to all other nodes.
    graph_[0] = new double[n_nodes_]{0};

    // Set all edge weights to NAN (no edge) and track the currency to ID map.
    for (uint32_t i = 0; i < n_currencies_; ++i) {
        graph_[i + 1] = new double[n_nodes_]{NAN};
        currency_to_id_[currencies.at(i)] = i + 1;
    }

    // These data structures are used by the Bellman-Ford algorithm.
    distance_ = new double[n_nodes_];
    predecessor_ = new double[n_nodes_];
}

ForexArbitrage::~ForexArbitrage() {
    for (uint32_t i = 0; i < n_currencies_; ++i)
        delete[] graph_[i];
    delete[] graph_;
}

void ForexArbitrage::Update(std::string currency1, std::string currency2,
                            double exchange_rate) {
    uint32_t currency1_id = currency_to_id_.at(currency1);
    uint32_t currency2_id = currency_to_id_.at(currency2);
    double weight = -log(exchange_rate);

    graph_[currency1_id][currency2_id] = weight;
}

bool ForexArbitrage::IsArbitragePossible() {
    // The distance to the source node equals 0. All other distances start at
    // infinity.
    distance_[0] = 0;
    for (uint32_t i = 0; i < n_currencies_; ++i)
        distance_[i + 1] = INFINITY;

    // Relax the edges n_nodes_ - 1 = n_currencies_ times. This ensures that all
    // edges are completely relaxed (unless a negative cycle exists).
    double weight;
    bool relaxed;
    for (uint32_t _ = 0; _ < n_currencies_; ++_) {
        relaxed = false;
        for (uint32_t i = 0; i < n_nodes_; ++i) {
            for (uint32_t j = 0; j < n_nodes_; ++j) {
                if (i == j)
                    continue;

                weight = graph_[i][j];
                if (isnan(weight))
                    continue;

                if (distance_[i] + weight < distance_[j]) {
                    distance_[j] = distance_[i] + weight;
                    relaxed = true;
                }
            }
        }

        // If no edges are relaxed, then there must not be a negative cycle.
        if (!relaxed)
            return false;
    }

    // Try to relax the edges one more time. If any edge relaxes, then there
    // must be a negative cycle.
    for (uint32_t i = 0; i < n_nodes_; ++i) {
        for (uint32_t j = 0; j < n_nodes_; ++j) {
            if (i == j)
                continue;

            weight = graph_[i][j];
            if (isnan(weight))
                continue;

            if (distance_[i] + weight < distance_[j])
                return true;
        }
    }

    return false;
}
