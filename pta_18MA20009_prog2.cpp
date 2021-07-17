//Aviraj Singh Bevli, 18MA20009
//O(nlogn)implementation

/*
The split and merge vertices are sources
of local non-monotonicity
• A polygon is y-monotone if it has no
split or merge vertices
• Use the plane-sweep method to remove
split & merge vertice
*/

//We create a priority queue of vertices
//We create a Binary search tree for edges
//Use a stack to triangulate the monotone polygon in O(n) time
#include<iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
#define M_PI 3.141593

struct point{
	float x; float y;
};

struct edge;

struct vertex{
	point pt;
	bool is_in_polygon=1;
	bool is_reflex=0;
	bool is_ear=1;

	//Will be used to partition polygon into monotone pieces
	int type_of_vertex=0;//0 for normal, 1 onwards turn vertex.
	//1->start, 2->end, 3->split, 4->merge vertex

	edge *edg_next, *edg_prev;
	vertex *diag, *pdiag;
};

struct edge{
	vertex *v_from;
	vertex *v_to;
	vertex *helper;//to help in monotone triangulization
};


//Crreate a Priority queue of vertices
struct priority_queue{
	vertex** priority_queue_elems;
	int size;
	int n;
};

//Compare two vertices
int find_greater(vertex* v1, vertex* v2){
  return (v1->pt.y > v2->pt.y) || ((v1->pt.y == v2->pt.y) && (v1->pt.x < v2->pt.x));
}

//Is the priority queue(Heap) a leaf node....
//All nodes with index greater than n/2 are leaf nodes in a heap
int is_node_leaf(int index, priority_queue* pq){
  return (index >= pq->n/2) && (index < pq->n);
}

int find_left_child(int index, priority_queue* pq){
  return 2*index + 1;
}

int find_right_child(int index, priority_queue* pq){
  return 2*index + 2;
}

int find_parent(int index, priority_queue* pq){
  return (index-1)/2;
}

void swap(int i, int j, priority_queue* pq){
	vertex* temp;
	temp = pq->priority_queue_elems[i];
	pq->priority_queue_elems[i] = pq->priority_queue_elems[j];
	pq->priority_queue_elems[j] = temp;
}

//This is the implementation of the sift_down functino
void down_shift(int index, priority_queue* pq){
	int j;
	while (!is_node_leaf(index, pq)) {
		j = find_left_child(index, pq);
		if ( (j < (pq->n-1)) && (find_greater(pq->priority_queue_elems[j+1], pq->priority_queue_elems[j])) ) 
			j++; 

		if ( !find_greater(pq->priority_queue_elems[j], pq->priority_queue_elems[index]) )
			return;

		swap(index, j, pq);
		index = j; 
	}
}

//Creates the priority queue(As a heap implementation) of vertices
priority_queue* priority_queue_create(int size){
	priority_queue* to_return;
	to_return = (priority_queue*)malloc(sizeof(priority_queue));
	to_return->size=size;
	//to_return->priority_queue_elems = malloc(sizeof(void*)*size);
	to_return->n = 0;
	return to_return;
}

//Insert into priority queue
void priority_queue_insert(vertex* v, priority_queue* pq){
	int current = pq->n++;
	pq->priority_queue_elems[current] = v; //Start from the end of the heap
	while ((current!=0) && find_greater(pq->priority_queue_elems[current], pq->priority_queue_elems[find_parent(current,pq)]) ){
		swap(current, find_parent(current, pq), pq);
		current = find_parent(current, pq);
	}
}

//Remove from priority queue
vertex* priority_queue_remove(priority_queue* pq){
	if( pq->n == 0 ) return NULL;
	pq->n--;
	swap(0, pq->n, pq);
	if (pq->n != 0) down_shift(0, pq);     
	return pq->priority_queue_elems[pq->n];
}


//Create a Binary search tree for edges
struct bstnode{
	edge *edg;
	bstnode *left, *right;
};

bstnode* bst_create(edge *edg, vertex *helper_vert){
    bstnode *result = (bstnode*)malloc(sizeof(bstnode));
    
    result->left = result->right = NULL;
    result->edg = edg;
    edg->helper = helper_vert;
    return result;
}

void bst_free(bstnode *bst){
  if( bst == NULL ) return;

  bst_free(bst->left);
  bst_free(bst->right);
  free(bst);
}

double calc_key(edge *edg, double y){
	double k = (edg->v_to->pt.y - edg->v_from->pt.y)/(edg->v_to->pt.x - edg->v_from->pt.x);
	return edg->v_to->pt.x == edg->v_from->pt.x ? edg->v_from->pt.x : (y - edg->v_from->pt.y + k*edg->v_from->pt.x)/k;
}

