# Discrete Structure Project: An Exploration of Graph Algorithms
> A C++ implementation of the Bellman-Ford algorithm and an analysis of the Traveling Salesman Problem (TSP) using dynamic programming.

## Objective: Understand how to use Bellman-Ford algorithm and also learn to solve the Traveling Salesman Problem using algorithms.

## Algorithms used in my version.
- Bellman-Ford for shortest path problem
- For the TSM:
    + Branch & Bound algorithm for graph with < 20 vertices.
    + Multi-Opt optimization with ... for graph with >= 20 vertices.

## TIPS: The more vertices the graph have, the time needed for exact solution is also increase. Therefore, as it's difficult to find exact solution fast without TIMEOUT, we can aim to find algorithms that is fast and mostly accurate without providing the exact solution, to avoid TIMEOUT.
> For example, Held-Karp provide exact solution, but with O(2^n * n^2) time complexity. Although it's faster than brute forcing, but for large-scale graph like more than 15 vertices, we have to change the algorithm to LKH, Ant Colony, or even Greedy Algorithm with alternative changes.
> Sorry for bad English btw.
