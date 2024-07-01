#include <iostream>
#include <cmath>
#include <fstream>
#include <stack>
#include <vector>
#include <limits>
#include <iomanip>
#include <string>
#include "bitmap_image.hpp"
using namespace std;

// global variables
class Triangle
{
public:
    double points[3][3];
    double colors[3];
};
class matrix
{
public:
    double m[4][4];
    // constructor that takes a 2d array and copies it to the matrix
    matrix()
    {
        int i, j;
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                if (i == j)
                    m[i][j] = 1;
                else
                    m[i][j] = 0;
            }
        }
    }
    void print()
    {
        int i, j;
        cout << "Matrix:\n";
        for (i = 0; i < 4; i++)
        {
            cout << "\t";
            for (j = 0; j < 4; j++)
            {
                cout << m[i][j] << " ";
            }
            cout << endl;
        }
    }
};

// initialize a 4*4 identity matrix
matrix identity;
// initialize a 4*4 matrix for translation
matrix translation;
// initialize a 4*4 matrix for scaling
matrix scaling;
// initialize a 4*4 matrix for rotation
matrix rotation;

double triangle[4];

stack<matrix> stck;
// multiply a matrix and a 1D array
void multiply(matrix a)
{
    int i, j;
    double sum = 0;
    double c[4];
    for (i = 0; i < 4; i++)
    {
        sum = 0;
        for (j = 0; j < 4; j++)
        {
            sum += a.m[i][j] * triangle[j];
        }
        c[i] = sum;
    }
    //copy c in triangle
    for (i = 0; i < 4; i++)
    {
        triangle[i] = c[i];
    }
}

vector<double> multiply(matrix a, vector<double> c)
{
    int i, j;
    double sum = 0;
    vector<double> d(4, 0);
    for (i = 0; i < 4; i++)
    {
        sum = 0;
        for (j = 0; j < 4; j++)
        {
            sum += a.m[i][j] * c[j];
        }
        d[i] = sum;
    }
    return d;
}

// multiply 2 4*4 matrices
void multiply(matrix a, matrix b)
{
    double c[4][4];
    int i, j, k;
    double sum = 0;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            sum = 0;
            for (k = 0; k < 4; k++)
            {
                sum += a.m[i][k] * b.m[k][j];
            }
            c[i][j] = sum;
        }
    }
    // copy c in identity
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            identity.m[i][j] = c[i][j];
        }
    }
}

// multiply 2 4*4 matrices
matrix multiply2(matrix a, matrix b)
{
    matrix c;
    int i, j, k;
    double sum = 0;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            sum = 0;
            for (k = 0; k < 4; k++)
            {
                sum += a.m[i][k] * b.m[k][j];
            }
            c.m[i][j] = sum;
        }
    }
    return c;
}

vector<double> RodriguesValue(vector<double>x,vector<double>a,double angle){
    vector<double>result(3,0);
    double cosValue=cos(angle);
    double sinValue=sin(angle);
    result[0]=x[0]*cosValue+(1-cosValue)*(a[0]*a[0]*x[0]+a[0]*a[1]*x[1]+a[0]*a[2]*x[2])+sinValue*(a[1]*x[2]-a[2]*x[1]);
    result[1]=x[1]*cosValue+(1-cosValue)*(a[0]*a[1]*x[0]+a[1]*a[1]*x[1]+a[1]*a[2]*x[2])+sinValue*(a[2]*x[0]-a[0]*x[2]);
    result[2]=x[2]*cosValue+(1-cosValue)*(a[0]*a[2]*x[0]+a[1]*a[2]*x[1]+a[2]*a[2]*x[2])+sinValue*(a[0]*x[1]-a[1]*x[0]);
    return result;
}

//normalize e vector
vector<double> normalize(vector<double>v){
    vector<double>result(3,0);
    double sum=0;
    for(int i=0;i<3;i++){
        sum+=v[i]*v[i];
    }
    sum=sqrt(sum);
    for(int i=0;i<3;i++){
        result[i]=v[i]/sum;
    }
    return result;
}

