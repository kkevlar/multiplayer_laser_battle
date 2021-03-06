//YOU MUST PICK A USERNAME
#define USERNAME ((const char*)"INVALID")

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <math.h>

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>

#include "CustomTextBillboard.h"
#include "Explosion.h"
#include "GLSL.h"
#include "LaserManager.h"
#include "LaserRenderer.h"
#include "PlaneRenderer.h"
#include "Planes.h"
#include "PlanesNetworked.h"
#include "Program.h"
#include "ScoreManager.h"
#include "Scoreboard.h"
#include "Shape.h"
#include "WindowManager.h"
#include "byte_order.h"
#include "log.h"
#include "stb_image.h"


using namespace std;
using namespace glm;
#define MESHSIZE 1000
#define PI 3.1415926
#define SS_CENTER_USED_FOR_MOUSE_CONTROLS vec2(1920 / 2, 1080 / 2)

static mat4 safe_lookat(vec3 me, vec3 target, vec3 up)
{
    mat4 m1;
    vec3 ex, ey, ez;
    ey = up;
    ez = -normalize(target - me);
    ex = cross(ey, ez);
    m1[0][0] = ex.x;
    m1[0][1] = ex.y;
    m1[0][2] = ex.z;
    m1[0][3] = 0;
    m1[1][0] = ey.x;
    m1[1][1] = ey.y;
    m1[1][2] = ey.z;
    m1[1][3] = 0;
    m1[2][0] = ez.x;
    m1[2][1] = ez.y;
    m1[2][2] = ez.z;
    m1[2][3] = 0;
    m1[3][0] = 0;
    m1[3][1] = 0;
    m1[3][2] = 0;
    m1[3][3] = 1.0f;

    return m1;
}

#define GLOBAL_COLOR_COUNT (8)
static glm::vec3 global_plane_color_allocation[] = {

    vec3(0, 0, 1),
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(1, 0.4, 0),  // orange
    vec3(0, 1, 1),    // cyan
    vec3(1, 1, 0),    // yellow
    vec3(1, 0, 1),    // magenta
    vec3(0.4, 0, 1),  // purple
};

class camera
{
   public:
    glm::vec3 pos, rot;
    int w, a, s, d, q, e, z, c, sp, shft, ctrl;
    camera()
    {
        w = a = s = d = q = e = z = c = 0;
        pos = rot = glm::vec3(0, 0, 0);
    }
    glm::mat4 process(double ftime, vec3 campos, vec3 forward, vec3 up, vec3 right)
    {
        float default_rotation_angle = -1.0 * PI / 2.0;
        mat4 rotate_default_plane = rotate(mat4(1), default_rotation_angle, vec3(0, 1, 0));

        mat4 R = mat4(1);
        R[0][0] = forward.x;
        R[0][1] = forward.y;
        R[0][2] = forward.z;
        R[1][0] = up.x;
        R[1][1] = up.y;
        R[1][2] = up.z;
        R[2][0] = right.x;
        R[2][1] = right.y;
        R[2][2] = right.z;

        pos = pos + (campos - pos) * 0.06f;
        glm::mat4 T = glm::translate(glm::mat4(1), pos);
        return inverse(rotate_default_plane) * inverse(R) * T;
    }
};

// TODO these should be fields of the Application class
camera mycam;
// TODO BRING THIS BACK
// vector<npc> thebots;
shared_ptr<Shape> shape;
int renderstate = 1;

class Application : public EventCallbacks
{
   public:
    WindowManager* windowManager = nullptr;

    // Our shader program
    std::shared_ptr<Program> prog, heightshader, skyprog, linesshader;

    // Contains vertex information for OpenGL
    GLuint VertexArrayID;

    // Data necessary to give our box to OpenGL
    GLuint MeshPosID, MeshTexID, IndexBufferIDBox;

    // texture data
    GLuint Texture, AudioTex, AudioTexBuf;
    GLuint Texture2, HeightTex;

