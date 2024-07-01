#include<bits/stdc++.h>
#include "bitmap_image.hpp"
#include <GL/glut.h>
using namespace std;

#define pi (2*acos(0.0))

struct point
{
    double x,y,z;

    point()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }

    point(double a, double b, double c)
    {
        x = a;
        y = b;
        z = c;
    }

    point operator+(const point& p) const
    {
        return point(x+p.x, y+p.y, z+p.z);
    }

    point operator-(const point& p) const
    {
        return point(x-p.x, y-p.y, z-p.z);
    }

    point operator*(const double& d) const
    {
        return point(x*d, y*d, z*d);
    }

    point operator/(const double& d) const
    {
        return point(x/d, y/d, z/d);
    }
    
    // cross product
    point operator*(const point& p) const
    {
        return point(y*p.z-z*p.y, z*p.x-x*p.z, x*p.y-y*p.x);
    }

    // dot product
    double dot(const point& p) const
    {
        return x*p.x + y*p.y + z*p.z;
    }

    double length() const
    {
        return sqrt(x*x + y*y + z*z);
    }

    point normalize() const
    {
        double l = length();
        return point(x/l, y/l, z/l);
    }

    void print()
    {
        cout << x << " " << y << " " << z << endl;
    }

    friend ostream& operator<<(ostream& os, const point& p)
    {
        os << p.x << " " << p.y << " " << p.z;
        return os;
    }

    friend istream& operator>>(istream& is, point& p)
    {
        is >> p.x >> p.y >> p.z;
        return is;
    }

    void draw()
    {
        glVertex3f(x,y,z);
    }
};

struct Color {
    double r, g, b;
    Color() {
        r = 0.0;
        g = 0.0;
        b = 0.0;
    }
    Color(double a, double c, double d) {
        r = a;
        g = c;
        b = d;
    }
    Color operator+(const Color& p) const
    {
        return Color(r+p.r, g+p.g, b+p.b);
    }
    Color operator-(const Color& p) const
    {
        return Color(r-p.r, g-p.g, b-p.b);
    }
    Color operator*(const double& d) const
    {
        return Color(r*d, g*d, b*d);
    }
    Color operator/(const double& d) const
    {
        return Color(r/d, g/d, b/d);
    }
    Color operator*(const Color& p) const
    {
        return Color(r*p.r, g*p.g, b*p.b);
    }
    void fixRange()
    {
        this->r = std::min(this->r, 1.0);
        this->r = std::max(this->r, 0.0);
        this->g = std::min(this->g, 1.0);
        this->g = std::max(this->g, 0.0);
        this->b = std::min(this->b, 1.0);
        this->b = std::max(this->b, 0.0);
    }
    friend ostream& operator<<(ostream& os, const Color& p)
    {
        os << p.r << " " << p.g << " " << p.b;
        return os;
    }
};

struct Ray {
    point start;
    point dir;
    Ray() {
        start = point(0,0,0);
        dir = point(0,0,0);
    }
    Ray(point a, point b) {
        start = a;
        dir.normalize();
        dir = b;
    }
};

struct PointLight {
    point pos;
    double fallOff;
    PointLight() {
        pos = point(0,0,0);
        fallOff = 0.0;
    }
    PointLight(point a, double f) {
        pos = a;
        fallOff = f;
    }
    // draw as a sphere
    void draw()
    {
        glPushMatrix();
        {
            glTranslatef(pos.x, pos.y, pos.z);
            glColor3f(1.0, 1.0, 1.0);
            glutSolidSphere(5.0, 20, 20);
        }
        glPopMatrix();
    }

    friend istream& operator>>(istream& is, PointLight& p)
    {
        is >> p.pos.x >> p.pos.y >> p.pos.z >> p.fallOff;
        return is;
    }

    friend ostream& operator<<(ostream& os, const PointLight& p)
    {
        os << "Pointlight:" << endl;
        os << p.pos << " " << p.fallOff;
        return os;
    }
};

struct SpotLight{
    PointLight pl;
    point dir;
    double angle;

    SpotLight() {
        pl = PointLight();
        dir = point(0,0,0);
        angle = 0.0;
    }

    SpotLight(PointLight p, point a, point b, double c) {
        pl = p;
        dir = b;
        angle = c;
    }

