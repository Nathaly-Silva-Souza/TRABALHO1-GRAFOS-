#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define QTD_VERTICES 10 

// =========================================================
// [LETRA A]: DEFINIÇÃO DA ESTRUTURA DE REPRESENTAÇÃO
// =========================================================

typedef struct noh {
    int rotulo;
    int custo;
    struct noh *prox;
} Noh;

typedef struct {
    Noh **A; // Representação por Lista
    int n;   
} GrafoLista;

typedef struct {
    int matriz[QTD_VERTICES][QTD_VERTICES]; // Representação por Matriz
    int num_vertices; 
} GrafoMatriz;

GrafoLista* inicializarGrafoLista(int n) {
    GrafoLista *G = malloc(sizeof(GrafoLista));
    G->n = n;
    G->A = malloc(n * sizeof(Noh*));
    for (int i = 0; i < n; i++) G->A[i] = NULL;
    return G;
}

void inicializarMatriz(GrafoMatriz *g, int n) {
    g->num_vertices = n;
    for (int i = 0; i < QTD_VERTICES; i++)
        for (int j = 0; j < QTD_VERTICES; j++)
            g->matriz[i][j] = 0;
}

// =========================================================
// [LETRA D]: CÁLCULO DO GRAU DOS VÉRTICES
// =========================================================

void exibirGraus(GrafoMatriz *g, char tipo, int v) {
    printf("\n--- [LETRA D] GRAU DO VERTICE %d ---", v);
    if (tipo == 'D' || tipo == 'd') {
        int entrada = 0, saida = 0;
        for (int i = 0; i < g->num_vertices; i++) {
            if (g->matriz[v][i] != 0) saida++;
            if (g->matriz[i][v] != 0) entrada++;
        }
        printf("\nGrau de Entrada: %d | Grau de Saida: %d\n", entrada, saida);
    } else {
        int grau = 0;
        for (int i = 0; i < g->num_vertices; i++) if (g->matriz[v][i] != 0) grau++;
        printf("\nGrau: %d\n", grau);
    }
}

// =========================================================
// [LETRA E]: AGM PRIM (BASEADO NO SEU PSEUDOCÓDIGO)
// =========================================================

void primAGM(GrafoMatriz *g, char tipo, int valorado) {
    printf("\n--- [LETRA E] ARVORE GERADORA MINIMA (PRIM) ---");
    if (tipo == 'D' || tipo == 'd' || valorado == 0) {
        printf("\nOperacao nao aplicavel (Apenas grafos nao-dirigidos e valorados).\n");
        return;
    }

    int n = g->num_vertices;
    int key[QTD_VERTICES], pi[QTD_VERTICES], Q[QTD_VERTICES];

    for (int u = 0; u < n; u++) {
        key[u] = INT_MAX;
        pi[u] = -1;
        Q[u] = 1; // Q = V
    }
    key[0] = 0; // r = 0

    for (int i = 0; i < n; i++) {
        int u = -1, min = INT_MAX;
        for (int j = 0; j < n; j++) {
            if (Q[j] && key[j] < min) { min = key[j]; u = j; }
        }
        if (u == -1) break;
        Q[u] = 0;

        for (int v = 0; v < n; v++) {
            if (g->matriz[u][v] != 0 && Q[v] && g->matriz[u][v] < key[v]) {
                pi[v] = u;
                key[v] = g->matriz[u][v];
            }
        }
    }
    for (int i = 1; i < n; i++) 
        if (pi[i] != -1) printf("\nAresta: %d - %d | Peso: %d", pi[i], i, key[i]);
    printf("\n");
}

// =========================================================
// FUNÇÕES DE APOIO E CAMINHO
// =========================================================

void adicionarArestaLista(GrafoLista *G, int v1, int v2, int peso) {
    Noh *novo = malloc(sizeof(Noh));
    novo->rotulo = v2;
    novo->custo = peso;
    novo->prox = G->A[v1];
    G->A[v1] = novo;
}

void imprimirCaminho(int v, int *pred) {
    if (v == -1) return;
    imprimirCaminho(pred[v], pred);
    printf("%d ", v);
}

