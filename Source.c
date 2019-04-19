#include <stdio.h>
#include <stdlib.h>

typedef struct path{
	int ori[2];
	int dest[2];
	int capacity, flow;
	int isSaturated;
} Path;

typedef struct node{
	int id;																									/* here */
  int x_index, y_index;
  int p_weight, c_weight;
	int color, discovery;
  Path* paths;
	struct node* parent;
  char status;
} Node;

typedef struct queue{
	Node* node;
	struct queue* previous;
	struct queue* next;
} Queue;

typedef struct list{
	Path* p;
	struct list* next;
} List;

int counter;																							/* here */
int m, n, i, j;
int finish;
int lowest;
int E_cursor_x, E_cursor_y;
int S_cursor_x, S_cursor_y;
int P_Cost, C_Cost, M_Cost, T_Cost;
Node* V;
Node** matrix;
int** Sflow_matrix;
int** Tflow_matrix;
/* Queue do algoritmo BFS */
Queue* current;
Queue* head;
/* Lista de nodes visitados */
Queue* group_cursor;
Queue* group_head;

void printDebug(){
	for (i = 0; i < m; i++){
		for (j = 0; j < n; j++){
			printf("%d and %d for %d-%d\n", matrix[i][j].p_weight, matrix[i][j].c_weight, i + 1, j + 1);
			if (matrix[i][j].p_weight > matrix[i][j].c_weight)
				printf("C ");
			else
				printf("P ");
		}
		printf("\n");
	}
}

int min(int num1, int num2){
  if (num1 < num2)
    return num1;
  else
    return num2;
}

Path createPath(int ori_x, int ori_y, int dest_x, int dest_y, int cap){
	Path new;
	new.ori[0] = ori_x;
	new.ori[1] = ori_y;
	new.dest[0] = dest_x;
	new.dest[1] = dest_y;
	new.capacity = cap;
	new.flow = 0;
	if (cap == 0)
		new.isSaturated = 1;
	else
		new.isSaturated = 0;
	return new;
}

void createWithPrimary(int pri, int x, int y){
	int k;  
	Node* toCreate;
  toCreate = (Node*)malloc(sizeof(Node));
	toCreate->paths = (Path*)malloc(sizeof(Path) * 4);
	for(k = 0; k < 4; k++){
		toCreate->paths[k].ori[0] = -1;
		toCreate->paths[k].ori[1] = -1;
		toCreate->paths[k].dest[1] = -1;
		toCreate->paths[k].dest[0] = -1;
		toCreate->paths[k].isSaturated = 0;
	}
	counter++;
	toCreate->id = counter;																	/* here */
	toCreate->x_index = x;
	toCreate->y_index = y;
  toCreate->p_weight = pri;
	toCreate->color = 0;
	toCreate->discovery = 0;
	toCreate->status = 'U';
  matrix[x][y] = *toCreate;
}

void initializeEverything(){
	Path pt;
	int assist, op;
  for(i = 0; i < m; i++){
		matrix[i] = (Node*)malloc(sizeof(Node) * n);
		Sflow_matrix[i] = (int*)malloc(sizeof(int) * n);
		for(j = 0; j < n; j++){
      if ((scanf("%d", &assist)) < 0)
				return;
      createWithPrimary(assist, i, j);
			Sflow_matrix[i][j] = assist;
    }
  }
  getchar();
	for(i = 0; i < m; i++){
		Tflow_matrix[i] = (int*)malloc(sizeof(int) * n);
		for(j = 0; j < n; j++){
      if ((scanf("%d", &assist)) < 0)
				return;
      matrix[i][j].c_weight = assist;
			Tflow_matrix[i][j] = assist;
			op = min(Sflow_matrix[i][j], Tflow_matrix[i][j]);
			Sflow_matrix[i][j] = op;
			Tflow_matrix[i][j] = op;
    }
  }
  getchar();
	if (m == 1 && n == 1){
		finish = 1;
		if (matrix[0][0].p_weight <= matrix[0][0].c_weight){
			matrix[0][0].status = 'P';
			T_Cost = matrix[0][0].p_weight;
		}
		else{
			matrix[0][0].status = 'C';
			T_Cost = matrix[0][0].c_weight;
		}
		return;
	}
  for(i = 0; i < m; i++){
		for(j = 0; j < (n - 1); j++){
      if ((scanf("%d", &assist)) < 0)
				return;
			pt = createPath(i, j, i, (j + 1), assist);
			matrix[i][j].paths[0] = pt;
			matrix[i][j + 1].paths[2] = pt;
    }
  }
  getchar();
  for(i = 0; i < (m - 1); i++){
		for(j = 0; j < n; j++){
      if ((scanf("%d", &assist)) < 0)
				return;
			pt = createPath(i, j, (i + 1), j, assist);
			matrix[i][j].paths[1] = pt;
			matrix[i + 1][j].paths[3] = pt;
    }
  }
}

void printStack(Queue* h){
	printf("Stack is at: ");
	while(h != NULL){
		printf("%d ", h->node->id);
		h = h->next;
	}
	printf("\n");
}

