#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define QTD_VERTICES 20 

// =========================================================
// [LETRA A]: DEFINIÇÃO DAS ESTRUTURAS
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

// =========================================================
// [LETRA C]: SALVAR MATRIZ (ESTRUTURADA EM LINHAS E COLUNAS)
// =========================================================
// Esta função grava a matriz no arquivo .txt respeitando o formato
// visual de tabela solicitado na imagem do enunciado.
void salvarMatrizFormato(GrafoMatriz *g) {
    FILE *f_mat = fopen("matriz_adjacencia.txt", "w");
    if (!f_mat) return;
    
    for (int i = 0; i < g->num_vertices; i++) {
        for (int j = 0; j < g->num_vertices; j++) {
            // Salva o peso e um espaço
            fprintf(f_mat, "%d ", g->matriz[i][j]);
        }
        // AO FINAL DE CADA LINHA: Pula para a próxima no arquivo
        fprintf(f_mat, "\n"); 
    }
    fclose(f_mat);
}

// =========================================================
// FUNÇÕES DE INICIALIZAÇÃO E APOIO
// =========================================================

GrafoLista* inicializarGrafoLista(int n) {
    GrafoLista *G = (GrafoLista*)malloc(sizeof(GrafoLista));
    G->n = n;
    G->A = (Noh**)malloc(n * sizeof(Noh*));
    for (int i = 0; i < n; i++) G->A[i] = NULL;
    return G;
}

void inicializarMatriz(GrafoMatriz *g, int n) {
    g->num_vertices = n;
    for (int i = 0; i < QTD_VERTICES; i++)
        for (int j = 0; j < QTD_VERTICES; j++)
            g->matriz[i][j] = 0;
}

void adicionarArestaLista(GrafoLista *G, int v1, int v2, int peso) {
    Noh *novo = (Noh*)malloc(sizeof(Noh));
    novo->rotulo = v2;
    novo->custo = peso;
    novo->prox = G->A[v1];
    G->A[v1] = novo;
}

// =========================================================
// [LETRA D]: CÁLCULO DO GRAU DOS VÉRTICES
// =========================================================
void exibirGraus(GrafoMatriz *g, char tipo) {
    printf("\n--- [LETRA D] CALCULO DO GRAU DE CADA VERTICE ---");
    
    for (int v = 0; v < g->num_vertices; v++) {
        int entrada = 0, saida = 0;
        
        for (int i = 0; i < g->num_vertices; i++) {
            if (g->matriz[v][i] != 0) saida++;
            if (g->matriz[i][v] != 0) entrada++;
        }

        if (tipo == 'D' || tipo == 'd') {
            printf("\nVertice %d -> Grau de Entrada: %d | Grau de Saida: %d", v, entrada, saida);
        } else {
            // Em grafos não dirigidos, entrada e saída são iguais ao grau
            printf("\nVertice %d -> Grau: %d", v, saida);
        }
    }
    printf("\n");
}

// =========================================================
// [LETRA E]: AGM PRIM
// =========================================================
void primAGM(GrafoMatriz *g, char tipo, int valorado) {
    printf("\n--- [LETRA E] ARVORE GERADORA MINIMA (PRIM) ---");
    if (tipo == 'D' || tipo == 'd' || valorado == 0) { printf("\nOperacao nao aplicavel.\n"); return; }

    int n = g->num_vertices, key[QTD_VERTICES], pi[QTD_VERTICES], Q[QTD_VERTICES], custo = 0;
    int ordem[QTD_VERTICES], c = 0; // 'c' é o contador da ordem

    for (int i = 0; i < n; i++) { key[i] = INT_MAX; pi[i] = -1; Q[i] = 1; }
    key[0] = 0;

    for (int i = 0; i < n; i++) {
        int u = -1, min = INT_MAX;
        for (int j = 0; j < n; j++) if (Q[j] && key[j] < min) { min = key[j]; u = j; }
        if (u == -1) break;
        Q[u] = 0; ordem[c++] = u; 
        if (pi[u] != -1) custo += g->matriz[pi[u]][u];
        for (int v = 0; v < n; v++)
            if (g->matriz[u][v] && Q[v] && g->matriz[u][v] < key[v]) { pi[v] = u; key[v] = g->matriz[u][v]; }
    }

    printf("\nCaminho: ");
    for (int i = 0; i < c; i++) printf("%d%s", ordem[i], (i == c-1 ? "" : " -> "));
    
    printf("\n\nArestas Selecionadas:");
    for (int i = 1; i < n; i++) if (pi[i] != -1) printf("\n  %d-%d | Peso: %d", pi[i], i, g->matriz[pi[i]][i]);
    printf("\n\nSOMA TOTAL (AGM) = %d\n", custo);
}
// =========================================================
// [LETRA F]: DIJKSTRA
// =========================================================
void imprimirCaminho(int v, int *pred, int origem) {
    if (v == -1) return;
    imprimirCaminho(pred[v], pred, origem);
    // Imprime a seta apenas se não for o vértice de origem
    printf("%s%d", (v == origem ? "" : " -> "), v);
}