    SpotLight(point a, double f, point b, point c, double d) {
        pl = PointLight(a, f);
        dir = b;
        angle = d;
    }

    // draw as a cone
    void draw()
    {
        glPushMatrix();
        {
            glTranslatef(pl.pos.x, pl.pos.y, pl.pos.z);
            glColor3f(1.0, 1.0, 1.0);
            glutSolidCone(5.0, 10.0, 20, 20);
        }glPopMatrix();
    }

    friend istream& operator>>(istream& is, SpotLight& p)
    {
        is >> p.pl.pos.x >> p.pl.pos.y >> p.pl.pos.z >> p.pl.fallOff;
        is >> p.dir.x >> p.dir.y >> p.dir.z >> p.angle;
        p.dir = p.dir - p.pl.pos;
        p.dir = p.dir.normalize();
        return is;
    }

    friend ostream& operator<<(ostream& os, const SpotLight& p)
    {
        os << "Spotlight:" << endl;
        os << p.pl << " " << p.dir << " " << p.angle;
        return os;
    }
};

class Shape;
vector<SpotLight*> spotLights;
vector<PointLight*> pointLights;
vector<Shape*> shapes;
int recursionLevel;

class Shape {
public:
    point refPoint;
    Color color;
    int shine;
    double ambCoeff, difCoeff, specCoeff, reflCoeff;

    void setColor(Color c) {
        color = c;
    }

    void setShine(int s) {
        shine = s;
    }

    void setCoeff(double amb, double dif, double spec, double refl) {
        ambCoeff = amb;
        difCoeff = dif;
        specCoeff = spec;
        reflCoeff = refl;
    }

    virtual Color getColor(point p) = 0;
    virtual void draw() = 0;
    virtual double getT(Ray r) = 0;
    virtual Ray getNormal(point p) = 0;
    double intersect(Ray r, Color &color, int level) {
        double t = getT(r);
        if(t < 0) return -1;
        point intersectionPoint = r.start + r.dir*t;
        color = getColor(intersectionPoint);
        
        Ray normal = getNormal(intersectionPoint);
        Color baseColor = getColor(intersectionPoint);
        //c = c + color;
        //c.fixRange();
        double minT = 1000000000.0;
        double lambert = 0,phong = 0;
        for(int i=0; i<pointLights.size(); i++) {
            point dir = pointLights[i]->pos - intersectionPoint;
            double dist = dir.length();
            dir = dir.normalize();
            Ray shadowRay = Ray(intersectionPoint + dir*0.0001, dir);
            bool flag = false;
            for(int j=0; j<shapes.size(); j++) {
                double temp = shapes[j]->getT(shadowRay);
                if(temp > 0 && temp < dist) {
                    flag = true;
                    break;
                }
            }
            if(!flag) {
               
                point toSource = pointLights[i]->pos - intersectionPoint;
                toSource = toSource.normalize();
                point normal = getNormal(intersectionPoint).dir;
                
                if( normal.dot(shadowRay.dir) < 0) normal = normal*(-1);
                normal = normal.normalize();

                double distance = toSource.length();
                double scalingFactor = exp(-1.0*distance*distance*pointLights[i]->fallOff);
                
                lambert += scalingFactor * max(toSource.dot(normal),0.0);
                Ray reflectedRay = Ray(intersectionPoint, r.dir - normal*2*(r.dir.dot(normal)));
                reflectedRay.dir = reflectedRay.dir.normalize();
                reflectedRay.start = reflectedRay.start + reflectedRay.dir*0.0001;

               phong += scalingFactor * pow( max(reflectedRay.dir.dot(toSource),0.0) , shine);


            }
        }
        for(int i=0; i<spotLights.size(); i++) {
            point dir = spotLights[i]->pl.pos - intersectionPoint;
            double dist = dir.length();
            dir = dir.normalize();
            Ray shadowRay = Ray(intersectionPoint, dir);
            shadowRay.dir = shadowRay.dir.normalize();
            shadowRay.start = shadowRay.start + shadowRay.dir*0.0001;
            bool flag = false;
            for(int j=0; j<shapes.size(); j++) {
                double temp = shapes[j]->getT(shadowRay);
                if(temp > 0 && temp < dist) {
                    flag = true;
                    break;
                }
            }

            // check angle with spot light
            point toSource = intersectionPoint - spotLights[i]->pl.pos;
            toSource = toSource.normalize();
            point normal = getNormal(r.start + r.dir*t).dir;
            normal = normal.normalize();
            double angle = acos(toSource.dot(spotLights[i]->dir))*180.0/pi;
            if(angle > spotLights[i]->angle) flag = true;


            if(!flag) {
               
                point toSource = pointLights[i]->pos - intersectionPoint;
                toSource = toSource.normalize();
                point normal = getNormal(intersectionPoint).dir;
                if( normal.dot(shadowRay.dir) < 0) normal = normal*(-1);
                normal = normal.normalize();

                double distance = toSource.length();
                double scalingFactor = exp(-1.0*distance*distance*pointLights[i]->fallOff);
                
                lambert += scalingFactor * max(toSource.dot(normal),0.0);

                Ray reflectedRay = Ray(intersectionPoint, r.dir - normal*( r.dir.dot(normal)*2 ));
                reflectedRay.dir = reflectedRay.dir.normalize();
                reflectedRay.start = reflectedRay.start + reflectedRay.dir*0.0001;

                phong += scalingFactor * pow( max(reflectedRay.dir.dot(toSource),0.0) , shine);

            }
        }

        //color = baseColor*(ambCoeff + difCoeff*lambert + specCoeff*phong);
        color = baseColor*(ambCoeff + difCoeff*lambert + specCoeff*phong );
        
        if(level == 0) return t;
        Ray reflectedRay = Ray(intersectionPoint, r.dir - normal.dir*2*(r.dir.dot(normal.dir)));
        reflectedRay.dir = reflectedRay.dir.normalize();
        reflectedRay.start = reflectedRay.start + reflectedRay.dir*0.0001;
        Color reflectedColor;
        double minT2 = 1000000000.0;
        for(int i=0; i<shapes.size(); i++) {
            Color tempColor;
            double temp = shapes[i]->intersect(reflectedRay, tempColor, level-1);
            if(temp > 0 && temp < minT2) {
                minT2 = temp;
                reflectedColor = tempColor;
            }
        }
        if(minT2 < 1000000000.0) {
            color = color + reflectedColor * reflCoeff;
            
        }
        color.fixRange();
        return t;
    }
    
};

