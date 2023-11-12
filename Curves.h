#ifndef curves_h
#define curves_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
//#include "Camera.h"

using namespace std;

class Curves {

public:

    float wcs[3];
    vector <float> cntrlPoints;
    vector <float> coordinates;
    vector <float> normals;
    vector <int> indices;
    vector <float> vertices;

    GLint viewport[4];

    const double pi = 3.14159265389;
    const int nt = 40;
    const int ntheta = 20;
    bool showControlPoints = true;
    bool loadBezierCurvePoints = false;
    bool showHollowBezier = false;
    bool lineMode = false;
    unsigned int bezierVAO;

    glm::mat4 projection, view, translate, rotate, scale, model, identity;

    Curves()
    {
        identity = glm::mat4(1.0f);
    }

    ~Curves()
    {
        glDeleteVertexArrays(1, &bezierVAO);
    }
    /// <summary>
    /// provide only 2d points in the following manner [x1, y1, x2, y2]
    /// </summary>
    /// <param name="points"></param>
    void setControlPoints(vector<float>& points)
    {
        int numOfPoints = points.size();
        for (int i = 0; i < numOfPoints; i+=2) {
            scsToWcs(points[i], points[i+1], wcs);
            cntrlPoints.push_back(wcs[0]);
            cntrlPoints.push_back(wcs[1]);
            cntrlPoints.push_back(wcs[2]);
        }
        bezierVAO = hollowBezier(cntrlPoints.data(), ((unsigned int)cntrlPoints.size() / 3) - 1);
    }

    void setProjView(glm::mat4 proj, glm::mat4 vw)
    {
        this->projection = proj;
        this->view = vw;        
    }

    void setModel(glm::mat4 md)
    {
        this->model = md;
    }

    void setViewport(GLint vpInfo[])
    {
        for (int i = 0; i < 4; i++)
            this->viewport[i] = vpInfo[i];
    }

    void drawCurves(Shader& shader)
    {
        shader.use();
        shader.setBool("exposedToSun", true);
        shader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 1.0f));
        shader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 1.0f));
        shader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("material.shininess", 32.0f);

        glBindVertexArray(bezierVAO);
        glDrawElements(GL_TRIANGLES,                    // primitive type
            (unsigned int)indices.size(),          // # of indices
            GL_UNSIGNED_INT,                 // data type
            (void*)0);                       // offset to indices

        // unbind VAO
        glBindVertexArray(0);
    }

    

