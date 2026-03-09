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

#include <gtest/gtest.h>
#include "bumblebee/common/types/Graph.hpp"

using namespace bumblebee;

TEST(GraphTest, AddVertexIncreasesVertexCount) {
    Graph g(false);
    g.addVertex("A");
    g.addVertex("B");
    g.addVertex("C");

    EXPECT_EQ(g.getNumVertices(), 3);
}

TEST(GraphTest, AddEdgeCreatesDirectedConnection) {
    Graph g(false);
    g.addEdge("A", "B", 10);

    auto& edges = g.getEdges();
    idx_t idxA = g.addVertex("A");
    idx_t idxB = g.addVertex("B");

    ASSERT_TRUE(edges[idxA].contains(idxB));
    EXPECT_EQ(edges[idxA][idxB], 10);
}

TEST(GraphTest, RemoveEdgeDeletesConnection) {
    Graph g(false);
    g.addEdge("X", "Y", 5);
    g.removeEdge("X", "Y");

    idx_t idxX = g.addVertex("X");
    idx_t idxY = g.addVertex("Y");

    EXPECT_FALSE(g.getEdges()[idxX].contains(idxY));
}

TEST(GraphTest, GetDotFormatGeneratesValidDotOutput) {
    Graph g(false);
    g.addEdge("A", "B", 1);
    g.addEdge("B", "C", -1);

    std::string dot = g.getDotFormat("TestGraph");

    EXPECT_NE(dot.find("digraph TestGraph"), std::string::npos);
    EXPECT_NE(dot.find("\"A\"->\"B\" [color=blue];"), std::string::npos);
    EXPECT_NE(dot.find("\"B\"->\"C\" [color=red];"), std::string::npos);
    EXPECT_NE(dot.find("}"), std::string::npos);
}

TEST(GraphTest, StrongComponentDetectionWorks) {
    Graph g(false);
    g.addEdge("A", "B", 1);
    g.addEdge("B", "C", 1);
    g.addEdge("C", "A", 1);  // cycle
    g.addEdge("D", "E", 1);  // separate

    auto components = g.calculateStrongComponent();

    ASSERT_GE(components.size(), 2);

    // check if the cycle is detected and contains A B and C
    bool foundCycle = false;
    for (auto& comp : components) {
        if (comp.size() == 3 &&
            std::find(comp.begin(), comp.end(), "A") != comp.end() &&
            std::find(comp.begin(), comp.end(), "B") != comp.end() &&
            std::find(comp.begin(), comp.end(), "C") != comp.end()) {
            foundCycle = true;
            }
    }
    EXPECT_TRUE(foundCycle);
}

TEST(GraphTest, TopologicalSortReturnsValidOrder) {
    Graph g(false);
    g.addEdge("A", "B", 1);
    g.addEdge("B", "C", 1);
    g.addEdge("D", "E", 1);

    auto sorted = g.calculateTopologicalSort();

    // Flatten the result
    std::unordered_map<std::string, int> position;
    int pos = 0;
    for (const auto& level : sorted) {
        for (const auto& node : level) {
            position[node] = pos++;
        }
    }

    // Check topological order constraints
    EXPECT_LT(position["A"], position["B"]);
    EXPECT_LT(position["A"], position["C"]);
    EXPECT_LT(position["B"], position["C"]);
    EXPECT_LT(position["D"], position["E"]);
}

