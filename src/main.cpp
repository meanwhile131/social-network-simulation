#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
using namespace std;

#define HELP_EXIT()                                                   \
    cerr << "Использование:\n\""                                      \
         << argv[0] << "\" t m шанс_выбрать_соседа" << endl; \
    return 1;
typedef vector<size_t> Node; // Вершина графа (список смежных ей вершин)

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
        iterations = stoul(argv[1]);     // Количество итераций
        edges_per_iter = stoul(argv[2]); // Количество новых ребер за итерацию (шаг А)
        choose_neighbor_chance = stof(argv[3]); // Шанс выбрать соседа (шаг Б1)
    }
    catch (...)
    {
        HELP_EXIT();
    }
    cout << "m(итераций): " << iterations << "\tt(новых ребер за итерацию): " << edges_per_iter << "\tШанс выбрать соседа: " << choose_neighbor_chance << endl;
    std::random_device rd;
    std::mt19937 gen(rd()); // Генератор случайных чисел

    vector<Node> nodes = {{1, 2}, {0}, {0}}; // Граф
    for (size_t i = 0; i < iterations; i++)
    {
        vector<size_t> weights;
        weights.reserve(nodes.size());
        for (auto i = 0; i < nodes.size(); i++)
        {
            Node node = nodes[i];
            weights.push_back(node.size()); // Вес вершины это её степень
        }

        Node new_node;
        for (auto i = 0; i < edges_per_iter; i++) // Шаг А
        {
            discrete_distribution<size_t> distribution(weights.begin(), weights.end());
            size_t selected_node = distribution(gen); // Выбираем вершину для нового ребра
            new_node.push_back(selected_node);
            nodes[selected_node].push_back(nodes.size()); // Ребро нужно добавить в обе вершины
            weights[selected_node] = 0;                   // Обнуляем вес выбранной вершины, чтобы не выбрать ее снова в этой итерации
        }
        
        nodes.push_back(new_node); // Добавляем новую вершину в граф
    }

    ofstream out;
    out.open("graph.dot");
    out << "graph G {\n  overlap=false node [shape=circle];\n";
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
    return 0;
}