class Sphere : public Shape {
public:
    double radius;

    void draw() {
       // cout << "Drawing sphere" << endl;
       
        glPushMatrix();
        {
            glTranslatef(refPoint.x, refPoint.y, refPoint.z);
            glColor3f(color.r, color.g, color.b);
            glutSolidSphere(radius, 20, 20);
        } glPopMatrix();
    }

    Color getColor(point p) {
        return color;
    }

    double getT(Ray r) {
        r.start = r.start - refPoint; // adjust ray origin
            
        double a = r.dir.dot(r.dir);
        double b = 2 * (r.dir.dot(r.start));
        double c = (r.start.dot(r.start)) - (radius*radius);

        double discriminant = pow(b, 2) - 4 * a * c;
        double t = -1;
        if (discriminant < 0){
            t = -1;
        }
        else{
            
            if(fabs(a) < 1e-5)
            {
                t = -c/b;
                return t;
            }

            double t1 = (-b - sqrt(discriminant)) / (2 * a);
            double t2 = (-b + sqrt(discriminant)) / (2 * a);

            if(t2<t1) swap(t1, t2);

            if (t1 > 0){
                t = t1;
            }
            else if (t2 > 0){
                t = t2;
            }
            else{
                t = -1;
            }
        }

        return t;
    }

    Ray getNormal(point p) {
        point n = p - refPoint;
        n = n.normalize();
        Ray r = Ray(p, n);
        return r;
    }

    friend istream& operator>>(istream& is, Sphere& p)
    {
        is >> p.refPoint.x >> p.refPoint.y >> p.refPoint.z >> p.radius;
        is >> p.color.r >> p.color.g >> p.color.b;
        is >> p.ambCoeff >> p.difCoeff >> p.specCoeff >> p.reflCoeff;
        is >> p.shine;
        return is;
    }