//getL vector
vector<double> getL(vector<double>eye,vector<double>look){
    vector<double>L(3,0);
    L[0]=look[0]-eye[0];
    L[1]=look[1]-eye[1];
    L[2]=look[2]-eye[2];
    L=normalize(L);
    return L;
}

//getR vector
vector<double> getR(vector<double>L,vector<double>up){
    vector<double>R(3,0);
    R[0]=L[1]*up[2]-L[2]*up[1];
    R[1]=L[2]*up[0]-L[0]*up[2];
    R[2]=L[0]*up[1]-L[1]*up[0];
    R = normalize(R);
    return R;
}

//getU vector
vector<double> getU(vector<double>R,vector<double>L){
    vector<double>U(3,0);
    U[0]=R[1]*L[2]-R[2]*L[1];
    U[1]=R[2]*L[0]-R[0]*L[2];
    U[2]=R[0]*L[1]-R[1]*L[0];
    U = normalize(U);
    return U;
}

std::string readAndTrimTrailingSpaces(std::string const & filePath)
{
    std::ifstream file(filePath);
    std::string   buffer(std::istreambuf_iterator<char>{file}, {});

    while (!buffer.empty() && std::isspace(buffer.back()))
        buffer.pop_back();

    return buffer;
}

static unsigned long int g_seed = 1; 
inline int random() 
{ 
    g_seed = (214013 * g_seed + 2531011); 
    return (g_seed >> 16) & 0x7FFF; 
}

