#include "bfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>
#include <vector>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1
#define MAX_THREAD_NUM 4
#define alpha 14
#define beta 24

void vertex_set_clear(vertex_set *list)
{
    list->count = 0;
}

void vertex_set_init(vertex_set *list, int count)
{
    list->max_vertices = count;
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}

// Take one step of "top-down" BFS.  For each vertex on the frontier,
// follow all outgoing edges, and add all neighboring vertices to the
// new_frontier.
void top_down_step(
    Graph g,
    vertex_set *frontier,
    vertex_set *new_frontier,
    int *distances)
{
    int add_d = distances[frontier->vertices[0]]+1;
    std::vector<int> buffer[MAX_THREAD_NUM];
    {
#pragma omp parallel for schedule(dynamic,1028)
        for (int i = 0; i < frontier->count; i++)
        {
            int node = frontier->vertices[i];
            int tid = omp_get_thread_num();
            int start_edge = g->outgoing_starts[node];
            int end_edge = (node == g->num_nodes - 1)
                               ? g->num_edges
                               : g->outgoing_starts[node + 1];

            // attempt to add all neighbors to the new frontier
            for (int neighbor = start_edge; neighbor < end_edge; neighbor++)
            {
                int outgoing = g->outgoing_edges[neighbor];
                if(distances[outgoing]==NOT_VISITED_MARKER){
                    __sync_bool_compare_and_swap(&distances[outgoing],NOT_VISITED_MARKER,add_d);
                    //may push_back the same node from different threads, but it doesn't change the result
                    buffer[tid].push_back(outgoing);
                }
            }

        }
        int *v = new_frontier->vertices;
#pragma omp parallel for
        for(int i=0;i<MAX_THREAD_NUM;++i){
            int len = buffer[i].size();
            if(len==0)
                continue;
            int index = __sync_fetch_and_add(&new_frontier->count,len);
            for(int j=0;j<len;++j){
                v[index+j]=buffer[i][j];
            }
        }
    }
}

// Implements top-down BFS.
//
// Result of execution is that, for each node in the graph, the
// distance to the root is stored in sol.distances.
void bfs_top_down(Graph graph, solution *sol)
{

    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    // initialize all nodes to NOT_VISITED
    //for (int i = 0; i < graph->num_nodes; i++)
    //    sol->distances[i] = NOT_VISITED_MARKER;
    memset(sol->distances,NOT_VISITED_MARKER,graph->num_nodes*sizeof(int));

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0)
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);

        top_down_step(graph, frontier, new_frontier, sol->distances);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        std::swap(frontier,new_frontier);
    }
}

void bottom_up_step(
    Graph g,
    vertex_set *frontier,
    vertex_set *new_frontier,
    int *distances,
    bool *f)
{
    int add_d = distances[frontier->vertices[0]]+1;
    std::vector<int> buffer[MAX_THREAD_NUM];
#pragma omp parallel for
    for(int i=0;i<g->num_nodes;++i){
        if(distances[i]==NOT_VISITED_MARKER){
            int tid = omp_get_thread_num();
            int start_edge = g->incoming_starts[i];
            int end_edge = (i==g->num_nodes-1) ? g->num_edges : g->incoming_starts[i+1];
            for(int neighbor = start_edge;neighbor < end_edge; ++neighbor){
                int in_node = g->incoming_edges[neighbor];
                if(f[in_node]){
                    distances[i] = add_d;
                    buffer[tid].push_back(i);
                    break;
                }
            }
        }
    }

    int *v = new_frontier->vertices;
#pragma omp parallel for
    for(int i=0;i<MAX_THREAD_NUM;++i){
        int len = buffer[i].size();
        if(len==0)
            continue;
        int index = __sync_fetch_and_add(&new_frontier->count,len);
        for(int j=0;j<len;++j){
            v[index+j]=buffer[i][j];
        }
    }
}
void bfs_bottom_up(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    bool *f = (bool *)calloc(graph->num_nodes,sizeof(bool));

    memset(sol->distances,NOT_VISITED_MARKER,graph->num_nodes*sizeof(int));
    // initialize all nodes to NOT_VISITED
    //for (int i = 0; i < graph->num_nodes; i++)
    //    sol->distances[i] = NOT_VISITED_MARKER;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;
    f[ROOT_NODE_ID] = true;
    while (frontier->count != 0)
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);

        bottom_up_step(graph, frontier, new_frontier, sol->distances,f);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif
        memset(f,0,sizeof(f));
        int *v = new_frontier->vertices;
        for(int i=0;i<new_frontier->count;++i){
            f[v[i]] = true;
        }
        std::swap(frontier,new_frontier);

    }
}

inline int count_in_edge(const int id, Graph & g){
    int start = g->incoming_starts[id];
    int end = (id==g->num_nodes-1) ? g->num_edges : g->incoming_starts[id+1];
    return end - start;
}

inline int count_out_edge(const int id, Graph & g){
    int start = g->outgoing_starts[id];
    int end = (id==g->num_nodes-1) ? g->num_edges : g->outgoing_starts[id+1];
    return end - start;
}

void bfs_hybrid(Graph graph, solution *sol)
{

    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    bool *f = (bool *)calloc(graph->num_nodes,sizeof(bool));

    // initialize all nodes to NOT_VISITED
    //for (int i = 0; i < graph->num_nodes; i++)
    //    sol->distances[i] = NOT_VISITED_MARKER;
    memset(sol->distances,NOT_VISITED_MARKER,graph->num_nodes*sizeof(int));

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;
    f[ROOT_NODE_ID] = true;
    
    int edge_frontier_num = graph->outgoing_starts[ROOT_NODE_ID+1] - graph->outgoing_starts[ROOT_NODE_ID];//m_f
    int node_frontier_num = 1;//n_f
    int edge_unvisited_num = graph->num_edges - edge_frontier_num;//m_u
    int node_num = graph->num_nodes;
    bool state = 0;//0 for top-down ; 1 for botttom up
    while (frontier->count != 0)
    {

        vertex_set_clear(new_frontier);
        if(state==0){//top down
            if((float)edge_frontier_num > (float)edge_unvisited_num/alpha){
                bottom_up_step(graph,frontier,new_frontier,sol->distances,f);
                state = 1;
            }else{
                top_down_step(graph, frontier, new_frontier, sol->distances);
            }
        }else{//bottom up
            if((float)node_frontier_num < (float)node_num/beta){
                top_down_step(graph, frontier, new_frontier, sol->distances);
                state = 0;
            }else{
                bottom_up_step(graph,frontier,new_frontier,sol->distances,f);
            }
        }



        memset(f,0,sizeof(f));
        int *v = new_frontier->vertices;
        for(int i=0;i<new_frontier->count;++i){
            f[v[i]] = true;
        }

        //update metrics
        node_frontier_num = new_frontier->count;
        edge_frontier_num = 0;
        edge_unvisited_num = 0;
        for(int i=0;i<node_num;++i){
            if(f[i]){
                edge_frontier_num += count_out_edge(i,graph);        
            }else if(sol->distances[i]==NOT_VISITED_MARKER){
                edge_unvisited_num += count_in_edge(i,graph);
            }
        }

        std::swap(frontier,new_frontier);

    }
}