int isEmpty(Queue* hd){
	if (hd == NULL)
		return 1;
	else
		return 0;
}

int isIn(Node* nd, Queue* hd){
	Queue* cs;
	cs = hd;
	while (cs != NULL){
		if (nd == cs->node)
			return 1;
	}
	return 0;
}

Queue* find(Queue* c, Node* n){
	Queue* assist = c;
	while(assist != NULL){
		if (assist->node == n)
			return assist;
		assist = assist->previous;
	}
	return NULL;
}

void Add(Node* nd, int mode){
	Queue* assist = (Queue*)malloc(sizeof(Queue));
	assist->node = nd;
	assist->next = NULL;
	if (mode == 0){																			/* Adiciona à lista de nodes visitados */
		assist->previous = group_cursor;
		if (group_head == NULL){
			group_head = assist;
			group_cursor = assist;
		}
		else{
			group_cursor->next = assist;
			group_cursor = assist;
		}
	}
	else{																								/* Adiciona à Queue do algoritmo BFS */
		assist->previous = current;
		if (head == NULL){
			head = assist;
			current = assist;
		}
		else{
			current->next = assist;
			current = assist;
		}
	}
}

Node* Pop(){
	Queue* tmp = head;
	head = head->next;
	return tmp->node;
}

void Cut(Queue* h){
	Queue* assist = h;
	h = h->next;																																		/*Volatile*/
	free(assist);
}

void Purge(Queue* h){
	Queue* tmp;
	while(h != NULL){
		tmp = h;
		h = h->next;
		free(tmp);
	}
}

void PurgeList(List* l){
	List* assist;
	while(l != NULL){
		assist = l;
		l = l->next;
		free(assist);
	}
}

void ApplyChange(List* h){
	int k;
	int x1, y1;
	int x2, y2;
	List* assist;
	assist = h;
	if ((matrix[E_cursor_x][E_cursor_y].p_weight - Sflow_matrix[E_cursor_x][E_cursor_y]) < lowest)
		lowest = (matrix[E_cursor_x][E_cursor_y].p_weight - Sflow_matrix[E_cursor_x][E_cursor_y]);
	if ((matrix[S_cursor_x][S_cursor_y].c_weight - Tflow_matrix[S_cursor_x][S_cursor_y]) < lowest)
		lowest = matrix[S_cursor_x][S_cursor_y].c_weight - Tflow_matrix[S_cursor_x][S_cursor_y];
	while(assist != NULL){
		x1 = assist->p->ori[0];
		y1 = assist->p->ori[1];
		x2 = assist->p->dest[0];
		y2 = assist->p->dest[1];
		for (k = 0; k < 4; k++){
			if ((matrix[x1][y1].paths[k].ori[0] == x2 && matrix[x1][y1].paths[k].ori[1] == y2) || (matrix[x1][y1].paths[k].dest[0] == x2 && matrix[x1][y1].paths[k].dest[1] == y2))
				matrix[x1][y1].paths[k].flow += lowest;
			if ((matrix[x2][y2].paths[k].ori[0] == x1 && matrix[x2][y2].paths[k].ori[1] == y1) || (matrix[x2][y2].paths[k].dest[0] == x1 && matrix[x2][y2].paths[k].dest[1] == y1))
				matrix[x2][y2].paths[k].flow += lowest;
		}
		if (assist->p->flow == assist->p->capacity){
			assist->p->isSaturated = 1;
			for (k = 0; k < 4; k++){
				if ((matrix[x1][y1].paths[k].ori[0] == x2 && matrix[x1][y1].paths[k].ori[1] == y2) || (matrix[x1][y1].paths[k].dest[0] == x2 && matrix[x1][y1].paths[k].dest[1] == y2))
					matrix[x1][y1].paths[k].isSaturated = 1;
				if ((matrix[x2][y2].paths[k].ori[0] == x1 && matrix[x2][y2].paths[k].ori[1] == y1) || (matrix[x2][y2].paths[k].dest[0] == x1 && matrix[x2][y2].paths[k].dest[1] == y1))
					matrix[x2][y2].paths[k].isSaturated = 1;
			}
		}
		assist = assist->next;
	}
	Sflow_matrix[E_cursor_x][E_cursor_y] += lowest;
	Tflow_matrix[S_cursor_x][S_cursor_y] += lowest;
}		

