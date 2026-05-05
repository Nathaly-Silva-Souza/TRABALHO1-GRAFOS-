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

// =========================================================
// [LETRA D]: CÁLCULO DO GRAU DOS VÉRTICES
// =========================================================

int calcularGrauMatriz(GrafoMatriz *g, int v) {
    int grau = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        if (g->matriz[v][i] == 1) grau++;
    }
    return grau;
}

int calcularGrauLista(GrafoLista *g, int v) {
    int grau = 0;
    Noh *atual = g->A[v];
    while (atual != NULL) {
        grau++;
        atual = atual->prox;
    }
    return grau;
}

// =========================================================
// FUNÇÕES DE APOIO E CRIAÇÃO
// =========================================================

void inicializarMatriz(GrafoMatriz *g, int n) {
    g->num_vertices = n;
    for (int i = 0; i < QTD_VERTICES; i++)
        for (int j = 0; j < QTD_VERTICES; j++)
            g->matriz[i][j] = 0;
}

void adicionarArestaLista(GrafoLista *G, int v1, int v2, int peso) {
    Noh *novo = malloc(sizeof(Noh));
    novo->rotulo = v2;
    novo->custo = peso;
    novo->prox = G->A[v1];
    G->A[v1] = novo;
}

// [LETRA B]: CRIAÇÃO MANUAL
void criarGrafoUsuario(char f[]) {
    FILE *file = fopen(f, "w");
    char tipo; int valorado, vertices, v1, v2; float peso;
    char continuar = 's';

    if (file) {
        printf("\n--- [LETRA B] CRIACAO MANUAL DE GRAFO ---\n");
        printf("Tipo (G para Grafo, D para Digrafo): ");
        scanf(" %c", &tipo);
        printf("Eh valorado? (1-Sim, 0-Nao): ");
        scanf("%d", &valorado);
        printf("Quantidade de vertices: ");
        scanf("%d", &vertices);
        fprintf(file, "%c %d %d\n", tipo, valorado, vertices);

        while (continuar == 's' || continuar == 'S') {
            printf("Informe a aresta (v1 v2): ");
            scanf("%d %d", &v1, &v2);
            if (valorado == 1) {
                printf("Peso: "); scanf("%f", &peso);
                fprintf(file, "%d %d %.2f\n", v1, v2, peso);
            } else fprintf(file, "%d %d\n", v1, v2);
            printf("Deseja inserir outra aresta? (s/n): ");
            scanf(" %c", &continuar);
        }
        fclose(file);
    }
}

// [LETRA C]: GERAR MATRIZ EM TXT
void gerarMatrizDeArquivo(char nomeLeitura[], char nomeEscrita[]) {
    FILE *entrada = fopen(nomeLeitura, "r");
    FILE *saida = fopen(nomeEscrita, "w");
    GrafoMatriz g;
    if (entrada && saida) {
        char tipo; int val, vertices, v1, v2; float peso;
        fscanf(entrada, " %c %d %d", &tipo, &val, &vertices);
        inicializarMatriz(&g, vertices);
        while (fscanf(entrada, "%d %d", &v1, &v2) != EOF) {
            if (val == 1) fscanf(entrada, "%f", &peso); // Peso consumido corretamente
            if (v1 < vertices && v2 < vertices) {
                g.matriz[v1][v2] = 1;
                if (tipo == 'G' || tipo == 'g') g.matriz[v2][v1] = 1;
            }
        }
        for (int i = 0; i < g.num_vertices; i++) {
            for (int j = 0; j < g.num_vertices; j++) fprintf(saida, "%d ", g.matriz[i][j]);
            fprintf(saida, "\n");
        }
        fclose(entrada); fclose(saida);
    }
}

// =========================================================
// ALGORITMOS DE BUSCA E CAMINHO (F, G, H)
// =========================================================

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
            if (g->matriz[vAtual][i] == 1 && !visitados[i]) { visitados[i] = 1; fila[e++] = i; }
        }
    }
    printf("\n");
}

