#include <iostream>
#include <vector>
#include <limits.h>
#include <cmath>

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


//////////////////////////////////////////////////////////////////////////////////////
///////////////////// tworzenie grafu ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


// krawędź u -> v
struct Edge{
    int v;
    int capacity;
    int cost;
    //Edge* counterEdge;   ???
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
    (*g).adj_list[u].push_back(edge);
}


Graph init_graph(int size){
    Graph g;
    g.size = size;
    g.adj_list = new vector<Edge*>[size];
    return g;
}


void printGraph(Graph g){
    for(int i=0; i<g.size; i++){
        for(int j=0; j<g.adj_list[i].size(); j++){
            cout << i << ": " << g.adj_list[i][j]->v << " " << (g.adj_list[i][j]->capacity) << " " << g.adj_list[i][j]->cost << endl;
        }
    }
    cout << endl;
}

//////////////////////////////////////////////////////////////////////////////////////
///////////////////// algorytm Bellmana Forda ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////



void relax(int u, int v, int weight, int *distance, int *parents){
    if (distance[u] != INT_MAX && distance[v] > distance[u] + weight){
        distance[v] = distance[u] + weight;
        parents[v] = u;
    }
}

// weryfikacja niepotrzebna?


bool BellmanFord(Graph *g, int s, int t, int *parents){

    int distance[g->size];


    for(int i=0; i<g->size; i++){
        distance[i] = INT_MAX;
    }

    distance[s] = 0;

    for(int i=0; i<(g->size)-1; i++){
        for(int u=0; u<(g->size); u++){
            for(int edge=0; edge<(g->adj_list[u].size()); edge++){
                if (g->adj_list[u][edge]->capacity != 0){
                    relax(u, g->adj_list[u][edge]->v, g->adj_list[u][edge]->cost,distance,parents);
                }
            }
        }
    }
//    cout << endl<<"distance " <<endl;
//    print_arr(distance, g.size);
//    cout << endl<<"parents " <<endl;
//    print_arr(parents, g.size);

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

   while (BellmanFord(g,s,t,parents)){
        int current = t;
        int cur_flow = INT_MAX;
        while(current != s){
            cur_flow = min(cur_flow, )
                // todooooooooooo
        }
   }
}



//////////////////////////////////////////////////////////////////////////////////////
///////////////////// budowanie grafu turniejowego ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// input: B, t
Graph build_graph(int budget, int entrances){
    int n = entrances;
    Graph g;
    int g_size = int((pow(n,2)+n+6)/2);

    g = init_graph(g_size);
    return g;
}



int main() {

    Graph g;
    g = init_graph(4);
    add_edge(&g,0,1,3,5);
    add_edge(&g,2,1,31,1);
    add_edge(&g,0,2,8,3);
    add_edge(&g,1,3,3,2);
    add_edge(&g,2,3,31,7);
    printGraph(g);


    min_cost_flow(&g,0,1);

    // tournaments
    int T;
    cin >> T;
    for (int i = 0; i < T; i++) {
        // budget, entrances
        int B,n;
        cin >> B >> n;
        Graph g;
        g = build_graph(B,n);
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
        // king_wins == 1
        add_edge(&g,t,t1,int(n*(n-1)/2) - 1,0);

        for (int v=0; v<n; v++){
            if (v==0){
                // king_wins == 1
                add_edge(&g,v,t1,1,0);
            }
            else{
                add_edge(&g,v,t,1,0);
            }
        }
        printGraph(g);
    }




}
