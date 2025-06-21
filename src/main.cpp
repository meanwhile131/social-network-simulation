#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <algorithm>
#include <format>

using namespace std;

#define HELP_EXIT()                                                                                \
    cerr << "Использование:\n\""                                                                   \
         << argv[0] << "\" t(итераций) m(новых ребер за итерацию) p(шанс выбрать соседа)" << endl; \
    return 1;
typedef vector<size_t> Node; // Вершина графа (список смежных ей вершин)

void saveGraph(std::vector<Node> &nodes, std::string filename = "graphs/graph.dot")
{
    ofstream out;
    out.open(filename);
    out << "graph G {\n  splines=true;\n  overlap=false;  node [shape=circle];\n";
    for (auto i = 0; i < nodes.size(); ++i)
    {
        for (auto neighbor : nodes[i])
        {
            if (i <= neighbor)
                out << "  " << i << " -- " << neighbor << ";\n";
        }
    }
    out << "}\n";
    out.close();
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian"); // Исправляет проблемы с выводом киррилицы
    if (argc != 4)
    {
        HELP_EXIT();
    }
    size_t iterations;
    size_t edges_per_iter;
    float choose_neighbor_chance;
    try
    {
        iterations = stoul(argv[1]);            // Количество итераций
        edges_per_iter = stoul(argv[2]);        // Количество новых ребер за итерацию (шаг А)
        choose_neighbor_chance = stof(argv[3]); // Шанс выбрать соседа (шаг Б)
    }
    catch (...)
    {
        HELP_EXIT();
    }
    std::bernoulli_distribution choose_neighbor(choose_neighbor_chance);
    cout << "t: " << iterations << "\tm: " << edges_per_iter << "\tp: " << choose_neighbor_chance << endl;
    std::mt19937 gen(0);                     // Генератор случайных чисел
    vector<Node> nodes = {{1, 2}, {0}, {0}}; // Граф (вершина это вектор её соседей)
    vector<size_t> weights;
    for (size_t i = 0; i < iterations; i++)
    {
        weights.clear();
        weights.reserve(nodes.size());
        std::transform(nodes.begin(), nodes.end(), std::back_inserter(weights), [](const auto& node){ return node.size(); }); // Записывает веса (степени) вершин в weights

        Node new_node;

        // Шаг А
        discrete_distribution<size_t> weighted_distribution(weights.begin(), weights.end());
        size_t selected_node = weighted_distribution(gen); // Выбираем вершину для нового ребра
        new_node.push_back(selected_node);
        nodes[selected_node].push_back(nodes.size()); // Ребро нужно добавить в обе вершины
        weights[selected_node] = 0;                   // Обнуляем вес выбранной вершины, чтобы не выбрать ее снова в этой итерации
        saveGraph(nodes, std::format("graphs/{}_a.dot", i));

        // Шаг Б
        vector<pair<size_t, size_t>> weighted_neighbors; // Вес соседа и его индекс в паре
        weighted_neighbors.reserve(nodes[selected_node].size() - 1);
        for (size_t i = 0; i < nodes[selected_node].size() - 1; ++i)
        {
            size_t neighbor = nodes[selected_node][i];
            weighted_neighbors.emplace_back(weights[neighbor], neighbor);
        }
        sort(weighted_neighbors.begin(), weighted_neighbors.end(), [](const auto &a, const auto &b)
             { return a.first > b.first; }); // Сортируем соседей по весу
        for (size_t j = 0; j < edges_per_iter - 1; j++)
        {
            if (choose_neighbor(gen)) // Б1: Если случайное число < p, то выбираем соседа с наибольшей степенью
            {
                size_t top_neighbor = weighted_neighbors[0].second;
                new_node.push_back(top_neighbor);
                nodes[top_neighbor].push_back(nodes.size());          // Добавляем новое ребро в обе вершины
                weights[top_neighbor] = 0;                            // Обнуляем вес соседа, чтобы не выбрать его снова в Б2
                weighted_neighbors.erase(weighted_neighbors.begin()); // Удаляем соседа из списка, чтобы не выбрать его снова в Б1
            }
            else // Б2: Если случайное число > p, то выбираем случайную вершину (в соответствии с весами)
            {
                discrete_distribution<size_t> distribution(weights.begin(), weights.end());
                size_t selected_node = distribution(gen);
                nodes[selected_node].push_back(nodes.size());
                new_node.push_back(selected_node);
                weights[selected_node] = 0;
            }
            saveGraph(nodes, std::format("graphs/{}_b_{}.dot", i, j));
        }
        nodes.push_back(new_node); // Добавляем новую вершину в граф
    }

    saveGraph(nodes);
    return 0;
}