    std::string my_username;
    player theplayer;
    Explosion explosion;
    LaserRenderer laser;
    PlaneRenderer plane_renderer;
    CustomTextBillboard custom_text;
    Scoreboard scores;
    LaserManager laser_manager;
    PlanesNetworked network;
    ScoreManager score_manager;

    uint16_t ucid_from_server = 0;
    uint16_t my_score = 0;

    vec3 my_allocated_color_from_server = vec3(0, 0, 0);

    bool shoot;
    bool external_key_to_die;

    bool is_dead = false;
    vec3 deadpos = vec3(0, 0, 0);
    float dietime = 0;


    double get_last_elapsed_time()
    {
        static double lasttime = glfwGetTime();
        double actualtime = glfwGetTime();
        double difference = actualtime - lasttime;
        lasttime = actualtime;
        return difference;
    }

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (key == GLFW_KEY_W && action == GLFW_PRESS) theplayer.w = true;
        if (key == GLFW_KEY_W && action == GLFW_RELEASE) theplayer.w = false;

        if (key == GLFW_KEY_S && action == GLFW_PRESS) theplayer.s = true;
        if (key == GLFW_KEY_S && action == GLFW_RELEASE) theplayer.s = false;

        if (key == GLFW_KEY_A && action == GLFW_PRESS) theplayer.a = true;
        if (key == GLFW_KEY_A && action == GLFW_RELEASE) theplayer.a = false;

