#include <iostream>
#include <vector>
#include <climits>
#include <cmath>

using namespace std;


//void print_arr(int *arr, int size){
//
//    for (int i=0; i<size; i++){
//        if (arr[i] != INT_MAX){
//            cout  << arr[i] << " ";
//        }
//        else{
//            cout  << "inf ";
//        }
//    }
//    cout << endl;
//}


//////////////////////////////////////////////////////////////////////////////////////
///////////////////// tworzenie grafu ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


// krawędź u -> v
struct Edge{
    int v;
    int capacity;
    int cost;
    Edge* backEdge;
    bool isBack;
};

struct Graph{
    int size;   // liczba wierzchołków
    vector<Edge*> *adj_list;

};


void add_edge(Graph *g, int u, int v, int capacity, int cost){
    Edge* edge = new Edge;
    edge->v = v;
    edge->capacity = capacity;
    edge->cost = cost;
    edge -> isBack = false;

    Edge* back_edge = new Edge;
    back_edge->v = u;
    back_edge->capacity = 0;
    back_edge->cost = -1*cost;
    back_edge -> isBack = true;
    edge->backEdge = back_edge;
    back_edge->backEdge = edge;
    (*g).adj_list[u].push_back(edge);
    (*g).adj_list[v].push_back(back_edge);
}


Graph init_graph(int size){
    Graph g;
    g.size = size;
    g.adj_list = new vector<Edge*>[size];
    return g;
}


//void printGraph(Graph* g){
//    for(int i=0; i<g->size; i++){
//        for(int j=0; j<g->adj_list[i].size(); j++){
//            cout << i << ": " << g->adj_list[i][j]->v << " " << (g->adj_list[i][j]->capacity)
//            << " " << g->adj_list[i][j]->cost << " " << g->adj_list[i][j]->isBack << endl;
//        }
//    }
//    cout << endl;
//}

//////////////////////////////////////////////////////////////////////////////////////
///////////////////// algorytm Bellmana Forda ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


// true jeśli zrelaksowano, false w p.p.
bool relax(int u, int v, int weight, int *distance, int *parents){
    if (distance[u] != INT_MAX && distance[v] > distance[u] + weight){
        distance[v] = distance[u] + weight;
        parents[v] = u;
        return true;
    }
    return false;
}

// weryfikacja niepotrzebna?


bool BellmanFord(Graph *g, int s, int t, int *parents, Edge** parentsEdge){

    int distance[g->size];


    for(int i=0; i<g->size; i++){
        distance[i] = INT_MAX;
    }

    distance[s] = 0;

    for(int i=0; i<(g->size)-1; i++){
        for(int u=0; u<(g->size); u++){
            for(int edge=0; edge<(g->adj_list[u].size()); edge++){
                if (g->adj_list[u][edge]->capacity != 0){
                    if (relax(u, g->adj_list[u][edge]->v, g->adj_list[u][edge]->cost,distance,parents)){
                        parentsEdge[g->adj_list[u][edge]->v] = g->adj_list[u][edge];
                    }
                }
            }
        }
    }
    return distance[t] != INT_MAX;
}


//////////////////////////////////////////////////////////////////////////////////////
///////////////////// min cost flow //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


int min_cost_flow(Graph *g, int s, int t){
    int parents[g->size];
    for(int i=0; i<g->size; i++){
        parents[i] = -1;
    }
    int flow = 0;
    int cost = 0;
    Edge* parentsEdge[g->size];

   while (BellmanFord(g,s,t,parents,parentsEdge)){
        int current = t;
        int cur_flow = INT_MAX;
        while (current != s){
            Edge* parentEdge = parentsEdge[current];
            cur_flow = min(cur_flow,parentEdge->capacity);
            current = parents[current];
        }

        flow += cur_flow;

        int v = t;
        while (v != s){
            Edge* parentEdge = parentsEdge[v];
            parentEdge->capacity -= cur_flow;
            cost += parentEdge->cost * cur_flow;

            Edge* back_edge = parentsEdge[v]->backEdge;

            back_edge->capacity += cur_flow;
            v = parents[v];
        }

       for(int i=0; i<g->size; i++){
           parents[i] = -1;
       }
   }
   return cost;
}



//////////////////////////////////////////////////////////////////////////////////////
///////////////////// budowanie grafu turniejowego ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// input: B, t
Graph build_graph(int entrances){
    int n = entrances;
    Graph g = init_graph(int((pow(n,2)+n+6)/2));
    return g;
}


Graph copy_g(Graph* g){
    Graph cp = init_graph(g->size);

    for(int i=0; i<g->size; i++) {
        for (int j = 0; j < g->adj_list[i].size(); j++) {
            if (! g->adj_list[i][j]->isBack) {
                add_edge(&cp, i, g->adj_list[i][j]->v, g->adj_list[i][j]->capacity, g->adj_list[i][j]->cost);
            }

        }
    }
    return cp;
}

int main() {

//    Graph g;
//    g = init_graph(4);
//    add_edge(&g,0,1,3,5);
//    add_edge(&g,2,1,31,1);
//    add_edge(&g,0,2,8,3);
//    add_edge(&g,1,3,3,2);

//    min_cost_flow(&g,0,1);



    // tournaments
    int T;
    cin >> T;
    for (int i = 0; i < T; i++) {
        // budget, entrances
        int B,n;
        cin >> B >> n;
        Graph g = build_graph(n);
        int cur_match = n;
        int s = int((pow(n,2)+n)/2);
        int t = s+1;
        int t1 = t+1;

        for (int j = 0; j < n * (n - 1) / 2; j++) {
            // x vs y, winner, bribe
            int x, y, w, b;
            cin >> x >> y >> w >> b;

            if (x==w){
                add_edge(&g,cur_match,x,1,0);
                add_edge(&g,cur_match,y,1,b);
            }
            else{
                add_edge(&g,cur_match,y,1,0);
                add_edge(&g,cur_match,x,1,b);
            }

            add_edge(&g,s,cur_match,1,0);

            cur_match++;
        }

        Graph original_g = copy_g(&g);

        int min_king_wins = ceil(static_cast<double>(n-1)/2) ;
        bool found = false;

        for (int wins=min_king_wins; wins < n; wins++) {
            Graph graph = copy_g(&original_g);

            add_edge(&graph, t, t1, int(n * (n - 1) / 2) - wins, 0);

            for (int v = 0; v < n; v++) {
                if (v == 0) {
                    add_edge(&graph, v, t1, wins, 0);
                } else {
                    add_edge(&graph, v, t, wins, 0);
                }
            }
            int total_cost = min_cost_flow(&graph, s, t1);
            if (total_cost <= B){
                cout << "TAK" << endl;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "NIE" << endl;
        }
    }
    return 0;
}
/*
 * input:
 * 3 5 3 0 1 0 0 1 2 2 5 0 2 2 3 10 4 0 1 1 5 0 3 0 0 0 2 2 14 1 2 2 3 1 3 1 8 2 3 2 1 1 4 0 1 1 5 0 3 0 0 0 2 2 14 1 2 2 3 1 3 1 8 2 3 2 1
 *
 * output :
 * TAK TAK NIE
*/