void Dijkstra(GrafoLista *G, int origem) {
    int dist[QTD_VERTICES], pred[QTD_VERTICES], R[QTD_VERTICES] = {0};
    int i, v, w, tam_R = 0;
    for (i = 0; i < G->n; i++) { dist[i] = INT_MAX; pred[i] = -1; }
    dist[origem] = 0;
    while (tam_R < G->n) {
        int min_dist = INT_MAX; v = -1;
        for (i = 0; i < G->n; i++) if (!R[i] && dist[i] < min_dist) { v = i; min_dist = dist[i]; }
        if (v == -1) break;
        R[v] = 1; tam_R++;
        for (Noh *p = G->A[v]; p != NULL; p = p->prox) {
            w = p->rotulo;
            if (!R[w] && dist[v] != INT_MAX && dist[w] > dist[v] + p->custo) {
                dist[w] = dist[v] + p->custo; pred[w] = v;
            }
        }
    }
    printf("\n--- [LETRA F] RESULTADOS DIJKSTRA ---\n");
    for (i = 0; i < G->n; i++) {
        if (dist[i] == INT_MAX) printf("Vertice %d: Inalcancavel\n", i);
        else { 
            printf("Destino %d: Distancia Minima = %d | Caminho: ", i, dist[i]); 
            imprimirCaminho(i, pred, origem); 
            printf("\n"); 
        }
    }
}

// =========================================================
// [LETRA G e H]: BUSCAS (LARGURA E PROFUNDIDADE)
// =========================================================
void bfs(GrafoMatriz *g, int inicial) {
    int vis[QTD_VERTICES] = {0}, fila[QTD_VERTICES], r = 0, e = 0;
    vis[inicial] = 1; fila[e++] = inicial;
    printf("\n--- [LETRA G] BUSCA EM LARGURA (BFS) ---\nOrdem de visita: ");
    while (r < e) {
        int u = fila[r++]; printf("%d ", u);
        for (int i = 0; i < g->num_vertices; i++)
            if (g->matriz[u][i] != 0 && !vis[i]) { vis[i] = 1; fila[e++] = i; }
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
    char arqNome[100];
    int opcao;

    printf("=== TRABALHO DE GRAFOS ===\n");
    printf("1 - Criar Grafo Manualmente (Letra B)\n");
    printf("2 - Ler Grafo de Arquivo (Letra A)\n");
    printf("Escolha: ");
    scanf("%d", &opcao);

    if (opcao == 1) {
        char t; int v, n, contador = 1;
        printf("\n--- CONFIGURACAO ---\nTipo [G/D]: "); scanf(" %c", &t);
        printf("Valorado [0/1]: "); scanf("%d", &v);
        printf("Qtd Vertices: "); scanf("%d", &n);
        printf("Nome do arquivo para salvar: "); scanf("%s", arqNome);

        FILE *f_novo = fopen(arqNome, "w");
        if (!f_novo) return 1;
        fprintf(f_novo, "%c %d %d\n", t, v, n);

        printf("\n--- INSERCAO DE ARESTAS ---\nDigite: origem destino peso\n");
        printf("Para parar, digite: -1 -1\n\n");

        int v1, v2; float p;
        while (1) {
            printf("Aresta %d: ", contador);
            scanf("%d %d", &v1, &v2);
            if (v1 == -1) break;
            if (v == 1) {
                scanf("%f", &p);
                fprintf(f_novo, "%d %d %.1f\n", v1, v2, p);
            } else {
                fprintf(f_novo, "%d %d\n", v1, v2);
            }
            contador++;
        }
        fclose(f_novo);
        printf("\n[OK] Arquivo '%s' gerado com sucesso!\n", arqNome);
    } else {
        printf("Digite o nome do arquivo (ex: grafo.txt): ");
        scanf("%s", arqNome);
    }

    // Leitura e processamento do arquivo de entrada
    FILE *f = fopen(arqNome, "r");
    if (!f) { printf("Erro ao abrir arquivo!\n"); return 1; }

    char tipo; int val, n_final, v1, v2; float peso_f;
    fscanf(f, " %c %d %d", &tipo, &val, &n_final);

    GrafoLista *GL = inicializarGrafoLista(n_final);
    GrafoMatriz GM; inicializarMatriz(&GM, n_final);

    while (fscanf(f, "%d %d", &v1, &v2) != EOF) {
        int p = 1;
        if (val == 1) { fscanf(f, "%f", &peso_f); p = (int)peso_f; }
        adicionarArestaLista(GL, v1, v2, p);
        GM.matriz[v1][v2] = p;
        if (tipo == 'G' || tipo == 'g') {
            adicionarArestaLista(GL, v2, v1, p);
            GM.matriz[v2][v1] = p;
        }
    }
    fclose(f);

    // [LETRA C]: Chama a função para salvar a matriz no arquivo formatado
    salvarMatrizFormato(&GM); 

    // Exibição da Matriz no Terminal (Letra C)
    printf("\n--- [LETRA C] MATRIZ DE ADJACENCIA GERADA ---\n");
    for (int i = 0; i < n_final; i++) {
        for (int j = 0; j < n_final; j++) {
            printf("%d\t", GM.matriz[i][j]); 
        }
        printf("\n");
    }

    printf("\n[OK] Arquivo 'matriz_adjacencia.txt' gerado com sucesso.\n");

    // Solicita o vértice inicial para as buscas e cálculos
    int ini; 
    printf("\nInforme o vertice inicial (0 a %d): ", n_final-1); 
    scanf("%d", &ini);

    // Executa e mostra as letras D, E, F, G e H
    exibirGraus(&GM, tipo);
    primAGM(&GM, tipo, val);
    Dijkstra(GL, ini);
    bfs(&GM, ini);
    
    int visDFS[QTD_VERTICES] = {0};
    printf("\n--- [LETRA H] BUSCA EM PROFUNDIDADE (DFS) ---\nOrdem de visita: ");
    dfs(&GM, ini, visDFS);
    printf("\n");

    return 0;
}