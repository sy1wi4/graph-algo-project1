#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
#include <queue>

using namespace std;


void print_arr(int *arr, int size){

    for (int i=0; i<size; i++){
        if (arr[i] != INT_MAX){
            cout  << arr[i] << " ";
        }
        else{
            cout  << "inf ";
        }
    }
    cout << endl;
}


////////////////////////////////////////////////////////////////////////////////
///////////////////// tworzenie grafu //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


// krawędź u -> v
struct Edge{
    int v;
    int capacity;
    int initial_capacity;
    int cost;
    Edge* backEdge;
};

struct Graph{
    int size;   // liczba wierzchołków
    vector<Edge*> *adj_list;
};


void add_edge(Graph *g, int u, int v, int capacity, int cost){

    Edge* edge = new Edge;
    edge->v = v;
    edge->capacity = capacity;
    edge->initial_capacity = capacity;
    edge->cost = cost;

    Edge* back_edge = new Edge;
    back_edge->v = u;
    back_edge->capacity = 0;
    back_edge->initial_capacity = 0;
    back_edge->cost = -1*cost;

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


void printGraph(Graph* g){
    for(int i=0; i<g->size; i++){
        for(int j=0; j<g->adj_list[i].size(); j++){
            cout << i << ": " << g->adj_list[i][j]->v << " " << (g->adj_list[i][
                    j]->capacity)
                 << " " << g->adj_list[i][j]->cost << endl;
        }
    }
    cout << endl;
}


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



bool BellmanFord(Graph *g, int s, int t, int *parents, Edge** parentsEdge){

    int distance[g->size];
    for(int i=0; i<g->size; i++){
        distance[i] = INT_MAX;
    }

    distance[s] = 0;

    bool inQueue[g->size];
    for(int i=0; i<g->size; i++){
        inQueue[i] = false;
    }

    queue<int> Q;
    Q.push(s);
    inQueue[s] = true;

// faster BF algorithm

    while (! Q.empty()){
        int u = Q.front();
        Q.pop();
        inQueue[u] = false;
        for(int edge=0; edge<(g->adj_list[u].size()); edge++){
            if (g->adj_list[u][edge]->capacity != 0){
                if (relax(u, g->adj_list[u][edge]->v, g->adj_list[u][edge]->cost,distance,parents)){
                    parentsEdge[g->adj_list[u][edge]->v] = g->adj_list[u][edge];
                    if (!inQueue[g->adj_list[u][edge]->v]){
                        Q.push(g->adj_list[u][edge]->v);
                        inQueue[g->adj_list[u][edge]->v] = true;
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


// odnajduje krawędź u->v
Edge* find_edge(Graph* g, int u, int v){
    for (auto & i : g->adj_list[u]){
        if (i->v == v){
            return i;
        }
    }
    return nullptr;
}

// n - liczba zawodników
int remove_costless_flow(Graph *g,int n){
    int removed = 0;
    for (int v=0; v<n; v++){
        Edge* edge_from_s = find_edge(g,n,v);

        // król
        if (v == 0 && edge_from_s != nullptr){
            Edge* edge_to_t1 = find_edge(g,v,n+2);
            int Min = min(edge_from_s->capacity, edge_to_t1->capacity);
            removed += Min;
            edge_from_s->capacity -= Min;
            edge_from_s->backEdge->capacity += Min;

            edge_to_t1->capacity -= Min;
            edge_to_t1->backEdge->capacity += Min;
        }

        // pozostali
        else if (v != 0 && edge_from_s != nullptr){
            Edge* edge_to_t = find_edge(g,v,n+1);
            Edge* edge_t_to_t1 = find_edge(g,n+1,n+2);
            int Min = min(min(edge_from_s->capacity, edge_to_t->capacity), edge_t_to_t1->capacity);
            removed += Min;
            edge_from_s->capacity -= Min;
            edge_from_s->backEdge->capacity += Min;

            edge_to_t->capacity -= Min;
            edge_to_t->backEdge->capacity += Min;

            edge_t_to_t1->capacity -= Min;
            edge_t_to_t1->backEdge->capacity += Min;
        }
    }
    return removed;
}


int min_cost_flow(Graph *g, int s, int t, int required_flow, int budget){
    int parents[g->size];
    for(int i=0; i<g->size; i++){
        parents[i] = -1;
    }
    int flow = 0;
    int cost = 0;
    Edge* parentsEdge[g->size];
    int removed = remove_costless_flow(g,g->size - 3);

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


        if (cost > budget){
            return -1;
        }
        for(int i=0; i<g->size; i++){
            parents[i] = -1;
        }

    }

    if (flow == required_flow-removed) {
        return cost;
    }
    else{
        return -1;
    }

}


//////////////////////////////////////////////////////////////////////////////////////
///////////////////// budowanie grafu turniejowego ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// input: B, t
Graph build_graph(int entrances){
    int n = entrances;
    Graph g = init_graph(entrances+3);
    return g;
}

// resetuje pojemności krawędzi w grafie do oryginalnych
void next_try(Graph* g){
    for(int i=0; i<g->size; i++){
        for(auto & j : g->adj_list[i]) {
            j->capacity = j->initial_capacity;
        }
    }
}


int main() {

//    Graph g;
//    g = init_graph(4);
//    add_edge(&g,0,1,3,5);
//    add_edge(&g,2,1,31,1);
//    add_edge(&g,0,2,8,3);
//    add_edge(&g,1,3,3,2);
//
//    int parents[g.size];
//    for(int i=0; i<g.size; i++){
//        parents[i] = -1;
//    }
//    Edge* parentsEdge[g.size];
//    BellmanFord(&g,1,3,parents,parentsEdge);
//
//    cout << endl;
//
//    Graph g1;
//    g1 = init_graph(4);
//    add_edge(&g1,0,1,3,5);
//    add_edge(&g1,2,1,31,1);
//    add_edge(&g1,0,2,8,3);
//    add_edge(&g1,1,3,3,2);
//
//    int parents1[g1.size];
//    for(int i=0; i<g1.size; i++){
//        parents[i] = -1;
//    }
//    Edge* parentsEdge1[g1.size];
//    Dijkstra(&g1,1,3,parents,parentsEdge);



// ----------------------------------------------------------------
    // tournaments
    int T;
    cin >> T;
    for (int i = 0; i < T; i++) {
        // budget, entrances
        int B,n;
        cin >> B >> n;
        Graph g = build_graph(n);
        int s = n;
        int t = n+1;
        int t1 = n+2;
        int wins[n];
        for(int j=0; j<n; j++){
            wins[j] = 0;
        }

        int max_king_wins = n-1;

        for (int j = 0; j < n * (n - 1) / 2; j++) {
            // x vs y, winner, bribe
            int x, y, w, b;
            cin >> x >> y >> w >> b;

            if (x==w){
                if (b <= B) add_edge(&g,x,y,1,b);
                else if (b > B and y==0){
                    max_king_wins -= 1;
                }
            }
            else if (y==w){
                if (b <= B) add_edge(&g,y,x,1,b);
                else if (b > B and x==0){
                    max_king_wins -= 1;
                }
            }

            wins[w]++;
        }

        // krawędzi ze źródła do zawodników o pojemności równej liczbie zwycięstw

        for (int v = 0; v<n; v++){
            if (wins[v] > 0){
                add_edge(&g,s,v,wins[v],0);
            }
        }

        int min_king_wins = ceil(static_cast<double>(n-1)/2) ;
        bool found = false;

        for (int w=max(min_king_wins, wins[0]); w < max_king_wins+1; w++) {
            if (w==max(min_king_wins, wins[0])){
                // pierwszy przebieg pętli

                // krawędź z pierwszego ujścia do drugiego
                add_edge(&g, t, t1, int(n * (n - 1) / 2) - w, 0);


                // krawędzi od zawodników do odpowiedniego ujścia
                for (int v = 0; v < n; v++) {

                    if (v == 0) {
                        add_edge(&g, v, t1, w, 0);
                    }
                    else {
                        add_edge(&g, v, t, w, 0);
                    }
                }
            }

            else{
                next_try(&g);

                // aktualizacja krawędzi do t
                for (auto & u : g.adj_list[t]){
                    u->backEdge->capacity = w;
                }

                // aktualizacja krawędzi do t1
                for (auto & u : g.adj_list[t1]){
                    if (u->v == 0) {
                        // od króla
                        u->backEdge->capacity = w;
                    }
                    else if(u->v == t){
                        // od t
                        u->backEdge->capacity = int(n * (n - 1) / 2) - w;
                    }
                }
            }


            int total_cost = min_cost_flow(&g, s, t1, int(n*(n-1)/2),B);

            if (total_cost != -1 && total_cost <= B){
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