    friend ostream& operator<<(ostream& os, const Sphere& p)
    {
        os << "Sphere:" << endl;
        os << p.refPoint << " " << p.radius << " " << p.color << " " << p.ambCoeff << " " << p.difCoeff << " " << p.specCoeff << " " << p.reflCoeff << " " << p.shine;
        return os;
    }
    
};

class Triangle : public Shape {
public:
    point a, b;
    Triangle()
    {

    }
    Triangle(point p1, point p2, point p3, Color cl, int s, double a, double d, double sp, double r) {
        refPoint = p1;
        this->a = p2;
        this->b = p3;
        color = cl;
        shine = s;
        ambCoeff = a;
        difCoeff = d;
        specCoeff = sp;
        reflCoeff = r;
    }

    void draw() {
        glBegin(GL_TRIANGLES);
        {
            glColor3f(color.r, color.g, color.b);
            refPoint.draw();
            a.draw();
            b.draw();
        }
        glEnd();
    }

    Color getColor(point p) {
        return color;
    }

    double getArea(point m, point n, point l)
    {
        point normal = (m-n)*(l-n);
        return normal.length()/2.0;
    }

    double getT(Ray r) {
        point normal = (b-a)*(refPoint-a);
        normal = normal.normalize();
        double t = (a-r.start).dot(normal) / (r.dir.dot(normal));
        if(t < 0) return -1;
        point p = r.start + r.dir*t;
        double alpha = getArea(p, b, refPoint) / getArea(a, b, refPoint);
        double beta = getArea(p, refPoint, a) / getArea(a, b, refPoint);
        double gamma = getArea(p, a, b) / getArea(a, b, refPoint);
        if(alpha + beta + gamma > 1.0001) return -1;
        return t;
    }

    Ray getNormal(point p) {
        point normal = (b-a)*(refPoint-a);
        normal = normal.normalize();
        Ray r = Ray(p, normal);
        return r;
    }

    friend istream& operator>>(istream& is, Triangle& p)
    {
        is >> p.refPoint.x >> p.refPoint.y >> p.refPoint.z;
        is >> p.a.x >> p.a.y >> p.a.z;
        is >> p.b.x >> p.b.y >> p.b.z;
        is >> p.color.r >> p.color.g >> p.color.b;
        is >> p.ambCoeff >> p.difCoeff >> p.specCoeff >> p.reflCoeff;
        is >> p.shine;
        return is;
    }

    friend ostream& operator<<(ostream& os, const Triangle& p)
    {
        os << p.refPoint << " " << p.a << " " << p.b << " " << p.shine << " " << p.ambCoeff << " " << p.difCoeff << " " << p.specCoeff << " " << p.reflCoeff;
        return os;
    }
};

class Floor : public Shape {
public:
    double tileWidth;

    void draw() {
        refPoint = point(0.0, 0.0, 0.0);
        for(int i = -50; i < 50; i++)
        {
            for(int j = -50; j < 50; j++)
            {
                // upper diagonal
                glBegin(GL_QUADS);
                {
                    if ((i + j) % 2 == 0)
                        glColor3f(0.0f, 0.0f, 0.0f);  // Black
                    else
                        glColor3f(1.0f, 1.0f, 1.0f);  // White
                    // Define the vertices of the current square
                    glVertex3f(i * tileWidth, j * tileWidth, 0);
                    glVertex3f((i + 1) * tileWidth, j * tileWidth, 0);
                    glVertex3f((i + 1) * tileWidth, (j + 1) * tileWidth, 0);
                    glVertex3f(i * tileWidth, (j + 1) * tileWidth, 0);
                }
                glEnd();
            }
        }
        
    }

    // Color getColor(point p) {
    //     int x = (p.x + 5000) / tileWidth;
    //     int y = (p.y + 5000) / tileWidth;

    //     if((x+y)%2 == 0) return Color(0,0,0);
    //     else return Color(1,1,1);
    // }

    Color getColor(point p) {
        int x = (p.x + 5000) / tileWidth;
        int y = (p.y + 5000) / tileWidth;

        if ((x+y) % 2 == 1) return Color(0,0,0);
        else return Color(1,1,1);
    }


