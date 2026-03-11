#include <iostream>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"

using namespace std;

const double G = 6.6743e-11; // m^3 kg^-1 s^-2
const float c = 299792458.0;
const double WORLD_SCALE = 400000000.0;
const double TIME_SCALE = 360000.0;

class Body {
private:
    VAO vao;
    VBO* vbo;
    int vertexCount;
    double renderX;
    double renderY;
    double renderRadius;

    vector<float> trail;
    VAO trailVAO;
    VBO* trailVBO;
    int trailLength = 200;
public:
    double radius;
    int res = 100;
    double x, y;
    double vx, vy;
    double mass;
    double ax, ay;

    Body(double x, double y, double radius, double vx, double vy, double mass) : x(x), y(y), radius(radius), vx(vx), vy(vy), mass(mass) {
        renderX = x / WORLD_SCALE;
        renderY = y / WORLD_SCALE;
        renderRadius = radius / WORLD_SCALE;

        vector<float> vertices;
        vertices.push_back(renderX);
        vertices.push_back(renderY);

        for (int i = 0; i <= res; ++i) {
            float angle = 2.0f * 3.14159265358f * (static_cast<float>(i) / res);
            double vertices_x = renderX + cos(angle) * renderRadius;
            double vertices_y = renderY + sin(angle) * renderRadius;
            vertices.push_back(vertices_x);
            vertices.push_back(vertices_y);
        }


        vertexCount = vertices.size() / 2;

        vbo = new VBO(vertices);
        vao.Bind();
        vao.LinkVBO(*vbo, 0);
        vao.Unbind();
        vbo->Unbind();

        trail.push_back(renderX);
        trail.push_back(renderY);

        trailVBO = new VBO(trail);

        trailVAO.Bind();
        trailVAO.LinkVBO(*trailVBO, 0);
        trailVAO.Unbind();
        trailVBO->Unbind();
    }

    void Draw(Shader& shader) {
        shader.Activate();

        renderX = x / WORLD_SCALE;
        renderY = y / WORLD_SCALE;
        renderRadius = radius / WORLD_SCALE;

        vector<float> vertices;
        vertices.push_back(renderX);
        vertices.push_back(renderY);

        for (int i = 0; i <= res; ++i) {
            float angle = 2.0f * 3.14159265358f * (static_cast<float>(i) / res);
            double vertices_x = renderX + cos(angle) * renderRadius;
            double vertices_y = renderY + sin(angle) * renderRadius;
            vertices.push_back(vertices_x);
            vertices.push_back(vertices_y);
        }

        vbo->Bind();
        vbo->Update(vertices);

        vao.Bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    }

    void Update(double dt)
    {
        vx += ax * dt;
        vy += ay * dt;

        x += vx * dt;
        y += vy * dt;

        double rx = x / WORLD_SCALE;
        double ry = y / WORLD_SCALE;

        trail.push_back(rx);
        trail.push_back(ry);

        if (trail.size() > trailLength * 2)
        {
            trail.erase(trail.begin(), trail.begin() + 2);
        }

        ax = 0;
        ay = 0;
    }

    void DrawTrail(Shader& shader)
    {
        if (trail.size() < 4) return;

        shader.Activate();

        trailVAO.Bind();
        trailVBO->Bind();

        glBufferData(GL_ARRAY_BUFFER,
                     trail.size() * sizeof(float),
                     trail.data(),
                     GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINE_STRIP, 0, trail.size() / 2);
    }

};

void ApplyGravity(vector<Body>& bodyList, double dt)
{
    for (Body& body : bodyList)
    {
        body.ax = 0;
        body.ay = 0;
    }

    for (Body& bodyA : bodyList)
    {
        for (Body& bodyB : bodyList)
        {
            if (&bodyA == &bodyB)
                continue;

            double dx = bodyB.x - bodyA.x;
            double dy = bodyB.y - bodyA.y;

            double r2 = dx*dx + dy*dy + 1e-6;
            double r = sqrt(r2);

            double factor = G * bodyB.mass / (r*r*r);

            bodyA.ax += factor * dx;
            bodyA.ay += factor * dy;
        }
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int screenWidth = 800;
    int screenHeight = 800;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "nBodySimulation", NULL, NULL);
    if (!window)
    {
        cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGL();
    glViewport(0, 0, 800, 800);

    Shader shaderProgram("Shaders/default.vert", "Shaders/default.frag");
    Shader planetShader("Shaders/default.vert", "Shaders/planet.frag");

    vector<Body> bodyList;

    bodyList.emplace_back(0, 0, 18'371'000, 0, 0, 5.972e24);
    bodyList.emplace_back(384400000.0, 0, 6'737'400, 0, 1022, 7.342e22);

    double lastTime = glfwGetTime();

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double dt = (currentTime - lastTime) * TIME_SCALE;
        lastTime = currentTime;
        glClear(GL_COLOR_BUFFER_BIT);

        ApplyGravity(bodyList, dt);

        glLineWidth(2.0f);

        bodyList[0].Update(dt);
        bodyList[1].Update(dt);

        bodyList[0].DrawTrail(shaderProgram);
        bodyList[1].DrawTrail(shaderProgram);

        bodyList[0].Draw(shaderProgram);
        bodyList[1].Draw(planetShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}