Queue* BFS(int changer){
	int k, l, p;

	V = NULL;

	head = NULL;
	current = head;
	for(i = 0; i < m; i++){
		for(j = 0; j < n; j++){
			matrix[i][j].color = 0;
			matrix[i][j].parent = NULL;
			if(Sflow_matrix[i][j] < matrix[i][j].p_weight)
				Add(&matrix[i][j], 1);
		}
	}
	while(!isEmpty(head)){
		V = Pop(head);
		Add(V, 0);
		if(Tflow_matrix[V->x_index][V->y_index] < matrix[V->x_index][V->y_index].c_weight){
			S_cursor_x = V->x_index;
			S_cursor_y = V->y_index;
			Purge(head);
			return group_cursor;
		}
		for(k = 0; k < 4; k++){
			if(V->x_index == V->paths[k].ori[0] && V->y_index == V->paths[k].ori[1]){
				l = V->paths[k].dest[0];
				p = V->paths[k].dest[1];
			}
			else{
				l = V->paths[k].ori[0];
				p = V->paths[k].ori[1];
			}
			if (l == -1 || p == -1)
				continue;
			if(matrix[l][p].color == 0 && !(V->paths[k].isSaturated)){
				Add(&matrix[l][p], 1);
				matrix[l][p].color = 1;
				matrix[l][p].parent = V;
			}
		}
	}
	
	if(!changer){
		while(group_head != NULL){
			Queue* ld;
			ld = group_head;
			group_head = group_head->previous;
			free(ld);
		}
		free(group_cursor);
		group_cursor = NULL;
		group_head = NULL;
		return NULL;
	}
	else
		return group_cursor;
}

List* MakePath(Queue* c){
	int k;
	int x1, y1;
	int x2, y2;
	List* h;
	List* scout;
	List* assist;
	lowest = 0;
	h = NULL;
	while (c != NULL){
		if (Sflow_matrix[c->node->x_index][c->node->y_index] < c->node->p_weight){
			E_cursor_x = c->node->x_index;
			E_cursor_y = c->node->y_index;
			break;
		}
		else{
			for (k = 0; k < 4; k++){
				x1 = c->node->paths[k].ori[0];
				y1 = c->node->paths[k].ori[1];
				x2 = c->node->paths[k].dest[0];
				y2 = c->node->paths[k].dest[1];
				if ((c->node->parent->x_index == x1 && c->node->parent->y_index == y1) || (c->node->parent->x_index == x2 && c->node->parent->y_index == y2)){
					assist = (List*)malloc(sizeof(List));
					assist->p = &(c->node->paths[k]);
					assist->next = NULL;
					if (lowest == 0 || (c->node->paths[k].capacity - c->node->paths[k].flow) < lowest){
						lowest = c->node->paths[k].capacity - c->node->paths[k].flow;
					}
					if (h == NULL){
						h = assist;
						scout = assist;
					}
					else{
						scout->next = assist;
						scout = assist;
					}
				}
			}
		}
		c = find(c, c->node->parent);
	}
	return h;
}

void Edmonds_Karp(){
	Queue* pt;
	List* p_head;
	List* scout;
	pt = BFS(0);
	while (pt != NULL){
		p_head = MakePath(pt);
		scout = p_head;
		while (scout != NULL){
			scout = scout->next;
		}
		ApplyChange(p_head);
		PurgeList(p_head);
		group_cursor = NULL;
		group_head = NULL;
		pt = BFS(0);
	}
}

int main(){
	int x1, y1;
	int x2, y2;
	Queue* cursor;
	finish = 0;
	counter = 0;																						/* here */
	P_Cost = 0;
	C_Cost = 0;
	if (scanf("%d %d", &m, &n) != 2)
    printf("Erro: Número errado de argumentos");
  matrix = (Node**)malloc(sizeof(Node*) * m);
	Sflow_matrix = (int**)malloc(sizeof(int*) * m);
	Tflow_matrix = (int**)malloc(sizeof(int*) * m);
  getchar();
  initializeEverything();
	if (finish){
		printf("%d\n\n", T_Cost);
		printf("%c\n", matrix[0][0].status);
		return 0;
	}

	Edmonds_Karp();
	cursor = BFS(1);
	while (cursor != NULL){
		cursor->node->status = 'C';
		cursor = cursor->previous;
	}
	for(i = 0; i < m; i++){
		for(j = 0; j < n; j++){
			if (matrix[i][j].status == 'U'){
				matrix[i][j].status = 'P';
				P_Cost += matrix[i][j].p_weight;
			}
			else
				C_Cost += matrix[i][j].c_weight;
		}
	}

	for(i = 0; i < m; i++){
		for(j = 0; j < n; j++){
			x1 = matrix[i][j].paths[0].ori[0];
			y1 = matrix[i][j].paths[0].ori[1];
			x2 = matrix[i][j].paths[0].dest[0];
			y2 = matrix[i][j].paths[0].dest[1];
			if (j < (n - 1) && (matrix[x1][y1].status != matrix[x2][y2].status))
				M_Cost += matrix[i][j].paths[0].capacity;
			x1 = matrix[i][j].paths[1].ori[0];
			y1 = matrix[i][j].paths[1].ori[1];
			x2 = matrix[i][j].paths[1].dest[0];
			y2 = matrix[i][j].paths[1].dest[1];
			if (i < (m - 1) && (matrix[x1][y1].status != matrix[x2][y2].status))
				M_Cost += matrix[i][j].paths[1].capacity;
		}
	}

	T_Cost = P_Cost + C_Cost + M_Cost;
	printf("%d\n\n", T_Cost);

	for(i = 0; i < m; i++){
		for(j = 0; j < n; j++){
			printf("%c ", matrix[i][j].status);
		}
		printf("\n");
	}
  return 0;
}
