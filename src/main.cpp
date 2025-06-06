#include <iostream>
#include <vector>
#include <random>
#include <graphviz/gvc.h>
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
    GVC_t *gvc = gvContext(); // Инициализация Graphviz
    char graph_name[] = "graph";
    Agraph_t *g = agopen(graph_name, Agundirected, 0);
    vector<Agnode_t *> g_nodes;
    for (size_t i = 0; i < nodes.size(); i++)
    {
        Node node = nodes[i];
        Agnode_t *n = agnode(g, (char *)to_string(i).c_str(), 1);
        agsafeset(n, "shape", "circle", "");
        agsafeset(n, "width", "0.3", "");
        agsafeset(n, "fixedsize", "true", "");
        g_nodes.push_back(n);
    }
    for (size_t i = 0; i < nodes.size(); i++)
    {
        Node node = nodes[i];
        for (size_t neighbor : node)
        {
            if (neighbor < i) // Избегаем дублирования ребер
                continue;
            agedge(g, g_nodes[i], g_nodes[neighbor], 0, 1);
        }
    }

    gvLayout(gvc, g, "circo");
    gvRenderFilename(gvc, g, "png", "output.png");
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    return 0;
}