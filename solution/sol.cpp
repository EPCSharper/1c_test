#include <algorithm>
#include <iostream>
#include <print>
#include <set>
#include <vector>

struct Edge {
    int a;
    int b;
    double p; // Probability of being unusable
};

int cur;
int clearCost;

bool go(int index, const std::vector<Edge>& edges) {
    std::print("Go({})\n", index + 1);
    int res;
    std::cin >> res;
    if (res == 0) {
        return false;
    }
    cur = edges[index].a == cur ? edges[index].b : edges[index].a;
    return true;
}

void clear(int index) {
    std::print("Clear({})\n", index + 1);
}

void end() {
    std::cout << "End\n";
}

std::vector<int> prof;

void dfs(int v, std::vector<bool>& taken, const std::vector<std::vector<int>>& gr, const std::vector<Edge>& edges) {
    taken[v] = true;
    for (auto edgeNum : gr[v]) {
        int to = edges[edgeNum].a == v ? edges[edgeNum].b : edges[edgeNum].a;
        if (!taken[to]) {
            if (go(edgeNum, edges)) {
                dfs(to, taken, gr, edges);
                go(edgeNum, edges); // Go back after visiting dfs subtree of that vertex
            }
        }
    }
}

std::pair<std::vector<double>, std::vector<int>> paths(const std::vector<bool>& taken, const std::vector<std::vector<int>>& gr, const std::vector<Edge>& edges) {
    std::vector<double> dists(gr.size(), -1e18);
    std::vector<int> prevs(gr.size(), -1);
    std::set<std::pair<double, int>> q;
    for (size_t i = 0; i < gr.size(); ++i) {
        if (taken[i]) {
            dists[i] = 0;
            q.emplace(0, i);
        }
    }

    std::vector<bool> curUsed(taken.size(), false);

    while (!q.empty()) {
        auto [cost, v] = *q.begin();
        q.erase(q.begin());
        curUsed[v] = true;

        for (auto edgeNum : gr[v]) {
            int to = edges[edgeNum].a == v ? edges[edgeNum].b : edges[edgeNum].a;
            if (curUsed[to]) {
                continue;
            }
            double newDist = cost + prof[to] - edges[edgeNum].p * clearCost;
            if (dists[to] < newDist) {
                q.erase({dists[to], to});
                dists[to] = newDist;
                prevs[to] = edgeNum;
                q.emplace(dists[to], to);
            }
        }
    }

    return {dists, prevs};
}

void bfsToVertex(int v, const std::vector<std::vector<int>>& gr, const std::vector<Edge>& edges) {
    std::vector<int> dists(gr.size(), 1000000000);
    std::vector<int> prevs(gr.size(), -1);
    dists[v] = 0;
    std::vector<int> q;
    q.emplace_back(v);
    for (int i = 0; i < q.size(); ++i) {
        int curV = q[i];
        for (auto edgeNum : gr[curV]) {
            int to = edges[edgeNum].a == curV ? edges[edgeNum].b : edges[edgeNum].a;
            if (dists[to] > dists[curV] + 1) {
                dists[to] = dists[curV] + 1;
                prevs[to] = edgeNum;
                q.emplace_back(to);
            }
        }
    }

    std::vector<int> path;
    while (v != cur) {
        path.emplace_back(prevs[v]);
        int curEdge = prevs[v];
        v = edges[curEdge].a == v ? edges[curEdge].b : edges[curEdge].a;
    }
    std::reverse(path.begin(), path.end());
    for (int el : path) {
        go(el, edges);
    }
}

bool take1(std::vector<bool>& taken, const std::vector<std::vector<int>>& gr, const std::vector<Edge>& edges) {
    auto [costs, prevs] = paths(taken, gr, edges);
    double maxCost = -1;
    int maxCostV = -1;
    for (int i = 0; i < gr.size(); ++i) {
        double el = costs[i];
        if (el > maxCost) {
            maxCost = el;
            maxCostV = i;
        }
    }

    if (maxCost <= 0 || taken[maxCostV]) {
        return false;
    }

    int prevV = maxCostV;
    int lastEdge;
    while (!taken[maxCostV]) {
        lastEdge = prevs[maxCostV];
        int newV = edges[lastEdge].a == maxCostV ? edges[lastEdge].b : edges[lastEdge].a;
        prevV = maxCostV;
        maxCostV = newV;
    }

    [[maybe_unused]] int toTake = prevV; // For debugging purposes: the vertex we want to take at this step
    bfsToVertex(maxCostV, gr, edges); // move across taken vertices to the one adjacent to that we want to use now
    
    clear(lastEdge); // Clear the edge to toTake vertex
    go(lastEdge, edges); // Taken toTake
    dfs(prevV, taken, gr, edges); // Take all the vertices that we can reach without clearing anything
    return true;
}

int main() {
    int n, m;
    std::cin >> n >> m >> cur >> clearCost;
    --cur;

    prof.resize(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> prof[i];
    }

    std::vector<Edge> edges(m);
    std::vector<std::vector<int>> gr(n);

    for (int i = 0; i < m; ++i) {
        std::cin >> edges[i].a >> edges[i].b >> edges[i].p;
        --edges[i].a;
        --edges[i].b;
        gr[edges[i].a].emplace_back(i);
        gr[edges[i].b].emplace_back(i);
    }

    std::vector<bool> taken(n, false);
    taken[cur] = true;

    // Try to go everywhere we can without clearing anything
    dfs(cur, taken, gr, edges);
    while (take1(taken, gr, edges));
    end();
    return 0;
}