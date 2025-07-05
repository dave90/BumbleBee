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
#include "bumblebee/common/type/Graph.h"

#include <iostream>
#include <stack>
#include <__ostream/basic_ostream.h>

#include "bumblebee/common/Log.h"

namespace bumblebee{

Graph::Graph(bool update_policy)
    : updatePolicy_(update_policy) {
}

void Graph::addEdge(string_t v1, string_t v2, edge_weight_t weight) {
    auto idx1 = addVertex(v1);
    auto idx2 = addVertex(v2);
    // if updatePolicy_ is false and edge already exist do not update
    if (!updatePolicy_ && adj_lst_[idx1].contains(idx2)) return;
    adj_lst_[idx1][idx2] = weight;
}

idx_t Graph::addVertex(string_t v) {
    auto it = vertex_map_.find(v);
    // check if vertex is present
    if ( it != vertex_map_.end()) return it->second;
    // insert the new vertex
    vertex_map_[v] = idx_;
    if (index_map_.size() <= idx_) index_map_.resize(idx_ + 1);
    index_map_[idx_] = v;
    // resize the adj list if is smaller
    if (adj_lst_.size() <= idx_) adj_lst_.resize(idx_ +  1);
    return idx_++;
}

void Graph::removeEdge(string_t v1, string_t v2) {
    removeEdge(addVertex(v1), addVertex(v2));
}

void Graph::removeEdge(idx_t v1, idx_t v2) {
    adj_lst_[v1].erase(v2);
}

Graph::adjacency_list_t & Graph::getEdges() {
    return adj_lst_;
}

std::string Graph::getDotFormat(std::string graphName) {
    std::string dot = "digraph " + graphName + "{\n" ;
    for (unsigned int i = 0; i < adj_lst_.size(); i++) {
        auto&m = adj_lst_[i];
        for (auto& kv:m){
            auto color= (kv.second >= 0)?"blue":"red";
            dot += "\t\"" + getValue(i) + "\"->\"" + getValue(kv.first) +  "\" [color="+color+"];\n";
        }
    }
    return dot+"}"+"\n";
}

unsigned Graph::getNumVertices() {
    return vertex_map_.size();
}

unsigned Graph::getNumEdges() {
    unsigned edges = 0;
    for (auto& kv : adj_lst_)
        edges += kv.size();
    return edges;
}

string_t& Graph::getValue(idx_t v) {
    return index_map_[v];
}

strong_comp_vec_t Graph::calculateStrongComponent() {
    return tarjan();
}


void Graph::logStrongComponent(strong_comp_vec_t& components) {
    std::string scs = "";
    for (auto& sc : components) {
        scs += "Strong component:\t[ ";
        for (auto& vv : sc) {
            scs += vv + " ";
        }
        scs += "]\n";
    }
    LOG_DEBUG(scs.c_str());
}

strong_comp_vec_t Graph::tarjan() {
    const idx_t n = idx_;
    std::vector<int> index(n, -1);
    std::vector<int> lowlink(n, -1);
    std::vector<bool> onStack(n, false);
    std::stack<idx_t> stack;
    int currentIndex = 0;
    strong_comp_vec_t components;

    std::function<void(idx_t)> strongConnect = [&](idx_t v) {
        index[v] = currentIndex;
        lowlink[v] = currentIndex;
        ++currentIndex;
        stack.push(v);
        onStack[v] = true;

        for (const auto& [w, _] : adj_lst_[v]) {
            if (index[w] == -1) {
                strongConnect(w);
                lowlink[v] = std::min(lowlink[v], lowlink[w]);
            } else if (onStack[w]) {
                lowlink[v] = std::min(lowlink[v], index[w]);
            }
        }

        if (lowlink[v] == index[v]) {
            std::vector<string_t> component;
            idx_t w;
            do {
                w = stack.top();
                stack.pop();
                onStack[w] = false;
                component.push_back(getValue(w));
            } while (w != v);
            components.push_back(std::move(component));
        }
    };

    for (idx_t v = 0; v < n; ++v) {
        if (index[v] == -1) {
            strongConnect(v);
        }
    }
    logStrongComponent(components);

    return components;
}

bool Graph::haveIncomingEdges(idx_t v) {
    for (auto& edges: adj_lst_) {
        if (edges.contains(v)) return true;
    }
    return false;
}

void Graph::logTopoSort(top_sort_vec_t& topSort) {
    std::string ssTSort = "Sort: ";
    for (auto& c:topSort) {
        ssTSort += "[ ";
        for (auto& v:c)
            ssTSort += v + ", ";
        ssTSort += "] -> ";
    }
    ssTSort += " __END__";
    LOG_DEBUG(ssTSort.c_str());
}

top_sort_vec_t Graph::calculateTopologicalSort() {
    auto topIndexSort = kahn();
    top_sort_vec_t topSort;

    for (auto& iv : topIndexSort) {
        std::vector<string_t> components;
        for (auto& v : iv)
            components.push_back(getValue(v));
        topSort.push_back(std::move(components));
    }

    logTopoSort(topSort);

    return topSort;
}

std::vector<std::vector<idx_t>> Graph::kahn() {
    std::vector<idx_t> in_degree(adj_lst_.size(), 0);

    // Compute in-degree (number of incoming edges) for each vertex
    for (const auto& neighbors : adj_lst_) {
        for (const auto& edge : neighbors) {
            in_degree[edge.first]++;
        }
    }

    // Queue for vertices with no incoming edges
    std::queue<idx_t> zero_in_degree;
    for (idx_t i = 0; i < in_degree.size(); ++i) {
        if (in_degree[i] == 0) {
            zero_in_degree.push(i);
        }
    }

    std::vector<std::vector<idx_t>> top_order;

    while (!zero_in_degree.empty()) {
        std::vector<idx_t> current_zero;
        while (!zero_in_degree.empty()) {
            idx_t u = zero_in_degree.front();
            zero_in_degree.pop();
            current_zero.push_back(u);
        }
        for (auto& u : current_zero) {
            for (const auto& neighbor : adj_lst_[u]) {
                idx_t v = neighbor.first;
                if (--in_degree[v] == 0) {
                    zero_in_degree.push(v);
                }
            }
        }
        top_order.push_back(std::move(current_zero));

    }

    // Check for cycle (not a DAG)
    size_t total_processed = 0;
    for (const auto& level : top_order) {
        total_processed += level.size();
    }
    if (total_processed!= adj_lst_.size()) {
        throw std::runtime_error("Graph contains a cycle; topological sort not possible.");
    }

    return top_order;
}

}
