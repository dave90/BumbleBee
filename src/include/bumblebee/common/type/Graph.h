/*
 * Copyright (C) 2025 Davide Fuscà
 *
 * This file is part of BumbleBee.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once
#include <unordered_map>
#include <unordered_set>

#include "bumblebee/common/Hash.h"
#include "bumblebee/common/TypeDefs.h"

namespace bumblebee{

using edge_weight_t = int64_t;
using strong_comp_vec_t = std::vector<std::vector<string_t>>;
using top_sort_vec_t = std::vector<std::vector<string_t>>;
using index_vertex_map_t = std::vector<string_t>;

class Graph {
private:

    using vertex_map_t = std::unordered_map<string_t,idx_t, StringTHash>;
    using adjacency_list_t = std::vector<std::unordered_map<idx_t, edge_weight_t>>;

public:
    Graph(bool update_policy);
    void addEdge(string_t v1, string_t v2, edge_weight_t weight);
    idx_t addVertex(string_t v);
    void removeEdge(string_t v1, string_t v2);
    adjacency_list_t& getEdges();
    std::string getDotFormat(std::string graphName);
    unsigned getNumVertices();
    unsigned getNumEdges();
    strong_comp_vec_t calculateStrongComponent();
    top_sort_vec_t calculateTopologicalSort();

private:
    string_t& getValue(idx_t v);
    strong_comp_vec_t tarjan();
    std::vector<std::vector<idx_t>> kahn();
    bool haveIncomingEdges(idx_t v);

    void logTopoSort(top_sort_vec_t& topSort);
    void logStrongComponent(strong_comp_vec_t& components);

    idx_t idx_{0};
    // dictionary of vertex and index and inverse
    vertex_map_t vertex_map_;
    index_vertex_map_t index_map_;
    // Adjacency list representation of the graph.
    // Each index corresponds to a vertex ID (idx_t), and maps to a dictionary of
    // outgoing edges where keys are destination vertex IDs and values are edge weights.
    adjacency_list_t adj_lst_;

    bool updatePolicy_;

};


}