    double getT(Ray r) {
        point normal = point(0,0,1);
        double t = (refPoint-r.start).dot(normal) / (r.dir.dot(normal));
        if(t < 0) return -1;
        point p = r.start + r.dir*t;
        return t;
    }

    Ray getNormal(point p) {

        point normal = point(0.0,0.0,1.0);
        Ray r = Ray(p, normal);
        return r;
    }
    friend istream& operator>>(istream& is, Floor& p)
    {
        p.refPoint = point(0,0,0);
        p.specCoeff = 0.0;
        is >> p.tileWidth;
        is >> p.ambCoeff >> p.difCoeff >> p.reflCoeff;
        p.shine = 0;
        return is;
    }
};

class Pyramid: public Shape {
    Triangle triangles[6];
    double height, length;
    
public:
    Pyramid()
    {

    }
    void draw()
    {
       // cout << "Drawing pyramid" << endl;
        // Create the vertices of the pyramid
        point apex(refPoint.x + length / 2.0f, refPoint.y + length / 2.0f , refPoint.z + height);
        point frontLeft(refPoint.x, refPoint.y+ length, refPoint.z);
        point frontRight(refPoint.x + length, refPoint.y+ length, refPoint.z);
        point backLeft(refPoint.x, refPoint.y, refPoint.z);
        point backRight(refPoint.x + length, refPoint.y, refPoint.z);
        // bottom left corner is the refPoint
        // Define the triangles of the pyramid
        triangles[0] = Triangle(apex, frontLeft, frontRight, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);  // Front
        triangles[1] = Triangle(apex, frontRight, backRight, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);  // Right
        triangles[2] = Triangle(apex, backRight, backLeft, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);    // Back
        triangles[3] = Triangle(apex, backLeft, frontLeft, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);    // Left
        triangles[4] = Triangle(frontLeft, frontRight, backRight, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);  // Base bottom (front-right-back)
        triangles[5] = Triangle(frontLeft, backRight, backLeft, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);    // Base bottom (front-back-left)
        
        for(int i=0; i<6; i++) triangles[i].draw();
    }
    Color getColor(point p) {
        return color;
    }

    double getT(Ray r) {
        double tMin = 10000000.0;
        for(int i=0; i<6; i++) {
            double t = triangles[i].getT(r);
            if(t > 0 && t < tMin) tMin = t;
        }
        if(tMin == 10000000.0) return -1;
        return tMin;
    }

    Ray getNormal(point p) {

        for(int i=0; i<6; i++) {
            // barycentric coordinates
            double alpha = triangles[i].getArea(p, triangles[i].b, triangles[i].refPoint) / triangles[i].getArea(triangles[i].a, triangles[i].b, triangles[i].refPoint);
            double beta = triangles[i].getArea(p, triangles[i].refPoint, triangles[i].a) / triangles[i].getArea(triangles[i].a, triangles[i].b, triangles[i].refPoint);
            double gamma = triangles[i].getArea(p, triangles[i].a, triangles[i].b) / triangles[i].getArea(triangles[i].a, triangles[i].b, triangles[i].refPoint);
            if(alpha + beta + gamma > 1.0001) continue;

            point normal = (triangles[i].b-triangles[i].a)*(triangles[i].refPoint-triangles[i].a);
            normal = normal.normalize();
            Ray r = Ray(p, normal);
            return r;
        }

        return Ray();
    }
    friend ifstream& operator>>(ifstream& is, Pyramid& p)
    {
        is >> p.refPoint.x >> p.refPoint.y >> p.refPoint.z;
        is >> p.length >> p.height;
        is >> p.color.r >> p.color.g >> p.color.b;
        is >> p.ambCoeff >> p.difCoeff >> p.specCoeff >> p.reflCoeff;
        is >> p.shine;
        return is;
    }
    friend ostream& operator<<(ostream& os, const Pyramid& p)
    {
        os << "Pyramid:" << endl;
        os << p.refPoint << " " << p.length << " " << p.height << " " << p.color << " " << p.ambCoeff << " " << p.difCoeff << " " << p.specCoeff << " " << p.reflCoeff << " " << p.shine;
        return os;
    }
};