void Dijkstra(GrafoLista *G, int origem, int *dist, int *pred) {
    int i, *R, v, w, custo, tam_R, min_dist;
    Noh *p;
    for (i = 0; i < G->n; i++) { dist[i] = INT_MAX; pred[i] = -1; }
    dist[origem] = 0; R = calloc(G->n, sizeof(int)); tam_R = 0;
    while (tam_R < G->n) {
        min_dist = INT_MAX; v = -1;
        for (i = 0; i < G->n; i++) if (R[i] == 0 && dist[i] < min_dist) { v = i; min_dist = dist[i]; }
        if (v == -1) break;
        R[v] = 1; tam_R++;
        for (p = G->A[v]; p != NULL; p = p->prox) {
            w = p->rotulo; custo = p->custo;
            if (R[w] == 0 && dist[v] != INT_MAX && dist[w] > dist[v] + custo) {
                dist[w] = dist[v] + custo; pred[w] = v;
            }
        }
    }
    printf("\n--- [LETRA F] RESULTADOS DIJKSTRA ---\n");
    for (i = 0; i < G->n; i++) {
        if (dist[i] == INT_MAX) printf("Vertice %d: Inalcancavel\n", i);
        else { printf("Vertice %d: Distancia = %d | Caminho: ", i, dist[i]); imprimirCaminho(i, pred); printf("\n"); }
    }
    free(R);
}

void bfs(GrafoMatriz *g, int verticeInicial) {
    int visitados[QTD_VERTICES] = {0}, fila[QTD_VERTICES], r = 0, e = 0;
    visitados[verticeInicial] = 1; fila[e++] = verticeInicial;
    printf("\n--- [LETRA G] BUSCA EM LARGURA ---\nOrdem de visita: ");
    while (r < e) {
        int vAtual = fila[r++]; printf("%d ", vAtual);
        for (int i = 0; i < g->num_vertices; i++) {
            if (g->matriz[vAtual][i] != 0 && !visitados[i]) { visitados[i] = 1; fila[e++] = i; }
        }
    }
    printf("\n");
}

void dfs(GrafoMatriz *g, int v, int vis[]) {
    vis[v] = 1; printf("%d ", v);
    for (int i = 0; i < g->num_vertices; i++) 
        if (g->matriz[v][i] != 0 && !vis[i]) dfs(g, i, vis);
}

// =========================================================
// MAIN INTEGRADO
// =========================================================
int main() {
    char arqNome[] = "grafo.txt"; 

    FILE *f = fopen(arqNome, "r");
    if (!f) {
        printf("Erro: Arquivo %s nao encontrado!\n", arqNome);
        return 1;
    }

    char tipo; int val, n, v1, v2; float peso_lido;
    fscanf(f, " %c %d %d", &tipo, &val, &n);

    GrafoLista *GL = inicializarGrafoLista(n);
    GrafoMatriz GM; 
    inicializarMatriz(&GM, n);

    while (fscanf(f, "%d %d", &v1, &v2) != EOF) {
        int p = 1;
        if (val == 1) { fscanf(f, "%f", &peso_lido); p = (int)peso_lido; }
        
        adicionarArestaLista(GL, v1, v2, p);
        GM.matriz[v1][v2] = p;
        
        if (tipo == 'G' || tipo == 'g') {
            adicionarArestaLista(GL, v2, v1, p);
            GM.matriz[v2][v1] = p;
        }
    }
    fclose(f);

    int inicial;
    printf("\nInforme o vertice inicial (0 a %d): ", n - 1);
    scanf("%d", &inicial);

    exibirGraus(&GM, tipo, inicial);
    primAGM(&GM, tipo, val);

    int dist[n], pred[n];
    Dijkstra(GL, inicial, dist, pred);
    bfs(&GM, inicial);
    
    int vis[QTD_VERTICES] = {0};
    printf("\n--- [LETRA H] BUSCA EM PROFUNDIDADE ---\nOrdem de visita: ");
    dfs(&GM, inicial, vis);
    printf("\n");

    return 0;
}