private:

    void scsToWcs(float sx, float sy, float wcsv[3])
    {

        glm::vec4 winCoordinateVector; //vector to hold screen x,y,z coordinates
        glm::vec4 worldCoordinateVector; //vector to hold world x,y,z coordinates

        //glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info

        // setup screen coordinates values
        winCoordinateVector.x = sx;
        winCoordinateVector.y = (float)viewport[3] - (float)sy; // reversed since y-coordinates go from bottom to top
        winCoordinateVector.z = 0.0f;
        winCoordinateVector.w = 1.0f;

        // prepare viewport matrix
        float v_l = viewport[0], v_b = viewport[1], v_r = viewport[2], v_t = viewport[3];
        glm::mat4 viewportMat = glm::mat4(1.0f);
        viewportMat[0].x = (v_r - v_l) / 2.0f;  viewportMat[1].x = 0.0f; viewportMat[2].x = 0.0f;  viewportMat[3].x = (v_r + v_l) / 2.0f;
        viewportMat[0].y = 0.0f; viewportMat[1].y = (v_t - v_b) / 2.0f;  viewportMat[2].y = 0.0f;  viewportMat[3].y = (v_t + v_b) / 2.0f;
        viewportMat[0].z = 0.0f; viewportMat[1].z = 0.0f; viewportMat[2].z = 0.5f;  viewportMat[3].z = 0.5f;
        viewportMat[0].w = 0.0f; viewportMat[1].w = 0.0f; viewportMat[2].w = 0.0f;  viewportMat[3].w = 1.0f;

        //get the world coordinates from the screen coordinates
        worldCoordinateVector = glm::inverse(viewportMat * projection * view * model) * winCoordinateVector;
        wcsv[0] = worldCoordinateVector.x / worldCoordinateVector.w;
        wcsv[1] = worldCoordinateVector.y / worldCoordinateVector.w;
        wcsv[2] = worldCoordinateVector.z / worldCoordinateVector.w;
    }

    long long nCr(int n, int r)
    {
        if (r > n / 2)
            r = n - r; // because C(n, r) == C(n, n - r)
        long long ans = 1;
        int i;

        for (i = 1; i <= r; i++)
        {
            ans *= n - r + i;
            ans /= i;
        }

        return ans;
    }

    void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L)
    {
        double y = 0;
        double x = 0;
        t = t > 1.0 ? 1.0 : t;
        for (int i = 0; i < L + 1; i++)
        {
            long long ncr = nCr(L, i);
            double oneMinusTpow = pow(1 - t, double(L - i));
            double tPow = pow(t, double(i));
            double coef = oneMinusTpow * tPow * ncr;
            x += coef * ctrlpoints[i * 3];
            y += coef * ctrlpoints[(i * 3) + 1];

        }
        xy[0] = float(x);
        xy[1] = float(y);
    }

    unsigned int hollowBezier(GLfloat ctrlpoints[], int L)
    {
        int i, j;
        float x, y, z, r;                //current coordinates
        float theta;
        float nx, ny, nz, lengthInv;    // vertex normal


        const float dtheta = 2 * pi / ntheta;        //angular step size

        float t = 0;
        float dt = 1.0 / nt;
        float xy[2];

        for (i = 0; i <= nt; ++i)              //step through y
        {
            BezierCurve(t, xy, ctrlpoints, L);
            r = xy[0];
            y = xy[1];
            theta = 0;
            t += dt;
            lengthInv = 1.0 / r;

            for (j = 0; j <= ntheta; ++j)
            {
                double cosa = cos(theta);
                double sina = sin(theta);
                z = r * cosa;
                x = r * sina;

                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                // normalized vertex normal (nx, ny, nz)
                // center point of the circle (0,y,0)
                nx = (x - 0) * lengthInv;
                ny = (y - y) * lengthInv;
                nz = (z - 0) * lengthInv;

                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);

                theta += dtheta;
            }
        }

        // generate index list of triangles
        // k1--k1+1
        // |  / |
        // | /  |
        // k2--k2+1

        int k1, k2;
        for (int i = 0; i < nt; ++i)
        {
            k1 = i * (ntheta + 1);     // beginning of current stack
            k2 = k1 + ntheta + 1;      // beginning of next stack

            for (int j = 0; j < ntheta; ++j, ++k1, ++k2)
            {
                // k1 => k2 => k1+1
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                // k1+1 => k2 => k2+1
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

        size_t count = coordinates.size();
        for (int i = 0; i < count; i += 3)
        {
            vertices.push_back(coordinates[i]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);

            vertices.push_back(normals[i]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);
        }

        unsigned int bezierVAO;
        glGenVertexArrays(1, &bezierVAO);
        glBindVertexArray(bezierVAO);

        // create VBO to copy vertex data to VBO
        unsigned int bezierVBO;
        glGenBuffers(1, &bezierVBO);
        glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);           // for vertex data
        glBufferData(GL_ARRAY_BUFFER,                   // target
            (unsigned int)vertices.size() * sizeof(float), // data size, # of bytes
            vertices.data(),   // ptr to vertex data
            GL_STATIC_DRAW);                   // usage

        // create EBO to copy index data
        unsigned int bezierEBO;
        glGenBuffers(1, &bezierEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierEBO);   // for index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
            (unsigned int)indices.size() * sizeof(unsigned int),             // data size, # of bytes
            indices.data(),               // ptr to index data
            GL_STATIC_DRAW);                   // usage

        // activate attrib arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // set attrib arrays with stride and offset
        int stride = 24;     // should be 24 bytes
        glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));

        // unbind VAO, VBO and EBO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return bezierVAO;
    }

};


#endif /* curves_h */