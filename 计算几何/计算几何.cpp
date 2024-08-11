#include<bits/stdc++.h>
const int N = 5007, M = 50007, INF = 0x3f3f3f3f;
const double DINF = 1e18, eps = 1e-5;
struct Point{
    double x, y;
    Point(double x = 0, double y = 0):x(x), y(y){ }
};
typedef Point Vector;
struct Line{
    Vector v;
    Point p;
    Line(Vector v, Point p):v(v), p(p) {}
    Point get_point_in_line(double t){
        return v + (p - v)*t;
   }
bool parallel(Line v){
 return sgn((e?s)^(v.e?v.s)) ==0;
} 
};
Vector operator + (Vector A, Vector B){return Vector(A.x + B.x, A.y + B.y);}
Vector operator - (Point A, Point B){return Vector(A.x - B.x, A.y - B.y);}
Vector operator * (Vector A, double p){return Vector(A.x * p, A.y * p);}
Vector operator / (Vector A, double p){return Vector(A.x / p, A.y / p);}
bool operator < (const Point& a, const Point& b) {return a.x < b.x ;}
int sgn(double x){
    if(fabs(x) < eps)
        return 0;
    if(x < 0)
        return -1;
    return 1;
}
double Cross(Vector A, Vector B){return A.x *  B.y - B.x * A.y;}
double Dot(Vector A, Vector B){return A.x * B.x + A.y * B.y;}
double Length(Vector A){return sqrt(Dot(A, A));}
bool operator ==(const Point& a, const Point& b){return !sgn(a.x - b.x);} 
double Polar_angle(Vector A){return atan2(A.y, A.x);}
double Area2(Point A, Point B, Point C){return Cross(B - A, C - A);}
bool ToLeftTest(Point a, Point b, Point c){
    return Cross(b - a, c - b) > 0;
}
inline Vector Normal(Vector A) {
    double L = Length(A);
    return Vector(-A.y / L, A.x / L);
}
Vector Rotate(Vector A, double rad){
    return Vector(A.x * cos(rad) - A.y * sin(rad), A.x * sin(rad) + A.y * cos(rad));
}

int dcmp(double x){
    if (fabs(x)<eps) {
        return 0;
    }
    else return x<0?-1:1;
}
int relation(Point A, Point B, Point C)
{
    int c = sgn(Cross((B - A), (C - A)));
    if(c < 0) return 1;
    else if(c > 0) return -1;
    return 0;
}
Point Get_line_intersection(Point P,Vector v,Point Q,Vector w)
{
 Vector u = P - Q;
 double t = Cross(w, u) / Cross(v, w);
 return P + v * t;
}
double Distance_point_to_line(Point P, Point A, Point B){
    Vector v1=B-A,v2=P-A;
    return fabs(Cross(v1,v2)/Length(v1));
}
double Distance_point_to_segment(Point P, Point A, Point B)
{
    if(A==B)return Length(P-A); 
    Vector v1 = B - A, v2 = P - A, v3 = P - B;
    if(dcmp(Dot(v1, v2)) < 0) return Length(v2);
    if(dcmp(Dot(v1, v3)) > 0)return Length(v3);
    return fabs(Cross(v1, v2) / Length(v1));
}
Point Get_line_projection(Point P, Point A, Point B)
{
    Vector v = B - A;
    return A + v * (Dot(v, P - A) / Dot(v, v));
}
inline Point FootPoint(Point p, Point a, Point b){
    Vector x = p - a, y = p - b, z = b - a;
    double len1 = Dot(x, z) / Length(z), len2 = - 1.0 * Dot(y, z) / Length(z);
    return a + z * (len1 / (len1 + len2));
}
inline Point Symmetry_PL(Point p, Point a, Point b){
    return p + (FootPoint(p, a, b) - p) * 2;
}
bool OnSegment(Point p, Point a1, Point a2){
    return sgn(Cross(a1-p, a2-p)) ==0&&sgn(Dot(a1-p,a2-p))<0;
}
bool Segment_proper_intersection(Point a1, Point a2, Point b1, Point b2){
    double c1 = Cross(a2-a1, b1-a1), c2 = Cross(a2-a1, b2-a1);
    double c3 = Cross(b2-b1, a1-b1), c4 = Cross(b2-b1, a2-b1);
    if(!sgn(c1)||!sgn(c2)||!sgn(c3)||!sgn(c4)){
        bool f1 = OnSegment(b1, a1, a2);
        bool f2 = OnSegment(b2, a1, a2);
        bool f3 = OnSegment(a1, b1, b2);
        bool f4 = OnSegment(a2, b1, b2);
        bool f = (f1|f2|f3|f4);
        return f;
    } 
    return (sgn(c1)*sgn(c2)<0&&sgn(c3)*sgn(c4)<0);
}
bool on_segment(Point P, Point a1, Point a2)
{
    return dcmp(Cross(a1 - P, a2 - P))==0;
}
int main(){
    return 0;
}