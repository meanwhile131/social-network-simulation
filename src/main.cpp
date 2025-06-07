#include <iostream>
#include <vector>
#include <random>
#include <fstream>
using namespace std;

#define HELP_EXIT()                                                   \
    cerr << "Использование:\n\""                                      \
         << argv[0] << "\" итераций новых_ребер_за_итерацию" << endl; \
    return 1;
typedef vector<size_t> Node; // Вершина графа (список смежных ей вершин)

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        HELP_EXIT();
    }
    size_t iterations;
    size_t edges_per_iter;
    try
    {
        iterations = std::stoi(argv[1]);     // Количество итераций
        edges_per_iter = std::stoi(argv[2]); // Количество новых ребер за итерацию (шаг А)
    }
    catch (...)
    {
        HELP_EXIT();
    }

    std::random_device rd;
    std::mt19937 gen(rd()); // Генератор случайных чисел

    vector<Node> nodes = {{1, 2}, {0}, {0}}; // Граф
    for (size_t i = 0; i < iterations; i++)
    {
        vector<size_t> weights; // Веса вершин
        weights.reserve(nodes.size());
        for (size_t i = 0; i < nodes.size(); i++)
        {
            Node node = nodes[i];
            weights.push_back(node.size()); // Вес вершины это её степень
        }

        Node new_node;
        for (size_t j = 0; j < edges_per_iter; j++) // Шаг А
        {
            discrete_distribution<size_t> distribution(weights.begin(), weights.end());
            size_t selected_node = distribution(gen); // Выбираем вершину для нового ребра
            new_node.push_back(selected_node);
            nodes[selected_node].push_back(nodes.size()); // Ребро нужно добавить в обе вершины
            weights[selected_node] = 0;                   // Обнуляем вес выбранной вершины, чтобы не выбрать ее снова в этой итерации
        }
        nodes.push_back(new_node); // Добавляем новую вершину в граф
    }
    for (auto node : nodes)
    {
        cout << "{";
        for (size_t neighbor : node)
        {
            cout << neighbor << ",";
        }
        cout << "} ";
    }

    ofstream out;
    out.open("graph.dot");
    out << "graph G {\n  overlap=false;\n  node [shape=circle];\n";
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        for (size_t neighbor : nodes[i])
        {
            if (i <= neighbor)
                out << "  " << i << " -- " << neighbor << ";\n";
        }
    }
    out << "}\n";
    out.close();
    return 0;
}