//Insert into the BST
void bst_insert(edge *edg, vertex *helper_vert, bstnode **head_pointer_tree, double y){
	double new_key, old_key;
	if (*head_pointer_tree == NULL){
		*head_pointer_tree = bst_create(edg, helper_vert);
		return;
	}

	old_key = calc_key((*head_pointer_tree)->edg, y);
	new_key = calc_key(edg, y);
	if (old_key > new_key)
		bst_insert(edg, helper_vert, &((*head_pointer_tree)->left), y);
	else
		bst_insert(edg, helper_vert, &((*head_pointer_tree)->right), y);
}

//recursively go to the left node to get the minimum from BST
edge* getmin(bstnode *tree) {
	if (tree->left == NULL)
		return tree->edg;
	else 
		return getmin(tree->left);
}

//recursively go to the left node to reach the minimum and then delete it from BST
void delete_min(bstnode **head_pointer_tree){
	bstnode *tn;
	if ((*head_pointer_tree)->left == NULL){
		tn = (*head_pointer_tree)->right;
		free(*head_pointer_tree);
		*head_pointer_tree = tn;
	}
	else delete_min(&(*head_pointer_tree)->left);
}

void bst_remove(edge* edg, bstnode **head_pointer_tree, double y){
	double current_key, delete_key;
	edge *temp;
	bstnode *tnode;

	if (*head_pointer_tree == NULL) return;
	current_key = calc_key((*head_pointer_tree)->edg, y); 
	delete_key = calc_key(edg, y); 
	if (delete_key < current_key)
		bst_remove(edg, &((*head_pointer_tree)->left), y);
	else if (delete_key > current_key)
		bst_remove(edg, &((*head_pointer_tree)->right), y);
	else 
	{
		if ((*head_pointer_tree)->left == NULL){
			tnode = (*head_pointer_tree)->right;
			free(*head_pointer_tree);
			*head_pointer_tree = tnode;
		}
		else if ((*head_pointer_tree)->right == NULL){
			tnode = ((*head_pointer_tree)->left);
			free(*head_pointer_tree);
			*head_pointer_tree = tnode;
		}
		else{
			temp = getmin((*head_pointer_tree)->right);
			(*head_pointer_tree)->edg = temp;
			delete_min(&((*head_pointer_tree)->right));
		}
	}
}

//Maintain a stack of vertices to implement monotone triangulation
struct stack{
	vertex** array;
	int top;
};

stack* initialise_stack(int size){
  stack* result;
  
  result = (stack*)malloc(sizeof(stack));
  //result->array = malloc(sizeof(stack*)*size);
  result->top = 0;

  return result;
}

void free_stack(stack* stack_in){
  free(stack_in->array);
  free(stack_in);
}

vertex* pop(stack* stack_in) {
  if( stack_in->top == 0 ) 
    return NULL;
  else
    return stack_in->array[--(stack_in->top)];
}

vertex* peek(stack* stack_in) {
  if( stack_in->top == 0 ) 
    return NULL;
  else
    return stack_in->array[(stack_in->top)-1];
}

void push(vertex *v, stack *stack_in){
  stack_in->array[(stack_in->top)++] = v;
}


float absol(float x){
	if(x<0) return -1*x;
	return x;
}

class triangle{
private:
	point a; point b; point c;
public:
	triangle(){}
	triangle(point a, point b, point c): a(a), b(b), c(c){}
	
	float area(point p1, point p2, point p3){
		return absol((p1.x*(p2.y-p3.y) + p2.x*(p3.y-p1.y)+ p3.x*(p1.y-p2.y))/2.0);
	}

	bool is_point_interior(point pt){
		//cout<<"is_point_interior("<<pt.x<<","<<pt.y<<") wrto triangle ("<<a.x<<","<<a.y<<"), "<<b.x<<","<<b.y<<"), "<<c.x<<","<<c.y<<")"<<endl;
		float A = area(a,b,c);   //cout<<"A:"<<A<<", ";
		float A1 = area(pt,b,c); //cout<<"A1:"<<A1<<", "; //Calculate area of triangle PBC 
		float A2 = area(pt,a,c); //cout<<"A2:"<<A2<<", "; //Calculate area of triangle PAC   
		float A3 = area(pt,a,b); //cout<<"A3:"<<A3<<endl; //Calculate area of triangle PAB 
		return (A == A1 + A2 + A3); //Check if sum of A1, A2 and A3 is same as A
	}

	void show_triangle(){
		cout<<"{ "<<" ("<<a.x<<","<<a.y<<"), "<<" ("<<b.x<<","<<b.y<<"), "<<" ("<<c.x<<","<<c.y<<") }";
	}

	friend class polygon;
};

