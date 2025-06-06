#include <iostream>
#include <vector>
#include <random>
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
    std::mt19937 gen(rd());                  // Генератор случайных чисел
    vector<Node> nodes = {{1, 2}, {0}, {0}}; // Граф
    for (size_t i = 0; i < iterations; i++)
    {
        vector<size_t> weights; // Веса вершин
        for (size_t i = 0; i < nodes.size(); i++)
        {
            Node node = nodes[i];
            size_t edge_count = node.size(); // Степень/вес вершины
            weights.push_back(edge_count);
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
    for (Node node : nodes)
    {
        cout << "{";
        for (size_t neighbor : node)
        {
            cout << neighbor << ",";
        }
        cout << "} ";
    }
    cout << endl;
    return 0;
}