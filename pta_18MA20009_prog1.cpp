//Aviraj Singh Bevli, 18MA20009
//O(n^2)implementation

/*
BACKGORUND ->
There is a thoerem which says any n-gon can be partitioned into n-2 triangles
Hence, given any general n-gon, this code triangulates the polygon in complexity O(n square)
*/
#include <bits/stdc++.h> 
using namespace std;
#define M_PI 3.141593

struct point{
	float x; float y;
};

struct vertex{
	point pt;
	bool is_in_polygon=1;
	bool is_reflex=0;
	bool is_ear=1;
};

float abs(float x){
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
		return abs((p1.x*(p2.y-p3.y) + p2.x*(p3.y-p1.y)+ p3.x*(p1.y-p2.y))/2.0);
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

public:
	polygon(int num_vertices){
		n = num_vertices;
		vertex_list = new vertex[n];
		triangle_list = new triangle[n-2];//every n-gon can be partitioned into n-2 triangles
	}

	void set_vertices(int x, point a, point b, point c);
	void triangulate();//triangulate the polygon
	void print_polygon();//print polygon to console
	void get_polygon();//get input from user
	void print_triangles();//print the triangulated triangles of the polygon
};

void polygon::set_vertices(int triangle_index, point a, point b, point c){
	cout<<"Setting the "<<triangle_index<<"th triangle with points ("<<a.x<<","<<a.y<<"), ("<<b.x<<","<<b.y<<"), ("<<c.x<<","<<c.y<<")"<<endl;
	(triangle_list[triangle_index]).a=a;
	(triangle_list[triangle_index]).b=b;
	(triangle_list[triangle_index]).c=c;
}

