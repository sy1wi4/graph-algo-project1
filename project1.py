import math


# reprezentacja macierzowa grafu

class Edge:
    def __init__(self, capacity, cost):
        self.capacity = capacity
        self.cost = cost
    
class graph:

    def __init__(self,size):
        self.m=[[Edge(None,None)]*size for i in range(size)] 
        self.size=size
    
    def add_edge(self,v,u,capacity,cost):
        self.m[v][u]=Edge(capacity,cost)
    
    def printG(self):
        print(end = "     ")
        for i in range(len(self.m[0])):
            print(i, end="      ")
        print()

        for k in range(self.size):
            print(k, end = " ")
            for j in range(self.size):
                if (self.m[k][j].capacity is None):
                    print(("( ,  )"), end = " ")
                else:
                    print((self.m[k][j].capacity, self.m[k][j].cost), end = " ")
            print()




#--------------------------------------------------------------------
#------------------- ALGORYTM BELLMANA FORDA ------------------------
#--------------------------------------------------------------------


def relax1(u,v,weight,parent,distance) :
    
    if distance[v]>distance[u]+weight :
        distance[v]=distance[u]+weight
        parent[v]=u


# weryfikacja krawędzi z u do v
def verify1(u,v,weight,distance) :
    if distance[v]>distance[u]+weight : return False
    return True


def bellman(g,s,t,parents):
    
    distance=[float("inf")]*g.size
    distance[s]=0

    for _ in range(g.size-1):
        # przeglądamy wszystkie krawędzi
        for vertex in range(g.size) :  
            for v in range(g.size) :
                
                if g.m[vertex][v].capacity != None and g.m[vertex][v].capacity != 0:
                    relax1(vertex,v,g.m[vertex][v].cost,parents,distance)
                 

    # weryfikacja nie jest potrzebna w przypadku naszych grafów turniejowych

    # for vertex in range(g.size) :
    #         for v in range(g.size) :

    #             if g.m[vertex][v].cost != None:
    #                 if not verify1(vertex,v,g.m[vertex][v].cost,distance): 
    #                     print(vertex,v,g.m[vertex][v].cost)
    #                     return False
    

    return distance[t] != float("inf")



#--------------------------------------------------------------------

# maksymalny przepływ z wierzchołka s do t
def min_cost_flow(g,s,t):

    parents = [None]*g.size
    flow = 0
    cost = 0 

    while bellman(g,s,t,parents):
       
        # szukamy krawędzi o najmniejszej pojemności rezydualnej (czyli
        # największego przepływu jaki może być na danej ścieżce)
        # idziemy od ujścia po parentach w górę

        current=t
        cur_flow=float("inf")
        while(current!=s):
            if g.m[parents[current]][current].capacity < cur_flow :
                cur_flow=g.m[parents[current]][current].capacity
    
            current=parents[current]
        

        # po przejściu ścieżki zwiększamy flow o najmniejszą pojemność
        # rezydualną na tej ścieżce (cur_flow)
        
        flow += cur_flow

        # aktualizujemy pojemności rezydualne istniejących krawędzi i
        # krawędzi powrotnych oraz obliczamy łączny koszt krawędzi na danej ścieżce,
        # znowu idziemy od ujścia po parentach w górę
        v=t
        while(v!=s):
            # dodaj
            g.m[parents[v]][v].capacity-=cur_flow
            cost += g.m[parents[v]][v].cost * cur_flow

            # odejmij
            if g.m[v][parents[v]].capacity is None:
                g.add_edge(v,parents[v],cur_flow,-1* g.m[parents[v]][v].cost)

            else:
                g.m[v][parents[v]].capacity += cur_flow
                g.m[v][parents[v]].cost = -1* g.m[parents[v]][v].cost

            v = parents[v]
        for i in range(len(parents)):
            parents[i] = None
        

    print("\nKOSZT:", cost)
    print("flow:", flow,"\n")
    # przepływ równy ilości meczów!!!
    return cost,flow


# na razie całkowicie pomijam koszty

def make_graph(budget, entrances, arr, king_wins):
    n = entrances
    # n wierzchołków zawodników, kolejne (n po 2) wierzchołków meczów -> razem (n**2+n)/2) 
  
    s = int((n**2+n)/2)
    t = s+1
    t1 = t+1
    
    g = graph(int((n**2+n+6)/2))
    
    match = n   # indeks pierwszego wierzchołka meczu
    for u,v,winner,bribe in arr:
        if u == winner:
            g.add_edge(match,u,1,0)
            g.add_edge(match,v,1,bribe)
        else:
            g.add_edge(match,v,1,0)
            g.add_edge(match,u,1,bribe)
        g.add_edge(s,match,1,0)

        match += 1

    g.add_edge(t, t1, int(n*(n-1)/2) - king_wins,0)
    
    for v in range(n):
        if v == 0:
            g.add_edge(v,t1,king_wins,0)
        else:
            g.add_edge(v,t,king_wins,0)
    
    
    # g.printG()
    return g

        

def tournament(budget, entrances, arr):
    n = entrances
    s = int((n**2+n)/2)
    t1 = s+2

    # tyle minimalnie zwycięstw musi mieć król, by miał szansę wygrać
    min_king_wins = math.ceil((n-1)/2) 

    # dla każdej możliwej liczby zwycięstw króla szukamy minimalnej wartości łapówek
    for king_wins in range(min_king_wins, entrances):
        
        g = make_graph(budget, entrances, arr,king_wins)
        cost, flow = min_cost_flow(g,s,t1)

        if cost <= budget and flow == int(n*(n-1)/2):
            print("OK! \nKing wins:", king_wins,"\ncost:",cost,"\nbudget:", budget,"\n")
            return True
    
    return False





# [u, v, winner, bribe]

# min cost: 3
# arr = [[0,1,0,0],[1,2,2,5],[0,2,2,3]]



# min cost: 6
# arr = [
#     [0,1,1,5],[0,3,0,0],[0,2,2,14],
#     [1,2,2,3],[1,3,1,8],[2,3,2,1]]



# min cost: 6
arr = [
    [0,1,0,7],[0,2,2,6],[0,3,3,3],
    [0,4,4,4],[0,5,5,1],[1,2,2,1],
    [1,3,1,4],[1,4,4,4],[1,5,5,5],
    [2,3,2,3],[2,4,2,1],[2,5,2,1],
    [3,4,3,3],[3,5,3,2],[4,5,5,5]]

print(tournament(7,6,arr))