class Cube: public Shape {
    Triangle triangles[12];
    double length;

public:
    Cube()
    {
        length = 0.0;
    }
    void draw()
    {
       // cout << "Drawing cube" << endl;

        point frontLeftBottom(refPoint.x, refPoint.y, refPoint.z);
        point frontRightBottom(refPoint.x + length, refPoint.y, refPoint.z);
        point frontRightTop(refPoint.x + length, refPoint.y + length, refPoint.z);
        point frontLeftTop(refPoint.x, refPoint.y + length, refPoint.z);
        point backLeftBottom(refPoint.x, refPoint.y, refPoint.z + length);
        point backRightBottom(refPoint.x + length, refPoint.y, refPoint.z + length);
        point backRightTop(refPoint.x + length, refPoint.y + length, refPoint.z + length);
        point backLeftTop(refPoint.x, refPoint.y + length, refPoint.z + length);
        // bottom left corner is the refPoint
        // Front face
        triangles[0] = Triangle(frontLeftBottom, frontRightBottom, frontRightTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        triangles[1] = Triangle(frontLeftBottom, frontRightTop, frontLeftTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        // Right face
        triangles[2] = Triangle(frontRightBottom, backRightBottom, backRightTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        triangles[3] = Triangle(frontRightBottom, backRightTop, frontRightTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        // Back face
        triangles[4] = Triangle(backRightBottom, backLeftBottom, backLeftTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        triangles[5] = Triangle(backRightBottom, backLeftTop, backRightTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        // Left face
        triangles[6] = Triangle(backLeftBottom, frontLeftBottom, frontLeftTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        triangles[7] = Triangle(backLeftBottom, frontLeftTop, backLeftTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        // Top face
        triangles[8] = Triangle(frontLeftTop, frontRightTop, backRightTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        triangles[9] = Triangle(frontLeftTop, backRightTop, backLeftTop, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        // Bottom face
        triangles[10] = Triangle(frontLeftBottom, frontRightBottom, backRightBottom, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        triangles[11] = Triangle(frontLeftBottom, backRightBottom, backLeftBottom, color, shine, ambCoeff, difCoeff, specCoeff, reflCoeff);
        
        for(int i=0; i<12; i++) triangles[i].draw();
    }
    Color getColor(point p) {
        return color;
    }

    double getT(Ray r) {
        double tMin = 10000000.0;
        for(int i=0; i<12; i++) {
            double t = triangles[i].getT(r);
            if(t > 0 && t < tMin) tMin = t;
        }
        if(tMin == 10000000.0) return -1;
        return tMin;
    }

    Ray getNormal(point p) {
        for(int i=0; i<12; i++) {
            // barycentric coordinates
            double alpha = triangles[i].getArea(p, triangles[i].b, triangles[i].refPoint) / triangles[i].getArea(triangles[i].a, triangles[i].b, triangles[i].refPoint);
            double beta = triangles[i].getArea(p, triangles[i].refPoint, triangles[i].a) / triangles[i].getArea(triangles[i].a, triangles[i].b, triangles[i].refPoint);
            double gamma = triangles[i].getArea(p, triangles[i].a, triangles[i].b) / triangles[i].getArea(triangles[i].a, triangles[i].b, triangles[i].refPoint);
            if(alpha + beta + gamma > 1.0001) continue;
            
            point normal = (triangles[i].b-triangles[i].a)*(triangles[i].refPoint-triangles[i].a);
            normal = normal.normalize();
            Ray r = Ray(p, normal);
            return r;
        }

        return Ray();
    }
    friend ifstream& operator>>(ifstream& is, Cube& p)
    {
        is >> p.refPoint.x >> p.refPoint.y >> p.refPoint.z;
        is >> p.length;
        is >> p.color.r >> p.color.g >> p.color.b;
        is >> p.ambCoeff >> p.difCoeff >> p.specCoeff >> p.reflCoeff;
        is >> p.shine;
        return is;
    }
    friend ostream& operator<<(ostream& os, const Cube& p)
    {
        cout << "Cube:" << endl;
        os << p.refPoint << " " << p.length << " " << p.color << " " << p.ambCoeff << " " << p.difCoeff << " " << p.specCoeff << " " << p.reflCoeff << " " << p.shine;
        return os;
    }
};