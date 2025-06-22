#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <algorithm>
#include <format>
#include <filesystem>

using namespace std;

#define HELP_EXIT()                                                                                \
    cerr << "Использование:\n\""                                                                   \
         << argv[0] << "\" t(итераций) m(новых ребер за итерацию) p(шанс выбрать соседа)" << endl; \
    return 1;
typedef vector<size_t> Node; // Вершина графа (список смежных ей вершин)

void saveGraph(std::vector<Node> &nodes, std::string filename = "out/graph.dot")
{
    ofstream graphFile;
    graphFile.open(filename);
    graphFile << "graph G {\n  splines=true;\n  overlap=false;  node [shape=circle];\n";
    for (auto i = 0; i < nodes.size(); ++i)
    {
        for (auto neighbor : nodes[i])
        {
            if (i <= neighbor)
                graphFile << "  " << i << " -- " << neighbor << ";\n";
        }
    }
    graphFile << "}\n";
    graphFile.close();
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
    filesystem::create_directory("out/");
    bernoulli_distribution choose_neighbor(choose_neighbor_chance);
    cout << "t: " << iterations << "\tm: " << edges_per_iter << "\tp: " << choose_neighbor_chance << endl;
    mt19937 gen(0);                          // Генератор случайных чисел
    vector<Node> nodes = {{1, 2}, {0}, {0}}; // Граф

    ofstream statsFile;
    statsFile.open("out/stats.csv");
    statsFile << "Итерация,Треугольники,2-х реберные пути\n";

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

        // Шаг А
        discrete_distribution<size_t> weighted_distribution(weights.begin(), weights.end());
        size_t selected_node = weighted_distribution(gen); // Выбираем вершину для нового ребра
        new_node.push_back(selected_node);
        nodes[selected_node].push_back(nodes.size()); // Ребро нужно добавить в обе вершины
        weights[selected_node] = 0;                   // Обнуляем вес выбранной вершины, чтобы не выбрать ее снова в этой итерации

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
        }
        nodes.push_back(new_node); // Добавляем новую вершину в граф

        // Считаем количество треугольников и путей из 2-х ребер у вершины 0
        size_t triangle_count = 0;
        size_t path2_count = 0;
        // Сортируем списки смежности для более эффективного поиска пересечений
        for (auto &node : nodes)
        {
            std::sort(node.begin(), node.end());
        }
        for (size_t i = 0; i < nodes[0].size(); ++i)
        {
            size_t node1_i = nodes[0][i];
            for (size_t j = i + 1; j < nodes[0].size(); ++j)
            {
                size_t node2_i = nodes[0][j];
                path2_count += 2; // Учитываем пути в обе стороны
                // Проверяем, есть ли ребро между node1_i и node2_i с помощью бинарного поиска
                if (std::binary_search(nodes[node1_i].begin(), nodes[node1_i].end(), node2_i))
                {
                    triangle_count++;
                }
            }
        }
        if (i % 1000 == 1)
        {
            saveGraph(nodes, std::format("out/{}.dot", i));
            statsFile << i << "," << triangle_count << "," << path2_count << "\n";
        }
    }
    saveGraph(nodes);
    statsFile.close();
    return 0;
}