void dfs(GrafoMatriz *g, int v, int vis[]) {
    vis[v] = 1; printf("%d ", v);
    for (int i = 0; i < g->num_vertices; i++) if (g->matriz[v][i] == 1 && !vis[i]) dfs(g, i, vis);
}

// =========================================================
// MAIN INTEGRADO
// =========================================================
// =========================================================
// MAIN INTEGRADO CORRIGIDO
// =========================================================
int main() {
    // DECLARAÇÃO ÚNICA: Nome do arquivo para os testes
    char arqNome[] = "grafo_testeA.txt"; 
    char arqMatriz[] = "matriz.txt";

    // -----------------------------------------------------
    // [LETRA B]: Descomente a linha abaixo no futuro para 
    // criar o grafo manualmente via teclado:
    // criarGrafoUsuario(arqNome); 
    // -----------------------------------------------------

    // 1. [LETRA C]: Gera a matriz de adjacência em arquivo .txt
    gerarMatrizDeArquivo(arqNome, arqMatriz);

    // 2. [LETRA A]: Abre o arquivo para carregar as estruturas de dados
    FILE *f = fopen(arqNome, "r");
    if (!f) {
        printf("Erro: Arquivo %s nao encontrado na pasta do projeto!\n", arqNome);
        return 1;
    }

    char tipo; int val, n, v1, v2; 
    float peso_lido;

    // Lendo o cabeçalho: <tipo> <valorado> <num_vertices>
    if (fscanf(f, " %c %d %d", &tipo, &val, &n) != 3) {
        printf("Erro no formato do cabecalho do arquivo!\n");
        fclose(f);
        return 1;
    }

    // Inicializando as estruturas de dados interna
    GrafoLista *GL = inicializarGrafoLista(n);
    GrafoMatriz GM; 
    inicializarMatriz(&GM, n);

    // Carregando as arestas/arcos do arquivo
    while (fscanf(f, "%d %d", &v1, &v2) != EOF) {
        peso_lido = 1.0; 
        if (val == 1) fscanf(f, "%f", &peso_lido); 
        
        // Alimenta a Lista de Adjacência
        adicionarArestaLista(GL, v1, v2, (int)peso_lido);
        // Alimenta a Matriz de Adjacência
        GM.matriz[v1][v2] = 1;
        
        // Se for Grafo não-dirigido (G), a conexão é mútua
        if (tipo == 'G' || tipo == 'g') {
            adicionarArestaLista(GL, v2, v1, (int)peso_lido);
            GM.matriz[v2][v1] = 1;
        }
    }
    fclose(f);

    // 3. Interface de execução das operações
    int inicial;
    printf("\n--- ARQUIVO CARREGADO COM SUCESSO ---");
    printf("\nInforme o vertice para iniciar as buscas (0 a %d): ", n - 1);
    if (scanf("%d", &inicial) != 1) return 0;

    if (inicial >= 0 && inicial < n) {
        // [LETRA D]: Cálculo de graus
        printf("\n--- [LETRA D] GRAU DO VERTICE %d ---", inicial);
        printf("\nGrau (via Matriz): %d", calcularGrauMatriz(&GM, inicial));
        printf("\nGrau (via Lista): %d\n", calcularGrauLista(GL, inicial));

        // [LETRA F]: Caminho mais curto (Dijkstra)
        int dist[n], pred[n];
        Dijkstra(GL, inicial, dist, pred);

        // [LETRA G]: Busca em Largura (BFS)
        bfs(&GM, inicial);
        
        // [LETRA H]: Busca em Profundidade (DFS)
        int vis[QTD_VERTICES] = {0};
        printf("\n--- [LETRA H] BUSCA EM PROFUNDIDADE ---\nOrdem de visita: ");
        dfs(&GM, inicial, vis);
        printf("\n");
    } else {
        printf("\nVertice invalido!");
    }

    return 0;
}