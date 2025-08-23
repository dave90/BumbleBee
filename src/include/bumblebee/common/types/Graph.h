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
using strong_comp_vec_t = vector<vector<string>>;
using top_sort_vec_t = vector<vector<string>>;
using index_vertex_map_t = vector<string>;

class Graph {
private:

    using vertex_map_t = std::unordered_map<string,idx_t, StringTHash>;
    using adjacency_list_t = vector<std::unordered_map<idx_t, edge_weight_t>>;

public:
    Graph(bool update_policy);
    void addEdge(string v1, string v2, edge_weight_t weight);
    idx_t addVertex(string v);
    void removeEdge(string v1, string v2);
    void removeEdge(idx_t v1,idx_t v2);
    adjacency_list_t& getEdges();
    std::string getDotFormat(std::string graphName);
    unsigned getNumVertices();
    unsigned getNumEdges();
    strong_comp_vec_t calculateStrongComponent();
    top_sort_vec_t calculateTopologicalSort();
    string& getValue(idx_t v);

private:
    strong_comp_vec_t tarjan();
    vector<vector<idx_t>> kahn();
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