void polygon::triangulate(){
	//make use of the ear and reflex list
	int triangle_list_counter=0;
	int i=-1;
	int it=-1;
	while(1){
		i=(i+1)%n;
		cout<<endl<<"it:"<<++it<<", i:"<<i<<" -> ";
		int prev_index = (i-1+n)%n;//the previous point which is a part of the polynomial still and hence can form the ear triangle
		int next_index = (i+1)%n;

		if(it>200){
			cout<<"Infinte loop encountered..some problem!! Breaking!"<<endl;
			break;
		}
		if(triangle_list_counter>=n-2){
			cout<<" Already n-2 triangles, Triangulation done....breaking!"<<endl;
			break;
		}
		if(vertex_list[i].is_in_polygon == 0){
			cout<<" ("<<vertex_list[i].pt.x<<","<<vertex_list[i].pt.y<<") is is no longer in polygon"<<endl;
			continue;
		}
		
		if(vertex_list[i].is_ear == 0)
			cout<<" ("<<vertex_list[i].pt.x<<","<<vertex_list[i].pt.y<<") is "<<"not ear"<<endl;

		else if(vertex_list[i].is_ear == 1){
			cout<<" Removing  ("<<vertex_list[i].pt.x<<","<<vertex_list[i].pt.y<<") from polygon "<<endl;
			vertex_list[i].is_in_polygon=0;//Remove the ear triangle to get the new ploygon
			while(vertex_list[(prev_index)%n].is_in_polygon==0){prev_index = (prev_index-1+n)%n;}
			while(vertex_list[(next_index)%n].is_in_polygon==0){next_index = (next_index+1)%n;}
			set_vertices(triangle_list_counter++, vertex_list[prev_index].pt, vertex_list[i].pt, vertex_list[next_index].pt);
			

			//Check for the previous neighbour if after removing, it has become convex, check if it has become an ear also
			int temp_curr = prev_index;
			int temp_prev = (prev_index-1+n)%n;
			int temp_next = (prev_index+1)%n;
			//find next and prev points to determine if the previous neighbour(point under consideration) is now a convex vertex
			while(vertex_list[(temp_prev)%n].is_in_polygon==0){temp_prev = (temp_prev-1+n)%n;}
			while(vertex_list[(temp_next)%n].is_in_polygon==0){temp_next = (temp_next+1)%n;}

			point curr = vertex_list[temp_curr].pt;
			point prev = vertex_list[temp_prev].pt;
			point next = vertex_list[temp_next].pt;
			
			if((curr.x-prev.x)*(next.y-curr.y) > (next.x-curr.x)*(curr.y-prev.y)){
				vertex_list[temp_curr].is_reflex = 1;
				vertex_list[temp_curr].is_ear = 0;
			}
			else{
				vertex_list[temp_curr].is_reflex = 0;//if it is not reflex after removal of triangle
			}
			if(vertex_list[(temp_curr)%n].is_reflex==0){
				//Now that we have the triangle centred at temp_curr, find whether this is an ear triangle
				triangle temp_tri(prev, curr, next);
				bool is_ear=1;
				for(int j=0;j<n;j++){
					if(vertex_list[j].is_in_polygon == 0)
						continue;
					if(j==temp_prev || j==temp_curr || j==temp_next)
						continue;
					if(temp_tri.is_point_interior(vertex_list[j].pt)){
						is_ear=0;
						break;
					}
				}	

				if(is_ear)
					vertex_list[temp_curr].is_ear=1; 
				else
					vertex_list[temp_curr].is_ear=0;
			}


			//Check for the next neighbour if after removing, it has become convex, check if it has become an ear also
			temp_curr = next_index;
			temp_prev = (next_index-1+n)%n;
			temp_next = (next_index+1)%n;
			//find next and prev points to determine if the previous neighbour(point under consideration) is now a convex vertex
			while(vertex_list[(temp_prev)%n].is_in_polygon==0){temp_prev = (temp_prev-1+n)%n;}
			while(vertex_list[(temp_next)%n].is_in_polygon==0){temp_next = (temp_next+1)%n;}

			curr = vertex_list[temp_curr].pt;
			prev = vertex_list[temp_prev].pt;
			next = vertex_list[temp_next].pt;
			
			if((curr.x-prev.x)*(next.y-curr.y) > (next.x-curr.x)*(curr.y-prev.y)){
				vertex_list[temp_curr].is_reflex = 1;
				vertex_list[temp_curr].is_ear = 0;
			}
			else{
				vertex_list[temp_curr].is_reflex = 0;//if it is not reflex after removal of triangle
			}
			
			if(vertex_list[(temp_curr)%n].is_reflex==0){
				//Now that we have the triangle centred at temp_curr, find whether this is an ear triangle
				triangle temp_tri(prev, curr, next);
				bool is_ear=1;
				for(int j=0;j<n;j++){
					if(vertex_list[j].is_in_polygon == 0)
						continue;
					if(j==temp_prev || j==temp_curr || j==temp_next)
						continue;
					if(temp_tri.is_point_interior(vertex_list[j].pt)){
						is_ear=0;
						break;
					}
				}	

				if(is_ear)
					vertex_list[temp_curr].is_ear=1; 
				else
					vertex_list[temp_curr].is_ear=0; 
			}
		}
	}
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
		}
		else{
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

//SAMPLE INPUTS ->
/*
0 0
0 3
1 1
3 6
4 1
4 -1
3 2
2 0

{  (0,0),  (0,3),  (1,1) }
{  (3,6),  (4,1),  (4,-1) }
{  (3,6),  (4,-1),  (3,2) }
{  (3,6),  (3,2),  (2,0) }
{  (2,0),  (0,0),  (1,1) }
{  (2,0),  (1,1),  (3,6) }
*/

/*
0 0
-3 4
-0.5 2
1 2
1 3
0 3
0.5 5
2 3
4 4

The triangles are:
{  (0,0),  (-3,4),  (-0.5,2) }
{  (0,0),  (-0.5,2),  (1,2) }
{  (1,3),  (0,3),  (0.5,5) }
{  (1,3),  (0.5,5),  (2,3) }
{  (2,3),  (4,4),  (0,0) }
{  (2,3),  (0,0),  (1,2) }
{  (2,3),  (1,2),  (1,3) }
*/