class polygon{
private:
	int n;
	vertex* vertex_list;
	triangle* triangle_list;
	vertex* monotone_y_list;

public:
	polygon(int num_vertices){
		n = num_vertices;
		vertex_list = new vertex[n];
		triangle_list = new triangle[n-2];//every n-gon can be partitioned into n-2 triangles
	}

	void set_vertices(int x, point a, point b, point c);
	void triangulate();
	void triangulate_y_monotone();
	void print_polygon();
	void get_polygon();
	void print_triangles();

	bool is_y_monotone();
};

void polygon::set_vertices(int triangle_index, point a, point b, point c){
	cout<<"Setting the "<<triangle_index<<"th triangle with points ("<<a.x<<","<<a.y<<"), ("<<b.x<<","<<b.y<<"), ("<<c.x<<","<<c.y<<")"<<endl;
	(triangle_list[triangle_index]).a=a;
	(triangle_list[triangle_index]).b=b;
	(triangle_list[triangle_index]).c=c;
}

bool polygon::is_y_monotone(){
	//If the polygon has only one start and end vertex, then it is y monotone
	int num_starts=0;
	int num_ends=0;
	for(int i=0;i<n;i++){
		if(vertex_list[i].type_of_vertex ==1)
			num_starts++;
		else if(vertex_list[i].type_of_vertex ==2)
			num_ends++;
	}
	if(num_starts==1 && num_ends==1)
		return 1;
	return 0;
}


//Now that the polygon is a montone polygon, triangulate it in O(n) time
void polygon::triangulate_y_monotone(){
	int start_index, end_index;
	for(int i=0;i<n;i++){
		if(vertex_list[i].type_of_vertex ==1)
			start_index=i;
		else if(vertex_list[i].type_of_vertex ==2)
			end_index=i;
	}

	//start from the start index
	int top_index = start_index;
	int left_index = (start_index-1+n)%n;
	int right_index = (start_index+1)%n;

}

//If already a y monotone polygon, then use triangulate_y_monotone
//Else, convert the polygon into y monotone polygon and then triangulate it
void polygon::triangulate(){
	if(is_y_monotone()){
		triangulate_y_monotone();
		return;
	}

	//make_y_monotone();//Convert into Y monotone polygon

	//Now that the polygon is Y monotone, triangulate it greedily
	triangulate_y_monotone();
	return;
}

void polygon::get_polygon(){
	cout<<"Enter the vertices of the polygon(in the CW order): (x,y)1, (x,y)2, ....."<<endl;
	for(int i=0;i<n;i++){
		point temp; cin>>temp.x>>temp.y;
		vertex_list[i].pt = temp;
	}

	//Mark the reflex points, ear points in the polygon
	point curr = vertex_list[0].pt;
	point next = vertex_list[1].pt;
	point prev = vertex_list[n-1].pt;

	for(int i=0;i<n;i++){
		//Using determinate values to determine if the point is a reflex point or not
		if((curr.x-prev.x)*(next.y-curr.y) > (next.x-curr.x)*(curr.y-prev.y)){
			vertex_list[i].is_reflex = 1;
			vertex_list[i].is_ear = 0;

			//Check if split or merge vertex
			if((curr.y > prev.y)&&(curr.y > next.y))
				vertex_list[i].type_of_vertex = 3;
			else if((curr.y < prev.y)&&(curr.y < next.y))
				vertex_list[i].type_of_vertex = 4;
		}
		else{
			//Check if start or end vertex
			if((curr.y > prev.y)&&(curr.y > next.y))
				vertex_list[i].type_of_vertex = 1;
			else if((curr.y < prev.y)&&(curr.y < next.y))
				vertex_list[i].type_of_vertex = 2;

			//Check whether ear or not
			triangle tr(prev,curr,next);
			for(int j=0;j<n;j++){
				if((j==(i-1+n)%n) || (j==i) || (j==(i+1)%n)) continue;
				if(tr.is_point_interior(vertex_list[j].pt)){
					vertex_list[i].is_ear = 0;
					break;
				}
			}
		}
		prev = curr;
		curr = next;
		next = vertex_list[(i+2)%n].pt;
	}
}

void polygon::print_polygon(){
	cout<<endl<<"The polygon(in the CW order) is:"<<endl;
	for(int i=0;i<n;i++)
		cout<<"("<<vertex_list[i].pt.x<<","<<vertex_list[i].pt.y<<"), ";
	cout<<endl;
}

void polygon::print_triangles(){
	cout<<endl<<"The triangles are:"<<endl;
	for(int i=0;i<n-2;i++){
		triangle_list[i].show_triangle();
		cout<<endl;
	}
	cout<<endl;
}

int main(){
	cout<<"Enter the number of vertices of the polygon: ";
	int n; cin>>n;
	polygon P(n);
	P.get_polygon();
	P.print_polygon();
	P.triangulate();
	P.print_triangles();
	return 0;
}