int main(int argc, char **argv)
{
    cout << fixed << setprecision(7);
    // cout << "here\n";
    // initialize a file
    ifstream fp("scene.txt");
    ofstream fp2("temp.txt");
    fp2 << fixed << setprecision(7);
    vector<double> eye(3, 0);
    vector<double> look(3, 0);
    vector<double> up(3, 0);
    double fovY, aspect, n, f;

    fp >> eye[0] >> eye[1] >> eye[2];
    // cout << eye[0] << " " << eye[1] << " " << eye[2] << endl;
    fp >> look[0] >> look[1] >> look[2];
    // cout << look[0] << " " << look[1] << " " << look[2] << endl;
    fp >> up[0] >> up[1] >> up[2];
    // cout << up[0] << " " << up[1] << " " << up[2] << endl;
    fp >> fovY >> aspect >> n >> f;
    // cout << fovY << " " << aspect << " " << n << " " << f << endl;

    string command;
    double x1, y1, z1;
    double tx, ty, tz;
    double sx, sy, sz;
    double rx, ry, rz, angle;
    while (!fp.eof())
    {
        fp >> command;
        // cout << command << endl;
        if (command.compare("triangle") == 0)
        {
            //cout << "Inside triangle\n";
            for (int i = 0; i < 3; i++)
            {
                fp >> x1 >> y1 >> z1;
                // cout << x1 << " " << y1 << " " << z1 << endl;
                // store the coordinates in a 2d array
                triangle[0] = x1;
                triangle[1] = y1;
                triangle[2] = z1;
                triangle[3] = 1;

                multiply(identity);

                if (triangle[3] != 1)
                {
                    triangle[0] = triangle[0] / triangle[3];
                    triangle[1] = triangle[1] / triangle[3];
                    triangle[2] = triangle[2] / triangle[3];
                }
                for (int p = 0; p < 3; p++)
                {
                    fp2 << triangle[p] << " ";
                }
                fp2 << endl;
            }
            fp2 << endl;
        }
        else if (command.compare("translate") == 0)
        {
            fp >> tx >> ty >> tz;
            // cout << tx << " " << ty << " " << tz << endl;
            // store the translation values in a 2d array
            translation.m[0][3] = tx;
            translation.m[1][3] = ty;
            translation.m[2][3] = tz;

            // translation.print();
            multiply(identity, translation);
            // cout << "printing M" << endl;
            // identity.print();
        }
        else if (command.compare("rotate") == 0)
        {
            fp >> angle >> rx >> ry >> rz;
            angle = M_PI/180 * angle;
            // cout << angle << " " << rx << " " << ry << " " << rz << endl;
            double nx = rx / sqrt(rx * rx + ry * ry + rz * rz);
            double ny = ry / sqrt(rx * rx + ry * ry + rz * rz);
            double nz = rz / sqrt(rx * rx + ry * ry + rz * rz);
            
            vector<double> a;
            a.push_back(nx);
            a.push_back(ny);
            a.push_back(nz);

            vector<double> i;
            i.push_back(1);
            i.push_back(0);
            i.push_back(0);

            vector<double> j;
            j.push_back(0);
            j.push_back(1);
            j.push_back(0);

            vector<double> k;
            k.push_back(0);
            k.push_back(0);
            k.push_back(1);

            vector<double> c1 = RodriguesValue(i, a, angle);

            rotation.m[0][0] = c1[0];
            rotation.m[1][0] = c1[1];
            rotation.m[2][0] = c1[2];

            vector<double> c2 = RodriguesValue(j, a, angle);
            rotation.m[0][1] = c2[0];
            rotation.m[1][1] = c2[1];
            rotation.m[2][1] = c2[2];

            vector<double> c3 = RodriguesValue(k, a, angle);
            rotation.m[0][2] = c3[0];
            rotation.m[1][2] = c3[1];
            rotation.m[2][2] = c3[2];

            // rotation.print();
            multiply(identity, rotation);
            // cout << "printing M" << endl;
            // identity.print();
        }
        else if (command.compare("scale") == 0)
        {
            fp >> sx >> sy >> sz;
            // cout << sx << " " << sy << " " << sz << endl;
            // store the scaling values in a 2d array
            scaling.m[0][0] = sx;
            scaling.m[1][1] = sy;
            scaling.m[2][2] = sz;

            // scaling.print();
            multiply(identity, scaling);
           // cout << "printing M" << endl;
            // identity.print();
        }
        else if (command.compare("push") == 0)
        {
            // push the current matrix to the stack
            // push the current matrix to the stack
            stck.push(identity);
        }
        else if (command.compare("pop") == 0)
        {
            // pop the current matrix from the stack
            identity = stck.top();
            stck.pop();
        }
        else if (command.compare("end") == 0)
        {
            // end the program
            break;
        }
        else
        {
            // error
            cout << "Invalid command" << endl;
        }
    }
    fp.close();
    fp2.close();

    string temporary = readAndTrimTrailingSpaces("temp.txt");
    //write the string to a file
    ofstream fpt;
    fpt.open("stage1.txt");
    fpt << fixed << setprecision(7);
    fpt << temporary;
    fpt.close();
    remove("temp.txt");

    vector<double> l = getL(eye, look);
    vector<double> r = getR(l, up);
    vector<double> u = getU(r, l);

    matrix T;
    T.m[0][3] = -eye[0];
    T.m[1][3] = -eye[1];
    T.m[2][3] = -eye[2];

    matrix R;
    R.m[0][0] = r[0];
    R.m[0][1] = r[1];
    R.m[0][2] = r[2];
    R.m[1][0] = u[0];
    R.m[1][1] = u[1];
    R.m[1][2] = u[2];
    R.m[2][0] = -l[0];
    R.m[2][1] = -l[1];
    R.m[2][2] = -l[2];

    matrix V = multiply2(R, T);

    ifstream fp3("stage1.txt");
    ofstream fp4("temp2.txt");
    
    // fp4 << setprecision(numeric_limits<double>::digits10 + 1);
    fp4 << fixed << setprecision(7);

    vector<double> array(4, 0);
    int jj = 0;
    while(!fp3.eof())
    {
        double x1, y1, z1;
        vector<double> newP;
        fp3 >> x1 >> y1 >> z1;
        //cout << x1 << " " << y1 << " " << z1 << endl;
        jj++;
        array[0] = x1;
        array[1] = y1;
        array[2] = z1;
        array[3] = 1;

        newP = multiply(V, array);
        fp4 << newP[0]/newP[3] << " " << newP[1]/newP[3] << " " << newP[2]/newP[3] << endl;
        if(jj == 3)
        {
            fp4 << endl;
            jj = 0;
        }
    }   

    fp3.close();
    fp4.close();
    
    string temporary2 = readAndTrimTrailingSpaces("temp2.txt");
    //write the string to a file
    ofstream fpt2;
    fpt2.open("stage2.txt");
    fpt2 << fixed << setprecision(7);
    fpt2 << temporary2;
    fpt2.close();
    remove("temp2.txt");

    ifstream fp5("stage2.txt");
    ofstream fp6("stage3.txt");

    // fp6 << setprecision(numeric_limits<double>::digits10 + 1);
    fp6 << fixed << setprecision(7);

    double fovX = fovY * aspect;
    double tt = n * tan((fovY / 2) * (acos(-1) / 180));
    double rr = n * tan((fovX / 2) * (acos(-1) / 180));

    matrix P;
    P.m[0][0] = n / rr;
    P.m[1][1] = n / tt;
    P.m[2][2] = -(f + n) / (f - n);
    P.m[2][3] = -(2 * f * n) / (f - n);
    P.m[3][2] = -1;
    P.m[3][3] = 0;

    vector<double> array2(4, 0);
    int kk = 0;
    while(!fp5.eof())
    {
        double x2, y2, z2;
        vector<double> newP2;
        fp5 >> x2 >> y2 >> z2;
        kk++;
        array2[0] = x2;
        array2[1] = y2;
        array2[2] = z2;
        array2[3] = 1;

        newP2 = multiply(P, array2);
        fp6 << newP2[0]/newP2[3] << " " << newP2[1]/newP2[3] << " " << newP2[2]/newP2[3] << endl;
        if(kk == 3)
        {
            fp6 << endl;
            kk = 0;
        }
    }

    // string temporary3 = readAndTrimTrailingSpaces("t3.txt");
    // //write the string to a file
    // ofstream fpt3;
    // fpt3.open("stage3.txt");
    // fpt3 << fixed << setprecision(7);
    // fpt3 << temporary3;
    // fpt3.close();
    // remove("t3.txt");

    ifstream fp7("config.txt");
    int screen_width, screen_height;
    fp7 >> screen_width >> screen_height;
    // cout << screen_width << " " << screen_height << endl;
    fp7.close();
    fp7.open("stage3.txt");
    ofstream fp8("z_buffer.txt");

    fp8 << fixed << setprecision(7);

    double z_max = 1.0;
    double boxRight, boxLeft, boxTop, boxBottom;
    boxRight = boxTop = 1;
    boxLeft = boxBottom = -1;

    double dx, dy;
    dx = (boxRight-boxLeft) / screen_width;
    dy = (boxTop-boxBottom) / screen_height;

    // cout << "dx: " << dx << " dy: " << dy << endl;

    double topY, bottomY, leftX, rightX;

    topY = boxTop - (dy / 2);
    bottomY = boxBottom + (dy / 2);
    leftX = boxLeft + (dx / 2);
    rightX = boxRight - (dx / 2);

    bitmap_image *image;
    image = new bitmap_image(screen_width, screen_height);

    for(int i = 0; i < screen_height; i++)
    {
        for(int j = 0; j < screen_width; j++)
        {
            image->set_pixel(j, i, 0, 0, 0);
        }
    }

    double **z_buffer = new double *[screen_height];
    for (int i = 0; i < screen_height; i++)
    {
        z_buffer[i] = new double[screen_width];
    }

    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            z_buffer[i][j] = z_max;
        }
    }

    while(!fp7.eof())
    {
        Triangle t;
        for(int i = 0; i < 3; i++)
        {
            fp7 >> t.points[i][0] >> t.points[i][1] >> t.points[i][2];
            // cout << t.points[i][0] << " " << t.points[i][1] << " " << t.points[i][2] << endl;
        }
        for(int i = 0; i < 3; i++)
        {
            t.colors[i] = random() % 256;
        }
        // cout << t.colors[0] << " " << t.colors[1] << " " << t.colors[2] << endl;

        //min max values of the triangle
        double xmax, xmin, ymax, ymin;

        xmax = max(t.points[0][0], max(t.points[1][0], t.points[2][0]));
        xmin = min(t.points[0][0], min(t.points[1][0], t.points[2][0]));

        ymax = max(t.points[0][1], max(t.points[1][1], t.points[2][1]));
        ymin = min(t.points[0][1], min(t.points[1][1], t.points[2][1]));

        //clipping
        xmin = max(xmin, leftX);
        xmax = min(xmax, rightX);
        ymin = max(ymin, bottomY);
        ymax = min(ymax, topY);

        // cout << "xmin: " << xmin << " xmax: " << xmax << " ymin: " << ymin << " ymax: " << ymax << endl;
        //find the equation of the plane passing through triangle
        double a1 = t.points[1][0] - t.points[0][0];
        double b1 = t.points[1][1] - t.points[0][1];
        double c1 = t.points[1][2] - t.points[0][2];
        double a2 = t.points[2][0] - t.points[0][0];
        double b2 = t.points[2][1] - t.points[0][1];
        double c2 = t.points[2][2] - t.points[0][2];
        double a = b1 * c2 - b2 * c1;
        double b = a2 * c1 - a1 * c2;
        double c = a1 * b2 - b1 * a2;
        double d = (- a * t.points[0][0] - b * t.points[0][1] - c * t.points[0][2]);
        // cout << "a: " << a << " b: " << b << " c: " << c << " d: " << d << endl;

        int startY = round((topY-ymin)/dy);
        int endY = round((topY-ymax)/dy);
        //cout<<"startY: "<<startY<<" endY: "<<endY<<endl;

        for(int scanY = endY+1; scanY <= startY; scanY++)
        {
            //cout << "Here\n";
            double y = topY - scanY * dy;
            double xx1[2];
            int cnt=0;
            for(int i = 0; i < 3; i++)
            {
                int j = (i+1) % 3;

                if(t.points[i][1] == t.points[j][1])
                {
                    if(y == t.points[i][1])
                    {
                        xx1[cnt] = min(t.points[i][0], t.points[j][0]);
                        xx1[cnt+1] = max(t.points[i][0], t.points[j][0]);
                        //cnt++;
                    }
                    continue;
                }
                    
                if(y >= min(t.points[i][1], t.points[j][1]) && y <= max(t.points[i][1], t.points[j][1])){
                    xx1[cnt] = t.points[i][0] + (y - t.points[i][1]) * (t.points[j][0] - t.points[i][0]) / (t.points[j][1] - t.points[i][1]);
                    cnt++;
                }
                
            }

            for(int i = 0; i < 2; i++)
            {
                if(xx1[i] < xmin) xx1[i] = xmin;
                if(xx1[i] > xmax) xx1[i] = xmax;
            }

            if(xx1[0] >= xx1[1])
            {
                double temp = xx1[0];
                xx1[0] = xx1[1];
                xx1[1] = temp;
            }
            //cout << "xx1[0]: " << xx1[0] << " xx1[1]: " << xx1[1] << endl;
            int startX = round((xx1[0] - leftX) / dx);
            int endX = round((xx1[1] - leftX) / dx);

            // cout << "startX: " << startX << " endX: " << endX << endl;
            for(int scanX = startX; scanX < endX; scanX++)
            {
                //cout << "Inside here\n";
                double x = leftX + scanX * dx;
                double z = (-d - a * x - b * y) / c;

                if(z < -1) continue;
                if(z < z_buffer[scanY][scanX])
                {
                    z_buffer[scanY][scanX] = z;
                    image->set_pixel(scanX, scanY, t.colors[0], t.colors[1], t.colors[2]);
                }
            }
        }
    }
    fp7.close();

    image->save_image("out.bmp");

    //write z_buffer to a file
    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            if(z_buffer[i][j] < z_max)
                fp8 << z_buffer[i][j] << "\t";
        }
        fp8 << endl;
    }
    fp8.close();
    //delete the z_buffer
    for (int i = 0; i < screen_height; i++)
    {
        delete[] z_buffer[i];
    }
    delete[] z_buffer;

    //delete the image
    delete image;

    // cout << "done" << endl;
    return 0;
}