        if (key == GLFW_KEY_D && action == GLFW_PRESS) theplayer.d = true;
        if (key == GLFW_KEY_D && action == GLFW_RELEASE) theplayer.d = false;

        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) shoot = true;
        if (key == GLFW_KEY_K && action == GLFW_PRESS) external_key_to_die = true;

        if (key == GLFW_KEY_F && action == GLFW_RELEASE)
        {
            if (renderstate == 1)
                renderstate = 2;
            else
                renderstate = 1;
        }
    }

    // callback for the mouse when clicked move the triangle when helper functions
    // written
    void mouseCallback(GLFWwindow* window, int button, int action, int mods)
    {
        double posX, posY;
        float newPt[2];
        if (action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &posX, &posY);
            std::cout << "Pos X " << posX << " Pos Y " << posY << std::endl;

            // change this to be the points converted to WORLD
            // THIS IS BROKEN< YOU GET TO FIX IT - yay!
            newPt[0] = 0;
            newPt[1] = 0;

            std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
            // update the vertex array with the updated points
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 6, sizeof(float) * 2, newPt);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    // if the window is resized, capture the new size and reset the viewport
    void resizeCallback(GLFWwindow* window, int in_width, int in_height)
    {
        // get the window size - may be different then pixels for retina
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
    }

    void init_mesh()
    {
        // generate the VAO
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // generate vertex buffer to hand off to OGL
        glGenBuffers(1, &MeshPosID);
        glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
        glm::vec3* vertices = new glm::vec3[MESHSIZE * MESHSIZE * 6];
        for (int x = 0; x < MESHSIZE; x++)
        {
            for (int z = 0; z < MESHSIZE; z++)
            {
                vertices[x * 6 + z * MESHSIZE * 6 + 0] = vec3(0.0, 0.0, 0.0) + vec3(x, 0, z);  // LD
                vertices[x * 6 + z * MESHSIZE * 6 + 1] = vec3(1.0, 0.0, 0.0) + vec3(x, 0, z);  // RD
                vertices[x * 6 + z * MESHSIZE * 6 + 2] = vec3(1.0, 0.0, 1.0) + vec3(x, 0, z);  // RU
                vertices[x * 6 + z * MESHSIZE * 6 + 3] = vec3(0.0, 0.0, 0.0) + vec3(x, 0, z);  // LD
                vertices[x * 6 + z * MESHSIZE * 6 + 4] = vec3(1.0, 0.0, 1.0) + vec3(x, 0, z);  // RU
                vertices[x * 6 + z * MESHSIZE * 6 + 5] = vec3(0.0, 0.0, 1.0) + vec3(x, 0, z);  // LU
            }
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * MESHSIZE * MESHSIZE * 6, vertices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        delete[] vertices;
        // tex coords
        float t = 1. / MESHSIZE;
        vec2* tex = new vec2[MESHSIZE * MESHSIZE * 6];
        for (int x = 0; x < MESHSIZE; x++)
            for (int y = 0; y < MESHSIZE; y++)
            {
                tex[x * 6 + y * MESHSIZE * 6 + 0] = vec2(0.0, 0.0) + vec2(x, y) * t;  // LD
                tex[x * 6 + y * MESHSIZE * 6 + 1] = vec2(t, 0.0) + vec2(x, y) * t;    // RD
                tex[x * 6 + y * MESHSIZE * 6 + 2] = vec2(t, t) + vec2(x, y) * t;      // RU
                tex[x * 6 + y * MESHSIZE * 6 + 3] = vec2(0.0, 0.0) + vec2(x, y) * t;  // LD
                tex[x * 6 + y * MESHSIZE * 6 + 4] = vec2(t, t) + vec2(x, y) * t;      // RU
                tex[x * 6 + y * MESHSIZE * 6 + 5] = vec2(0.0, t) + vec2(x, y) * t;    // LU
            }
        glGenBuffers(1, &MeshTexID);
        // set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, MeshTexID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * MESHSIZE * MESHSIZE * 6, tex, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        delete[] tex;
        glGenBuffers(1, &IndexBufferIDBox);
        // set the current state to focus on our vertex buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
        GLuint* elements = new GLuint[MESHSIZE * MESHSIZE * 8];
        //	GLuint ele[10000];
        int ind = 0, i = 0;
        for (i = 0; i < (MESHSIZE * MESHSIZE * 8); i += 8, ind += 6)
        {
            elements[i + 0] = ind + 0;
            elements[i + 1] = ind + 1;
            elements[i + 2] = ind + 1;
            elements[i + 3] = ind + 2;
            elements[i + 4] = ind + 2;
            elements[i + 5] = ind + 5;
            elements[i + 6] = ind + 5;
            elements[i + 7] = ind + 0;
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * MESHSIZE * MESHSIZE * 8, elements, GL_STATIC_DRAW);
        delete[] elements;
        glBindVertexArray(0);
    }
    /*Note that any gl calls must always happen after a GL state is initialized */
    void initGeom()
    {
        string resourceDirectory = "../resources";
        // initialize the net mesh
        init_mesh();

        laser.initGeom(resourceDirectory);
        explosion.initGeom(resourceDirectory);
        custom_text.initGeom(resourceDirectory);
        scores.initGeom(resourceDirectory);

        // Initialize mesh.
        shape = make_shared<Shape>();
        // shape->loadMesh(resourceDirectory + "/t800.obj");
        shape->loadMesh(resourceDirectory + "/sphere.obj");
        shape->resize();
        shape->init();

        plane_renderer.initGeom(resourceDirectory, stbi_load);

        int width, height, channels;
        char filepath[1000];

        // texture 1
        string str = resourceDirectory + "/sky.jpg";
        strcpy(filepath, str.c_str());
        unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        // texture 2
        str = resourceDirectory + "/sky1.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Texture2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // texture 3
        str = resourceDirectory + "/height.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &HeightTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, HeightTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //[TWOTEXTURES]
        // set the 2 textures to the correct samplers in the fragment shader:
        GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");  // tex, tex2... sampler in the fragment shader
        GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(prog->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);

        Tex1Location = glGetUniformLocation(heightshader->pid, "tex");  // tex, tex2... sampler in the fragment shader
        Tex2Location = glGetUniformLocation(heightshader->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(heightshader->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);

        Tex1Location = glGetUniformLocation(skyprog->pid, "tex");  // tex, tex2... sampler in the fragment shader
        Tex2Location = glGetUniformLocation(skyprog->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(skyprog->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);

        Tex1Location = glGetUniformLocation(linesshader->pid, "tex");  // tex, tex2... sampler in the fragment shader
        Tex2Location = glGetUniformLocation(linesshader->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(linesshader->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);

        // dynamic audio texture

        /*
                        int datasize = TEXSIZE *TEXSIZE * 4 * sizeof(GLfloat);
                        glGenBuffers(1, &AudioTexBuf);
                        glBindBuffer(GL_TEXTURE_BUFFER, AudioTexBuf);
                        glBufferData(GL_TEXTURE_BUFFER, datasize, NULL, GL_DYNAMIC_COPY);
                        glBindBuffer(GL_TEXTURE_BUFFER, 0);

                        glGenTextures(1, &AudioTex);
                        glBindTexture(GL_TEXTURE_BUFFER, AudioTex);
                        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, AudioTexBuf);
                        glBindTexture(GL_TEXTURE_BUFFER, 0);
                        //glBindImageTexture(2, AudioTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
                        */

        laser.initTexture(resourceDirectory, "laser.png", stbi_load);
        explosion.initTexture(resourceDirectory, stbi_load);
        custom_text.initTexture(resourceDirectory, stbi_load);
        scores.initTexture(resourceDirectory, stbi_load);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // TODO TODO
        // setup_players();
    }

    // General OGL initialization - set OGL state here
    void init(const std::string& resourceDirectory)
    {
        GLSL::checkVersion();

        // Set background color.
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        // Initialize the GLSL program.
        skyprog = std::make_shared<Program>();
        skyprog->setVerbose(true);
        skyprog->setShaderNames(resourceDirectory + "/sky_vertex.glsl", resourceDirectory + "/sky_fragment.glsl");
        if (!skyprog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        skyprog->addUniform("P");
        skyprog->addUniform("V");
        skyprog->addUniform("M");
        skyprog->addAttribute("vertPos");
        skyprog->addAttribute("vertTex");

        // Initialize the GLSL program.
        prog = std::make_shared<Program>();
        prog->setVerbose(true);
        prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
        if (!prog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog->addUniform("P");
        prog->addUniform("V");
        prog->addUniform("M");
        prog->addUniform("campos");
        prog->addAttribute("vertPos");
        prog->addAttribute("vertNor");
        prog->addAttribute("vertTex");

        // Initialize the GLSL program.
        heightshader = std::make_shared<Program>();
        heightshader->setVerbose(true);
        heightshader->setShaderNames(resourceDirectory + "/height_vertex.glsl",
                                     resourceDirectory + "/height_frag.glsl",
                                     resourceDirectory + "/geometry.glsl");
        if (!heightshader->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        heightshader->addUniform("P");
        heightshader->addUniform("V");
        heightshader->addUniform("M");
        heightshader->addUniform("camoff");
        heightshader->addUniform("campos");
        heightshader->addAttribute("vertPos");
        heightshader->addAttribute("vertTex");
        heightshader->addUniform("bgcolor");
        heightshader->addUniform("renderstate");

        // Initialize the GLSL program.
        linesshader = std::make_shared<Program>();
        linesshader->setVerbose(true);
        linesshader->setShaderNames(resourceDirectory + "/lines_height_vertex.glsl",
                                    resourceDirectory + "/lines_height_frag.glsl",
                                    resourceDirectory + "/lines_geometry.glsl");
        if (!linesshader->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        linesshader->addUniform("P");
        linesshader->addUniform("V");
        linesshader->addUniform("M");
        linesshader->addUniform("camoff");
        linesshader->addUniform("campos");
        linesshader->addAttribute("vertPos");
        linesshader->addAttribute("vertTex");
        linesshader->addUniform("bgcolor");

        plane_renderer.initProgram(resourceDirectory);
        explosion.initProgram(resourceDirectory);
        laser.initProgram(resourceDirectory, "laser_vertex.glsl", "laser_fragment.glsl");
        custom_text.initProgram(resourceDirectory);
        scores.initProgram(resourceDirectory);
    }
    /****DRAW
    This is the most important function in your program - this is where you
    will actually issue the commands to draw any geometry you have set up to
    draw
    ********/
    void render(GLFWwindow* window)
    {
        static double count = 0;
        double frametime = get_last_elapsed_time();
        count += frametime;

        double posX, posY;
        glfwGetCursorPos(window, &posX, &posY);
        double xdiff = posX - SS_CENTER_USED_FOR_MOUSE_CONTROLS.x;
        double ydiff = posY - SS_CENTER_USED_FOR_MOUSE_CONTROLS.y;

        double xpercent = xdiff / (1920.0f / 2);
        double ypercent = ydiff / (1080.0f / 2);

        xpercent *= 0.025;
        ypercent *= 0.020;

        theplayer.update(frametime, ypercent, xpercent);
        // for now

        vector<vec3> positions;

        // TODO TODO
        // for (int i = 0; i < thebots.size(); i++)
        // {
        //     positions.push_back(thebots[i].pos);
        // }
        // positions.push_back(theplayer.pos);

        // for (int i = 0; i < thebots.size(); i++)
        // {
        //     float min_distance = length(positions[i] - positions[i + 1]);
        //     int min_index = i + 1;
        //     for (int j = 0; j < positions.size(); j++)
        //     {
        //         if (j != i)
        //         {
        //             float distance = length(positions[i] - positions[j]);
        //             if (distance < min_distance)
        //             {
        //                 min_index = j;
        //                 min_distance = distance;
        //             }
        //         }
        //     }

        //     thebots[i].update(frametime, positions[min_index]);
        //     // thebots[i].update(frametime, theplayer.pos);
        // }

        // get max score

        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        float aspect = width / (float)height;
        glViewport(0, 0, width, height);

        // Clear framebuffer.
        glClearColor(0.8f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float default_rotation_angle = -1.0 * PI / 2.0;
        mat4 rotate_default_plane =  // rotate(mat4(1), default_rotation_angle, vec3(0,1,0)) *
            rotate(mat4(1), default_rotation_angle, vec3(1, 0, 0));

        glm::mat4 V, Vi, M, P;  // View, Model and Perspective matrix
        vec3 campos = theplayer.pos + -60.0f * theplayer.forward + 5.0f * theplayer.up;

        V = mycam.process(frametime, -1.0f * campos, theplayer.forward, theplayer.up, theplayer.right);

        Vi = glm::inverse(V);
        M = glm::mat4(1);
        // Apply orthographic projection....
        P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 10000.0f);
        if (width < height) P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, -2.0f, 10000.0f);
        // ...but we overwrite it (optional) with a perspective projection.
        P = glm::perspective((float)(3.14159 / 4.),
                             (float)((float)width / (float)height),
                             0.01f,
                             100000.0f);  // so much type casting... GLM metods are quite funny ones

        // Draw the skybox --------------------------------------------------------------
        skyprog->bind();
        static float w = 0.0;
        w += 1.0 * frametime;  // rotation angle
        float trans = 0;       // sin(t) * 2;
        w = 0.6;
        glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
        float angle = 3.1415926 / 2.0;
        glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), -mycam.pos);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));
        M = TransZ * RotateY * RotateX * S;
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glActiveTexture(GL_TEXTURE0);
        if (renderstate == 1)
            glBindTexture(GL_TEXTURE_2D, Texture);
        else if (renderstate == 2)
            glBindTexture(GL_TEXTURE_2D, Texture2);
        glDisable(GL_DEPTH_TEST);
        shape->draw(prog);
        glEnable(GL_DEPTH_TEST);

        mat4 rotate_plane = safe_lookat(theplayer.pos, theplayer.pos + theplayer.forward, theplayer.up);
        mat4 plane_overall_rot = rotate_plane * rotate_default_plane;

        quat q = quat(plane_overall_rot);
        network.BroadcastSelfPosition(glfwGetTime(),
                                      theplayer.pos,
                                      theplayer.velocity_cached,
                                      q,
                                      my_allocated_color_from_server,
                                      is_dead,
                                      my_score);
        score_manager.admitScore(my_username, my_score);
        network.PollIncoming(glfwGetTime());
        my_score += network.PopUnclaimedKillCount();

        // Special scope for netlaser
        {
            NewShotLaserInfo netlaser;
            while (network.MaybePopIncomingNetworkedLaser(&netlaser))
            {
                // TODO THIS IS silly
                netlaser.start_time = glfwGetTime();
                laser_manager.admitLaser(&netlaser);
            }
        }

        // Draw the terrain --------------------------------------------------------------
        heightshader->bind();
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 TransY = glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f, -9.0f, -500));
        M = TransY;
        glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniformMatrix4fv(heightshader->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(heightshader->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        vec3 offset = mycam.pos;
        offset.y = 0;
        offset.x = (int)offset.x;
        offset.z = (int)offset.z;
        //	offset = vec3(0, 0, 0);
        vec3 bg = vec3(254. / 255., 225. / 255., 168. / 255.);
        if (renderstate == 2) bg = vec3(49. / 255., 88. / 255., 114. / 255.);
        glUniform3fv(heightshader->getUniform("camoff"), 1, &offset[0]);
        glUniform3fv(heightshader->getUniform("campos"), 1, &mycam.pos[0]);
        glUniform3fv(heightshader->getUniform("bgcolor"), 1, &bg[0]);
        glUniform1i(heightshader->getUniform("renderstate"), renderstate);
        glBindVertexArray(VertexArrayID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, HeightTex);
        glDrawArrays(GL_TRIANGLES, 0, MESHSIZE * MESHSIZE * 6);
        heightshader->unbind();

        plane_renderer.renderAirplane(P,
                                      V,
                                      theplayer.pos,
                                      plane_overall_rot,
                                      mycam.pos,
                                      my_allocated_color_from_server,
                                      my_username,
                                      &custom_text,
                                      is_dead);

        if (is_dead)
        {
            theplayer.speed = 0;
            theplayer.pos = deadpos;
            explosion.renderExplosion(P, V, mycam.pos, deadpos, glfwGetTime());
        }

        // Render your own plane

        // Renders the other real human players
        const auto estimates = network.GiveOtherPlaneEstimates(glfwGetTime());
        for (const auto& estimate : estimates)
        {
            plane_renderer.renderAirplane(P,
                                          V,
                                          estimate.pos,
                                          estimate.rot,
                                          mycam.pos,
                                          estimate.color,
                                          estimate.username,
                                          &custom_text,
                                          estimate.is_dead);
            if (estimate.is_dead)
            {
                explosion.renderExplosion(P, V, mycam.pos, estimate.pos, glfwGetTime());
            }

            score_manager.admitScore(estimate.username, estimate.score);
        }

        auto ssss = score_manager.supplyScoreboardTexts();
        int num = ssss.size() - 1;
        for (auto str : ssss)
        {
            scores.renderCustomText(num, vec3(0.6, 0.5, 0.7), str);
            num -= 1;
        }

        // draw the bots

        // TODO TODO
        // for (int i = 0; i < thebots.size(); i++)
        // {
        //     // scale_plane = scale(mat4(1), vec3(10));
        //     rotate_plane = safe_lookat(thebots[i].pos, thebots[i].pos + thebots[i].forward, thebots[i].up);
        //     translate_plane = translate(mat4(1), thebots[i].pos);
        //     plane_overall_rot = rotate_plane * rotate_default_plane;
        //     M = translate_plane * plane_overall_rot * scale_plane;

        //     glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        //     glUniform3fv(pplane->getUniform("tint_color"), 1, &my_allocated_color_from_server[0]);
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_2D, Texture2);
        //     plane->draw(pplane);  // render!!!!!!
        // }

        if (shoot && !is_dead)
        {
            NewShotLaserInfo newlaser;
            vec3 rightvec = normalize(theplayer.right) * 6.0f;
            vec3 othervec = theplayer.pos + theplayer.up * -2.0f + theplayer.forward * -2.0f;
            newlaser.position_source = vec4(rightvec + othervec, 0);
            newlaser.position_target =
                vec4(vec3(((normalize(theplayer.forward) * 1000.0f) + trunc_v4(newlaser.position_source))), 0);
            newlaser.start_time = glfwGetTime();
            newlaser.speed = 0.25;
            newlaser.color = vec4(my_allocated_color_from_server, 0);
            newlaser.ucid_shooter_check_endianness = htons(ucid_from_server);
            laser_manager.admitLaser(&newlaser);
            network.BroadcastNewLaser(&newlaser);
            newlaser.position_source = vec4(-rightvec + othervec, 0);
            newlaser.position_target =
                vec4(((normalize(theplayer.forward) * 1000.0f) + trunc_v4(newlaser.position_source)), 0);
            laser_manager.admitLaser(&newlaser);
            network.BroadcastNewLaser(&newlaser);
            shoot = false;
        }

        laser_manager.renderLasers(P, V, campos, glfwGetTime(), &laser);

        uint16_t ucid_of_my_killer = 0;
        bool otherplayerkill =
            laser_manager.shouldDie(theplayer.pos, ucid_from_server, glfwGetTime(), &ucid_of_my_killer);
        if (!is_dead && (otherplayerkill || external_key_to_die))
        {
            is_dead = true;
            deadpos = theplayer.pos;
            dietime = glfwGetTime();
            external_key_to_die = false;
            if (otherplayerkill)
            {
                network.BroadcastKillConfirmation(ucid_of_my_killer);
            }

            if(my_score != 0)
        { 
              my_score -= 1;
        }
        }
        if (is_dead && glfwGetTime() - dietime > 10)
        {
            theplayer = player();
            is_dead = false;
            deadpos = vec3(0);
            dietime = 0;
        }
    }
};

#ifdef _MSC_VER
#include <Lmcons.h>
#include <windows.h>
#endif

//******************************************************************************************
int main(int argc, char** argv)
{
    std::string resourceDir = "../resources";  // Where the resources are loaded from
    const char* my_username = USERNAME;

    const char* hostname = (const char*)"174.87.85.45";

    srand((int) my_username[0]);

    if (argc >= 2)
    {
        resourceDir = argv[1];
    }
    if (argc >= 3)
    {
        my_username = argv[2];
    }
    if (argc >= 4)
    {
        hostname = argv[3];
    }

    Application* application = new Application();

    if (strcmp(my_username, "INVALID") == 0)
    {
        log_fatal("Please choose username. Usage: ./program1 ../resources YOUR_NAME_HERE");
        return 96;
    }

    uint16_t ucid_from_server;
    uint32_t timediff_unused;

    application->my_username = my_username;
    log_info("My username is %s", application->my_username.c_str());
    application->network.PlanesNetworkedSetup(my_username, hostname, &ucid_from_server, &timediff_unused);
    application->ucid_from_server = ucid_from_server;

    application->my_allocated_color_from_server = global_plane_color_allocation[ucid_from_server % GLOBAL_COLOR_COUNT];
    log_info("My UCID is %d", ucid_from_server);
    log_info("My allocated color is RGB %f %f %f",
             application->my_allocated_color_from_server.r,
             application->my_allocated_color_from_server.g,
             application->my_allocated_color_from_server.b);

    /* your main will always include a similar set up to establish your window
            and GL context, etc. */
    WindowManager* windowManager = new WindowManager();
    windowManager->init(1920, 1080);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;

    /* This is the code that will likely change program to program as you
            may need to initialize or set up different data and state */
    // Initialize scene.
    application->init(resourceDir);
    application->initGeom();

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(windowManager->getHandle()))
    {
        application->score_manager.maybeUpdateScoreboard(glfwGetTime());

        // Render scene.
        application->render(windowManager->getHandle());

        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }

    // Quit program.
    windowManager->shutdown();
    return 0;
}
