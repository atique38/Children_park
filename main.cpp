#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "basic_camera.h"
#include "camera.h"
#include "pointLight.h"
#include "stb_image.h"
#include "cube.h"
#include "SpotLight.h"


#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void lightEffect(unsigned int VAO, Shader lightShader, glm::mat4 model, glm::vec3 color, unsigned int size);
void drawCube(unsigned int VAO, Shader shader, glm::mat4 model, glm::vec4 color);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L);
void read_file(string file_name, vector<float>& vec);
unsigned int hollowBezier(GLfloat ctrlpoints[], int L, vector<float>& coordinates, vector<float>& normals, vector<int>& indices, vector<float>& vertices, float div);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
long long nCr(int n, int r);
void load_texture(unsigned int& texture, string image_name, GLenum format);
unsigned int f(int& size);
void FerrisWheel(Shader ourShader, glm::mat4 moveMatrix, unsigned int cubeVAO);
void FerrisWheelSeat(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z, unsigned int cubeVAO);
void swing(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader);
void textureEffect(unsigned int VAO, Shader shader, glm::mat4 model, unsigned int indicesSize, unsigned int texture0);
unsigned int triangle();
void seesaw(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, unsigned int triVAO, Shader texureShader);
void slider(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader);
void alphabet(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader);
unsigned int  generateSphere(float radius, int sectorCount, int stackCount,unsigned int & sphereIndicesSize);
void roundTable(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader);
void chair(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader);
void environment(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader, Shader ourshader);
void bench(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader);
void gate(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader);
unsigned int  generateCone(float radius, float height, int segments, std::vector<float>& vertices, std::vector<unsigned int>& indices);
void generateTree(std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec3 start, glm::vec3 direction, float length, int depth, int maxDepth);
unsigned int treeSetUp();
void drawTree(unsigned int VAO, Shader lightShader, glm::mat4 model, glm::vec3 color);
void rotor(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader);
// draw object functions
//void drawCube(Shader shaderProgram, unsigned int VAO, glm::mat4 parentTrans, float posX = 0.0, float posY = 0.0, float posz = 0.0, float rotX = 0.0, float rotY = 0.0, float rotZ = 0.0, float scX = 1.0, float scY = 1.0, float scZ = 1.0);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// modelling transform
float rotateAngle_X = 45.0;
float rotateAngle_Y = 45.0;
float rotateAngle_Z = 45.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;

//// camera
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// camera
Camera camera(glm::vec3(13.0f, 3.0f, 25.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

float eyeX = 1.35, eyeY = 4.8, eyeZ = 10.0;
float lookAtX = 4.0, lookAtY = 4.0, lookAtZ = 6.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

bool on = false;

//birds eye
bool birdEye = false;
glm::vec3 cameraPos(-2.0f, 5.0f, 13.0f);
glm::vec3 target(-2.0f, 0.0f, 5.5f);
float birdEyeSpeed = 1.0f;


// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(10.0f,  3.5f,  -1.5f),
    glm::vec3(10.0f,  3.5f,  -7.5f),
    glm::vec3(19.0f,  3.5f,  -1.5f),
    glm::vec3(19.0f,  3.5f,  -7.5f),
    glm::vec3(2.5f, 3.5f, -15.0f)

};

PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    0.5f, 0.5f, 0.5f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);
PointLight pointlight2(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    0.5f, 0.5f, 0.5f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2       // light number
);

PointLight pointlight3(

    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    0.5f, 0.5f, 0.5f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    3       // light number
);
PointLight pointlight4(

    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    0.5f, 0.5f, 0.5f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    4       // light number
);

PointLight pointlight5(

    pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.94f, 0.89f, 0.64f,     // diffuse //0.125f, 0.76f, 0.05f
    0.94f, 0.89f, 0.64,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    5       // light number
);

SpotLight spotlight1(
    15.0, 10.0, -25.0,  // position
    0.333f, 1.0f, 1.0f,     // ambient
    0.333f, 1.0f, 1.0f,      // diffuse
    0.333f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1,       // light number
    glm::cos(glm::radians(20.5f)),
    glm::cos(glm::radians(25.0f)),
    0, -1, 0
);

SpotLight spotlight2(
    7.0, 4.0, -20.0,  // position
    1.0f, 1.0f, 1.0f,     // ambient
    1.0f, 1.0f, 1.0f,      // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2,       // light number
    glm::cos(glm::radians(20.5f)),
    glm::cos(glm::radians(25.0f)),
    1, -1, -2
);

SpotLight spotlight3(
    20.0, 4.0, -20.0,  // position
    1.0f, 1.0f, 1.0f,     // ambient
    1.0f, 1.0f, 1.0f,      // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    3,       // light number
    glm::cos(glm::radians(20.5f)),
    glm::cos(glm::radians(25.0f)),
    -1, -1, -2.5
);
SpotLight spotlight4(
    10.0, 4.0, -20.0,  // position
    1.0f, 1.0f, 1.0f,     // ambient
    1.0f, 1.0f, 1.0f,      // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    3,       // light number
    glm::cos(glm::radians(20.5f)),
    glm::cos(glm::radians(25.0f)),
    0, -1, -1
);

// light settings
bool onOffToggle = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;
bool dl = true;
bool spt = true;
bool point5 = true;
bool point2 = true;
bool point3 = true;
bool spot1 = true;
bool spot2 = true;
bool spot3 = true;
bool pointLightOn = true;



//float d_amb_on = 1.0f;
//float d_def_on = 1.0f;
//float d_spec_on = 1.0f;

glm::vec3 amb(0.2f, 0.2f, 0.2f);
glm::vec3 def(0.8f, 0.8f, 0.8f);
glm::vec3 spec(1.0f, 1.0f, 1.0f);

float fov = glm::radians(camera.Zoom);
float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
float near = 0.1f;
float far = 100.0f;

float tanHalfFOV = tan(fov / 2.0f);

const double pi = 3.14159265389;
const int nt = 40;
const int ntheta = 30;
unsigned int ferriVAO, ropeVAO, rotorVAO, sliderVAO, carousalVAO, headVAO, hillVAO,pillarVAO,legVAO,sphereVAO,coneVAO,treeVAO;
unsigned int texture, texture2, texture3, texture4, texture5, texture6, textureWall, textureFlower, textureSeat, textureWel;

vector <float> cntrlPoints, cntrlPointsRope, cntrlPointsRope2, cntrlPointsRotor, cntrlPointsCylinder, cntrlPointsCarousal, cntrlPointsHead, cntrlPointsPilar, cntrlPointsFerri, cntrlPointsLeg;
vector <float> coordinates, coordinatesRope, coordinatesRope2,coordinatesRotor, coordinatesCylinder, coordinatesCarousal, coordinatesHead, coordinatesPillar, coordinatesFerri, coordinatesLeg;
vector <float> normals, normalsRope, normalsRope2, normalsRotor, normalsCylinder, normalsCarousal, normalsHead, normalsPillar, normalsFerri, normalsLeg;
vector <int> indices, indicesRope, indicesRope2,indicesRotor, indicesCylinder, indicesCarousal, indicesHead, indicesPillar, indicesFerri, indicesLeg;
vector <float> vertices, verticesRope, verticesRotor, verticesRope2, verticesCylinder, verticesCarousal, verticesHead, verticesPillar, verticesFerri, verticesLeg;
vector <float> coneVertices,treeVertices;
vector <unsigned int> coneIndices,treeIndices;
unsigned int sphereIndicesSize;
// texture
float extra = 4.0f;
float TXmin = 0.0f;
float TXmax = 1.0f ;
float TYmin = 0.0f;
float TYmax = 1.0f ;

//ferris
float ferrisWheelAngle = 0.0f;
float ferrisWheelSpeed = 0.0f;
float ferrisIncrement = 0.1f;
bool isFerrisWheelOn = false;

float swingAngle = 0.0f;
float swingSpeed = 0.5f;
bool isswingOn = false;
float swingMaxAngle = 60.0f;
float lastTime = 0.0f;

//seesaw
float seesawAngle = 20.0f;
float seesawSpeed = 0.3f;
bool isseesawOn = false;
float seesawMaxAngle = 20.0f;
float lastTimess = 0.0f;

//gate
float gateAngle = 0.0f;
float isGateOpening = -1.0f;
float gateSpeed = 0.2f;

//rotor
bool isRotorOn = false;
float rotorSpeed = 0.02f;
float rotorAngle = 0.0f;
float rotorIncrement = 0.1f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    //Shader constantShader("vertexShader.vs", "fragmentShaderV2.fs");

    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    Shader textureShader("texture_vertex.vs", "texture_fragment.fs");
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    /*float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };*/
    /*unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };*/

    float cube_vertices[] = {
        // positions      // normals         // texture coords
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmin,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmin,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmax,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmax,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmin,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmax,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, TXmin, TYmin,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, TXmin, TYmax,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, TXmin, TYmin,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, TXmax, TYmin,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, TXmax, TYmax,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, TXmin, TYmax,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, TXmax, TYmin,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, TXmax, TYmax,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmax,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmin,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, TXmax, TYmin,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, TXmax, TYmax,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, TXmin, TYmax,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, TXmin, TYmin,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmin, TYmin,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmax, TYmin,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, TXmin, TYmax
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
    glEnableVertexAttribArray(2);

    //light's VAO
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int triangleVAO = triangle();
    sphereVAO = generateSphere(1.0, 30, 30,sphereIndicesSize);
    coneVAO = generateCone(1.0, 1.0, 30, coneVertices, coneIndices);
    generateTree(treeVertices, treeIndices, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 1.0, 5, 5);
    treeVAO = treeSetUp();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //ourShader.use();
    //constantShader.use();

    
    
    read_file("hill.txt", cntrlPoints);
    hillVAO = hollowBezier(cntrlPoints.data(), ((unsigned int)cntrlPoints.size() / 3) - 1, coordinates, normals, indices, vertices,1.0);

    read_file("rope_points.txt", cntrlPointsRope);
    ropeVAO = hollowBezier(cntrlPointsRope.data(), ((unsigned int)cntrlPointsRope.size() / 3) - 1, coordinatesRope, normalsRope, indicesRope, verticesRope,1.0);
    
    read_file("slider_points.txt", cntrlPointsCylinder);
    sliderVAO = hollowBezier(cntrlPointsCylinder.data(), ((unsigned int)cntrlPointsCylinder.size() / 3) - 1, coordinatesCylinder, normalsCylinder, indicesCylinder, verticesCylinder,2.0);

    read_file("pillar_points.txt", cntrlPointsPilar);
    pillarVAO = hollowBezier(cntrlPointsPilar.data(), ((unsigned int)cntrlPointsPilar.size() / 3) - 1, coordinatesPillar, normalsPillar, indicesPillar, verticesPillar, 1.0);

    read_file("ferri_points.txt", cntrlPointsFerri);
    ferriVAO = hollowBezier(cntrlPointsFerri.data(), ((unsigned int)cntrlPointsFerri.size() / 3) - 1, coordinatesFerri, normalsFerri, indicesFerri, verticesFerri,1.0);

    read_file("seesawleg.txt", cntrlPointsLeg);
    legVAO = hollowBezier(cntrlPointsLeg.data(), ((unsigned int)cntrlPointsLeg.size() / 3) - 1, coordinatesLeg, normalsLeg, indicesLeg, verticesLeg, 1.0);
    // Texture loading

    //load_texture(texture1, "grass.jpg", GL_RGBA);
    
    
    string ImgPath = "grass.jpg";
    texture = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    unsigned int diffFloor = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specFloor = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube2 = Cube(diffFloor, specFloor, 32.0f, 0.0f, 0.0f, 10.0f, 20.0f);

    ImgPath = "rope.jpg";
    texture2 = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    

    ImgPath = "wood.jpg";
    texture3 = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    //diffFloor = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    //specFloor = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    

    ImgPath = "mashroom2.jpg";
    texture4 = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    ImgPath = "mushroom3.jpg";
    texture5 = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    ImgPath = "road.jpg";
    unsigned int diffMap = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 1.0f, 10.0f);
    Cube cube3 = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 10.0f, 1.0f);
    ImgPath = "brickwall.jpg";
    textureWall= loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cubeWall = Cube(textureWall, textureWall, 32.0f, 0.0f, 0.0f, 10.0f, 1.0f);

    ImgPath = "flower2.jpg";
    textureFlower = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    ImgPath = "seat.jpeg";
    textureSeat = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    ImgPath = "welcome.jpg";
    textureWel = loadTexture(ImgPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    
   int findicesSize;

    //Letter F
    unsigned int FVao = f(findicesSize);

    float r = 0.0f;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       
        // pass projection matrix to shader (note that in this case it could change every frame)
        //glm::mat4 projection = glm::perspective(glm::radians(basic_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        //ourShader.setMat4("projection", projection);
        //constantShader.setMat4("projection", projection);

        //glm::mat4 projection(0.0f); // Initialize with zero matrix

        //projection[0][0] = 1.0f / (aspect * tanHalfFOV);
        //projection[1][1] = 1.0f / tanHalfFOV;
        //projection[2][2] = -(far + near) / (far - near);
        //projection[2][3] = -1.0f;
        //projection[3][2] = -(2.0f * far * near) / (far - near);

        //lightingShader.setMat4("projection", projection);

        // camera/view transformation

        glm::mat4 view;

        if (birdEye) {
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(cameraPos, target, up);
        }
        else {
            //view = basic_camera.createViewMatrix();
            view = camera.GetViewMatrix();
        
        }

        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, modelCentered,
            translateMatrixprev;
       
        
        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setVec3("viewPos", camera.Position);
        lightingShaderWithTexture.setMat4("projection", projection);
        lightingShaderWithTexture.setMat4("view", view);

        // point light 1
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        // point light 2
        pointlight2.setUpPointLight(lightingShaderWithTexture);
        // point light 3
        pointlight3.setUpPointLight(lightingShaderWithTexture);
        pointlight4.setUpPointLight(lightingShaderWithTexture);
        pointlight5.setUpPointLight(lightingShaderWithTexture);

        spotlight1.setUpspotLight(lightingShaderWithTexture);
        spotlight2.setUpspotLight(lightingShaderWithTexture);
        spotlight3.setUpspotLight(lightingShaderWithTexture);

        lightingShaderWithTexture.setVec3("directionalLight.directiaon", 0.0f, -3.0f, 0.0f);
        lightingShaderWithTexture.setVec3("directionalLight.ambient", .5f, .5f, .5f);
        lightingShaderWithTexture.setVec3("directionalLight.diffuse", .8f, .8f, .8f);
        lightingShaderWithTexture.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);

        lightingShaderWithTexture.setBool("directionLightOn", dl);




        //hill
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0, 0.0, -25.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0, 3.0, 2.0));
        model = translateMatrix * scaleMatrix;
        textureEffect(hillVAO, lightingShaderWithTexture, model, indices.size(), texture);
        
        //grass floor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0, 0.0, 10.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(35.0, -0.1, -45.0));
        model = translateMatrix * scaleMatrix;
        cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //road left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0, 0.0, 5.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 0.05, -20.0));
        model = translateMatrix * scaleMatrix;
        cube.drawCubeWithTexture(lightingShaderWithTexture, model);

        //road front
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0, 0.0, 5.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(13.0, 0.05, 1.0));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //road back
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0, 0.0, -16.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(13.0, 0.05, 1.0));
        model = translateMatrix * scaleMatrix;
        cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

        //road right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(20.0, 0.0, 5.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 0.05, -20.0));
        model = translateMatrix * scaleMatrix;
        cube.drawCubeWithTexture(lightingShaderWithTexture, model);

        //wall left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0, 0.0, 10.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2, 4.0, -45.0));
        model = translateMatrix * scaleMatrix;
        cubeWall.drawCubeWithTexture(lightingShaderWithTexture, model);

        //wall right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(30.8, 0.0, 10.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2, 4.0, -45.0));
        model = translateMatrix * scaleMatrix;
        cubeWall.drawCubeWithTexture(lightingShaderWithTexture, model);
        

        //wall front left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.8, 0.0, 10.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(14.0, 4.0, 0.2));
        model = translateMatrix * scaleMatrix;
        Cube cubeWall2 = Cube(textureWall, textureWall, 32.0f, 0.0f, 0.0f, 5.0f, 1.0f);
        cubeWall2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //wall front right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(17.0, 0.0, 10.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(14.0, 4.0, 0.2));
        model = translateMatrix * scaleMatrix;
        cubeWall2.drawCubeWithTexture(lightingShaderWithTexture, model);

        //wall back
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.8, 0.0, -35.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(34.6, 4.0, 0.2));
        model = translateMatrix * scaleMatrix;
        cubeWall2.drawCubeWithTexture(lightingShaderWithTexture, model);
        
        //glBindVertexArray(cubeVAO);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);


        // point light 1
        pointlight1.setUpPointLight(lightingShader);
        // point light 2
        pointlight2.setUpPointLight(lightingShader);
        // point light 3
        pointlight3.setUpPointLight(lightingShader);
        pointlight4.setUpPointLight(lightingShader);
        pointlight5.setUpPointLight(lightingShader);

        spotlight1.setUpspotLight(lightingShader);
        spotlight2.setUpspotLight(lightingShader);
        spotlight3.setUpspotLight(lightingShader);
        
        //constantShader.setMat4("view", view);
        //lightingShader.setMat4("view", view);

        lightingShader.setVec3("directionalLight.directiaon", 0.0f, -3.0f, 0.0f);
        lightingShader.setVec3("directionalLight.ambient", .5f, .5f, .5f);
        lightingShader.setVec3("directionalLight.diffuse", .8f, .8f, .8f);
        lightingShader.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);

        lightingShader.setBool("directionLightOn", dl);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 0.0));
        slider(lightingShader, translateMatrix, cubeVAO, lightingShaderWithTexture);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(22.0, 0.0, 0.0));
        swing(lightingShader, translateMatrix, cubeVAO,lightingShaderWithTexture);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(22.0, 0.0, -6.0));
        seesaw(lightingShader, translateMatrix, cubeVAO,triangleVAO ,lightingShaderWithTexture);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0, 0.0, -25.0));
        alphabet(lightingShader, translateMatrix, cubeVAO, lightingShaderWithTexture);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, -5.0));
        roundTable(lightingShader, translateMatrix, cubeVAO, lightingShaderWithTexture);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(22.0, 0.0, 3.0));
        FerrisWheel(lightingShader, translateMatrix, cubeVAO);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0, 0.0, 10.0));
        gate(lightingShader, translateMatrix, cubeVAO, lightingShaderWithTexture);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 00.0));
        rotor(lightingShader, translateMatrix, cubeVAO, lightingShaderWithTexture);

        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0, 0.0, 0.0));
        environment(lightingShader, translateMatrix, cubeVAO, lightingShaderWithTexture,ourShader);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0, 5.0, 3.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
        model = translateMatrix * scaleMatrix;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVAO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------



void drawTree(unsigned int VAO, Shader lightShader, glm::mat4 model, glm::vec3 color)
{
    lightShader.use();
    lightShader.setVec3("material.ambient", color);
    lightShader.setVec3("material.diffuse", color);
    lightShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightShader.setFloat("material.shininess", 32.0f);

    lightShader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, treeVertices.size() / 6);
    glBindVertexArray(0);
}
void lightEffect(unsigned int VAO, Shader lightShader, glm::mat4 model, glm::vec3 color,unsigned int size)
{
    lightShader.use();
    lightShader.setVec3("material.ambient", color);
    lightShader.setVec3("material.diffuse", color);
    lightShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightShader.setFloat("material.shininess", 32.0f);

    lightShader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void drawCube(unsigned int VAO, Shader shader, glm::mat4 model, glm::vec4 color)
{
    shader.setMat4("model", model);
    shader.setVec4("color", color);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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

unsigned int hollowBezier(GLfloat ctrlpoints[], int L, vector<float>& coordinates, vector<float>& normals, vector<int>& indices, vector<float>& vertices, float div=1.0)
{
    int i, j;
    float x, y, z, r;                //current coordinates
    float theta;
    float nx, ny, nz, lengthInv;    // vertex normal


    const float dtheta =   (2*pi / ntheta)/div;        //angular step size

    float t = 0;
    float dt = 1.0 / nt;
    float xy[2];
    vector <float> textureCoords;
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

            normals.push_back(-nx);
            normals.push_back(-ny);
            normals.push_back(-nz);

            // Texture coordinates (u, v)
            float u = float(j) / float(ntheta); // Around the circle
            float v = float(i) / float(nt);     // Along the curve
            textureCoords.push_back(u);
            textureCoords.push_back(v);

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

        // Add texture coordinates
        vertices.push_back(textureCoords[i / 3 * 2]);
        vertices.push_back(textureCoords[i / 3 * 2 + 1]);
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
    glEnableVertexAttribArray(2);

    // set attrib arrays with stride and offset
    int stride = 32;     // should be 32 bytes
    glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float))); // Texture Coord
    

    // unbind VAO, VBO and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return bezierVAO;
}

unsigned int f(int &size)
{
    float vertices[] = {
        // positions           // normals
        // Vertical bar
        0.0f, 0.0f, 0.0f,      0.0f, 0.0f, -1.0f,
        0.2f, 0.0f, 0.0f,      0.0f, 0.0f, -1.0f,
        0.2f, 1.5f, 0.0f,      0.0f, 0.0f, -1.0f,
        0.0f, 1.5f, 0.0f,      0.0f, 0.0f, -1.0f,

        0.2f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
        0.2f, 1.5f, 0.0f,      1.0f, 0.0f, 0.0f,
        0.2f, 0.0f, -0.5f,     1.0f, 0.0f, 0.0f,
        0.2f, 1.5f, -0.5f,     1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, -0.5f,     0.0f, 0.0f, 1.0f,
        0.2f, 0.0f, -0.5f,     0.0f, 0.0f, 1.0f,
        0.2f, 1.5f, -0.5f,     0.0f, 0.0f, 1.0f,
        0.0f, 1.5f, -0.5f,     0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, -0.5f,     -1.0f, 0.0f, 0.0f,
        0.0f, 1.5f, -0.5f,     -1.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f,      -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,      -1.0f, 0.0f, 0.0f,

        0.0f, 1.5f, -0.5f,     0.0f, 1.0f, 0.0f,
        0.2f, 1.5f, -0.5f,     0.0f, 1.0f, 0.0f,
        0.2f, 1.5f, 0.0f,      0.0f, 1.0f, 0.0f,
        0.0f, 1.5f, 0.0f,      0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f,      0.0f, -1.0f, 0.0f,
        0.2f, 0.0f, 0.0f,      0.0f, -1.0f, 0.0f,
        0.2f, 0.0f, -0.5f,     0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, -0.5f,     0.0f, -1.0f, 0.0f
    };

    unsigned int indices[] = {
        // Vertical bar front face
        0, 1, 2,  0, 2, 3,

        // Vertical bar back face
        4, 5, 6,  4, 6, 7,

        // Vertical bar left side
        8, 9, 10,  8, 10, 11,

        // Vertical bar right side
        12, 13, 14,  12, 14, 15,

        // Vertical bar top
        16, 17, 18,  16, 18, 19,

        // Vertical bar bottom
        20, 21, 22,  20, 22, 23,

        // Indices for top horizontal bar and middle horizontal bar go here...
    };
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    size = sizeof(indices)/sizeof(int);

    return VAO;

}

void read_file(string file_name, vector<float>& vec)
{
    ifstream file(file_name);
    float number;

    while (file >> number)
        vec.push_back(number);

    file.close();
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
       
        camera.ProcessKeyboard(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        
        camera.ProcessKeyboard(P_UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
       
        camera.ProcessKeyboard(P_DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        
        camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        
        camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
       
        camera.ProcessKeyboard(R_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
       
        camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }

    
    //if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) birdEye = true;
    //if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) birdEye = false;

    /*if (birdEye) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos.z -= birdEyeSpeed * deltaTime;
            target.z -= birdEyeSpeed * deltaTime;
            if (cameraPos.z <= 4.0) {
                cameraPos.z = 4.0;
            }

            if (target.z <= -3.5) {
                target.z = -3.5;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos.z += birdEyeSpeed * deltaTime;
            target.z += birdEyeSpeed * deltaTime;
            
            if (cameraPos.z >= 13.5) {
                cameraPos.z = 13.5;
            }
            if (target.z >= 6.0) {
                target.z = 6.0;
            }
        }

    }*/
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        isFerrisWheelOn = true;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        isFerrisWheelOn = false;
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        isswingOn = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        isswingOn = false;
    }
    

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        isGateOpening = 1.0f;;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        isGateOpening = -1.0f;;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        isseesawOn = true;;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        isseesawOn = false;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        isRotorOn = true;;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        isRotorOn = false;
    }


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{


    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (pointLightOn)
        {
            pointLightOn = false;
            pointlight1.turnOff();
            pointlight2.turnOff();
            pointlight3.turnOff();
            pointlight4.turnOff();
        }
        else
        {
            pointLightOn = true;
            pointlight1.turnOn();
            pointlight2.turnOn();
            pointlight3.turnOn();
            pointlight4.turnOn();
        }

    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (point5)
        {
            point5 = false;
            pointlight5.turnOff();
        }
        else
        {
            point5 = true;
            pointlight5.turnOn();
        }

    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (spot1)
        {
            spot1 = false;
            spotlight1.turnOff();
            
        }
        else
        {
            spot1 = true;
            spotlight1.turnOn();
        }

    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        if (spot2)
        {
            spot2 = false;
            spotlight2.turnOff();
        }
        else
        {
            spot2 = true;
            spotlight2.turnOn();
        }

    }

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        if (spot3)
        {
            spot3 = false;
            spotlight3.turnOff();
        }
        else
        {
            spot3 = true;
            spotlight3.turnOn();
        }

    }

    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        dl = !dl;

    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        if (specularToggle)
        {
            pointlight1.turnSpecularOff();
            pointlight2.turnSpecularOff();
            //d_def_on = 0.0f;

            specularToggle = false;
        }
        else
        {
            pointlight1.turnSpecularOn();
            pointlight2.turnSpecularOn();

            //d_def_on = 1.0f;
            specularToggle = true;
        }

    }


}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    //glViewport(0, 0, width, height);

    // make sure the viewport matches the new window dimensions; note that width and height will be significantly larger than specified on retina displays.
    float aspectRatio = 4.0f / 3.0f;
    int viewWidth, viewHeight;

    if (width / (float)height > aspectRatio) {
        //Window is too wide, fit height and adjust width
        viewHeight = height;
        viewWidth = (int)(height * aspectRatio);
    }
    else {
        //Window is too tall, fit width and adjust height
        viewWidth = width;
        viewHeight = (int)(width / aspectRatio);
    }

    //Center the viewport
    int xOffset = (width - viewWidth) / 2;
    int yOffset = (height - viewHeight) / 2;

    glViewport(xOffset, yOffset, viewWidth, viewHeight);
    //glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void load_texture(unsigned int& texture, string image_name, GLenum format)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(image_name.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture " << image_name << endl;
    }
    stbi_image_free(data);
}

void textureEffect(unsigned int VAO, Shader shader, glm::mat4 model, unsigned int indicesSize, unsigned int texture0)
{
    shader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);
    shader.setFloat("material.shininess", 32.0f);
    shader.setMat4("model", model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, (void*)0);
}

void FerrisWheel(Shader ourShader, glm::mat4 moveMatrix, unsigned int cubeVAO)
{

    //front
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp;

    if (isFerrisWheelOn)
    {
        ferrisWheelSpeed += ferrisIncrement;
        ferrisWheelSpeed = min(0.5f, ferrisWheelSpeed);
        //ferrisWheelAngle += 0.5f;
    }
    else
    {
        ferrisWheelSpeed -= ferrisIncrement;
        ferrisWheelSpeed = max(0.02f, ferrisWheelSpeed);
        //ferrisWheelAngle -= 0.05;
    }

    if (ferrisWheelSpeed != 0.02f)
        ferrisWheelAngle += ferrisWheelSpeed;
    
    ferrisWheelAngle = fmod(ferrisWheelAngle, 360);
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(ferrisWheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 3.0f, 0.0f));//2.0
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateZMatrix * scaleMatrix * rotateTemp;

    lightEffect(ferriVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesFerri.size());
    
    //bars
    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec4 pivot = glm::vec4(glm::vec3(0.0f), 1.0f);

    pivot = translateMatrix * pivot;

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 3.0f, -0.025f));//2.0
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.5f, 0.1f));

    for (float i = 0.0f; i >= -360.0f; i -= 45.0f)
    {
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * rotateTemp * scaleMatrix;
        lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);
        
    }



    //back
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(ferrisWheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 3.0f, -1.0f));//2.0
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateZMatrix * scaleMatrix * rotateTemp;
    lightEffect(ferriVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesFerri.size());
    
    //bars

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 3.0f, -1.025f));//2.0
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.5f, 0.1f));
    for (float i = 0.0f; i >= -360.0f; i -= 45.0f)
    {
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * rotateTemp * scaleMatrix;
        lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024),36);
        
    }


    //connecting bars
    //middle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.975f, 2.975f, -1.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 2.0f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);
    

    //top ones
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 4.5f, -1.005f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 1.0f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    for (float i = 0.0f; i >= -360.0f; i -= 90.0f)
    {
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateFromPivot * rotateZMatrix * translateToPivot * translateFromPivot * rotateTemp * translateToPivot * translateMatrix * scaleMatrix;
        lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);
        
    }

    //pillars
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, -0.41f, 0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.2f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);
    
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.1f, -0.47f, 0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.2f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(31.5f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, -0.41f, -1.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.2f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.1f, -0.47f, -1.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.2f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(31.5f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);


    //sitting places
    translateMatrix = glm::translate(identityMatrix, glm::vec3(22.0f, 1.0f, 3.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle,cubeVAO);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(23.5f, 2.5f, 3.0f));//1.5f, 1.5f, 0.0f
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle, cubeVAO);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(22.0f, 4.0f, 3.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle, cubeVAO);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(20.5f, 2.5f, 3.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle, cubeVAO);

}

void FerrisWheelSeat(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z,unsigned int cubeVAO)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); 
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrixLocal, rotateZMatrixMain, rotateTemp;
    glm::mat4 translateToPivotMain, translateFromPivotMain;
    glm::vec4 translateToPivotOrgMain, translateFromPivotOrgMain;
    glm::mat4 translateToPivotLocal, translateFromPivotLocal;
    glm::vec4 translateToPivotOrgLocal, translateFromPivotOrgLocal;
    glm::vec4 pivot1, pivot2;
    glm::vec4 mainPivotPoint = glm::vec4(25.0f, 3.0f, 0.0f, 1.0);//3.0,2.0,0.0
    glm::vec4 localPivotPoint = glm::vec4(2.975f, 0.475f, -1.005f, 1.0);

    pivot1 = mainPivotPoint;
    translateToPivotMain = glm::translate(identityMatrix, glm::vec3(-pivot1));
    translateFromPivotMain = glm::translate(identityMatrix, glm::vec3(pivot1));
    rotateZMatrixMain = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Z), glm::vec3(0.0f, 0.0f, 1.0f));

    pivot2 = translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * moveMatrix * localPivotPoint;
    translateToPivotLocal = glm::translate(identityMatrix, glm::vec3(-pivot2));
    translateFromPivotLocal = glm::translate(identityMatrix, glm::vec3(pivot2));
    rotateZMatrixLocal = glm::rotate(identityMatrix, glm::radians(-rotateAngleTest_Z), glm::vec3(0.0f, 0.0f, 1.0f));

    //base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.05f, 0.75f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.7), 36);
    
    //0.7725, 0.6235, 0.4510  0.169, 0.078, 0.024
    //left
    //seat
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, -0.15f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.25f, 0.75f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.7725, 0.6235, 0.4510), 36);

    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.1f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.3f, 0.75f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.7725, 0.6235, 0.4510), 36);

    //seat right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.2f, -0.15f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.25f, 0.75f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.7725, 0.6235, 0.4510), 36);
    

    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.45f, 0.1f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.3f, 0.75f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.7725, 0.6235, 0.4510), 36);
    
    //left bars
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55f, 0.3f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.05f, 0.05f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55f, 0.3f, -0.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.05f, 0.05f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);
    
    //right bar
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.45f, 0.3f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.5f, 0.05f, 0.05f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);


    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.45f, 0.3f, -0.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.5f, 0.05f, 0.05f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, ourShader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

}

void swing(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp;

    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec4 pivot = glm::vec4(glm::vec3(3.1f, 3.3f, -3.0f), 1.0f);

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    float currentTime = static_cast<float>(glfwGetTime());
    if (currentTime - lastTime > 0.1)
    {
        if (isswingOn)
            swingAngle += swingSpeed;
        if (abs(swingAngle) >= swingMaxAngle)
        {
            swingSpeed *= -1;
            lastTime = static_cast<float>(glfwGetTime());
        }
    }
    if (!isswingOn)
    {
        if (swingAngle < 0)
        {
            swingAngle += abs(swingSpeed);
        }
        if (swingAngle > 0)
        {
            swingAngle -= abs(swingSpeed);
        }
    }
    
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(swingAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    //pillars
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.0f, -0.15f, -1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.2f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);

    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.2f, -0.2f, -1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.2f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(31.5f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);
   
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.0f, -0.15f, -5.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.2f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);
    
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.2f, -0.2f, -5.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.2f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(31.5f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);

    //middle bar
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.1f, 3.3f, -1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, -3.8f));
    model = translateMatrix* scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);

    //seat
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 1.0f, -2.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.1f, 0.1f, -2.0f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    Cube cube4 = Cube(texture3, texture3, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    cube4.drawCubeWithTexture(texureShader, moveMatrix * model);
    //lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8));

    //swing rope1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.1f, 1.1f, -3.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.72f, 0.4f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    textureEffect(ropeVAO, texureShader, moveMatrix * model, indicesRope.size(), texture2);

    //swing rope2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.1f, 1.1f, -2.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.72f, 0.4f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    textureEffect(ropeVAO, texureShader, moveMatrix * model, indicesRope.size(), texture2);
    
    
}

void seesaw(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, unsigned int triVAO, Shader texureShader)
{
    float currentTime = static_cast<float>(glfwGetTime());
    if (currentTime - lastTimess > 0.1)
    {
        
        if (abs(seesawAngle) >= seesawMaxAngle)
        {
            seesawSpeed *= -1;
            lastTimess = static_cast<float>(glfwGetTime());
        }
        if (isseesawOn)
            seesawAngle += seesawSpeed;
    }
    if (!isseesawOn)
    {
        if (seesawAngle < 20.0)
        {
            seesawAngle += abs(seesawSpeed);
        }
        if (seesawAngle > 20.0)
        {
            seesawAngle -= abs(seesawSpeed);
        }
    }
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp;

    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec4 pivot = glm::vec4(glm::vec3(2.7f, 1.2f, -5.75f), 1.0f);

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    //leg left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 0.0f, -5.3f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 1.5f, 0.7f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = translateMatrix* rotateTemp* scaleMatrix;
    lightEffect(triVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024),24);

    //leg right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.0f, -6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 1.5f, 0.7f));
    model = translateMatrix * scaleMatrix;
    lightEffect(triVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 24);

    //middle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 1.2f, -5.75f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, 0.5f, 0.8f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp*scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());
    //textureEffect(ropeVAO, texureShader, moveMatrix * model, indicesRope.size(), texture2);

    //seat
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.3f, 1.2f, -8.75f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 6.0f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(seesawAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = translateFromPivot * rotateTemp * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.6), 36);

    //handle
    //beck left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.4f, 1.2f, -7.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.2f, 0.4f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(seesawAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = translateFromPivot * rotateTemp * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.2f, 1.2f, -7.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.2f, 0.4f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(seesawAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = translateFromPivot * rotateTemp * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    glm::mat4 rotate;
    //back top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.2f, 1.8f, -7.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.25f, 0.4f));
    rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(seesawAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = translateFromPivot * rotateTemp * translateToPivot * translateMatrix * rotate * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.4f, 1.2f, -3.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.2f, 0.4f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(seesawAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = translateFromPivot * rotateTemp * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.2f, 1.2f, -3.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.2f, 0.4f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(seesawAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = translateFromPivot * rotateTemp * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //glm::mat4 rotate;
    //back top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.2f, 1.8f, -3.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.25f, 0.4f));
    rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(seesawAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = translateFromPivot * rotateTemp * translateToPivot * translateMatrix * rotate * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

}

unsigned int triangle()
{
    float triangle_vertices[] = {
        // Triangular End 1 (e.g., left end)
        // Positions        // Normals             // Texture Coords
        0.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,   TXmin, TYmin, // Vertex 0
        1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,   TXmax, TYmin, // Vertex 1
        0.0f, 1.0f, 0.0f,   0.0f,  0.0f, -1.0f,   TXmin, TYmax, // Vertex 2

        // Triangular End 2 (e.g., right end)
        // Positions        // Normals             // Texture Coords
        0.0f, 0.0f, 1.0f,   0.0f,  0.0f, 1.0f,     TXmin, TYmin, // Vertex 3
        1.0f, 0.0f, 1.0f,   0.0f,  0.0f, 1.0f,     TXmax, TYmin, // Vertex 4
        0.0f, 1.0f, 1.0f,   0.0f,  0.0f, 1.0f,     TXmin, TYmax, // Vertex 5

        // Rectangular Side 1 (Bottom Face)
        // Positions        // Normals             // Texture Coords
        0.0f, 0.0f, 0.0f,   0.0f, -1.0f, 0.0f,     TXmin, TYmin, // Vertex 6
        1.0f, 0.0f, 0.0f,   0.0f, -1.0f, 0.0f,     TXmax, TYmin, // Vertex 7
        1.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,     TXmax, TYmax, // Vertex 8
        0.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,     TXmin, TYmax, // Vertex 9

        // Rectangular Side 2 (Vertical Face Adjacent to Y-axis)
        // Positions        // Normals             // Texture Coords
        0.0f, 0.0f, 0.0f,  -1.0f,  0.0f, 0.0f,     TXmax, TYmin, // Vertex 10
        0.0f, 1.0f, 0.0f,  -1.0f,  0.0f, 0.0f,     TXmax, TYmax, // Vertex 11
        0.0f, 1.0f, 1.0f,  -1.0f,  0.0f, 0.0f,     TXmin, TYmax, // Vertex 12
        0.0f, 0.0f, 1.0f,  -1.0f,  0.0f, 0.0f,     TXmin, TYmin, // Vertex 13

        // Rectangular Side 3 (Hypotenuse Face)
        // Positions        // Normals             // Texture Coords
        0.0f, 1.0f, 0.0f,   0.0f,  1.0f, 0.0f,     TXmax, TYmin, // Vertex 14
        1.0f, 0.0f, 0.0f,   0.0f,  1.0f, 0.0f,     TXmax, TYmax, // Vertex 15
        1.0f, 0.0f, 1.0f,   0.0f,  1.0f, 0.0f,     TXmin, TYmax, // Vertex 16
        0.0f, 1.0f, 1.0f,   0.0f,  1.0f, 0.0f,     TXmin, TYmin, // Vertex 17
    };

    unsigned int triangle_indices[] = {
        // Triangular End 1
        0, 1, 2,

        // Triangular End 2
        3, 5, 4,

        // Rectangular Side 1 (Bottom Face)
        6, 7, 8,
        8, 9, 6,

        // Rectangular Side 2 (Vertical Face Adjacent to Y-axis)
        10, 11, 12,
        12, 13, 10,

        // Rectangular Side 3 (Hypotenuse Face)
        14, 15, 16,
        16, 17, 14
    };


    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
    glEnableVertexAttribArray(2);

    return VAO;
}

void slider(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, modelCentered;

    //slider floor1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 3.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 0.1f, 2.0f));
    model = translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.098, 0.114, 0.125), 36);

    //slider floor2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0, 3.0, 2.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.1f, -5.0f));
    model = translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.098, 0.114, 0.125), 36);

    //leg1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //leg2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 1.8));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //leg3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.8, 0.0, 1.8));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //leg4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0, 0.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //leg5
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0, 0.0, -3.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //leg6
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.8, 0.0, -3.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //leg7
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.8, 0.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //leg8
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.8, 0.0, 1.8));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 3.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);


    //slider
    glm::mat4 translateBack= glm::translate(identityMatrix, glm::vec3(-5.9, -3.1, -2.5));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.9, 3.1, 2.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 1.0, 0.5));
    
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(145.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(rotateXMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //rotateZMatrix = glm::rotate(rotateXMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateYMatrix * translateBack* translateMatrix * scaleMatrix;
    lightEffect(sliderVAO, shader, moveMatrix * model, glm::vec3(0.0, 1.0, 0.5), indicesRope.size());

    //top extra
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0, 3.0, 2.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.8f, 0.1f, 0.5f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.7725, 0.6235, 0.4510), 36);

    //top extra down
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0, 3.1, 2.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.8f, -0.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.7725, 0.6235, 0.4510), 36);

    //handle front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.2, 3.1, 2.6));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.3f, 0.5f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.7725, 0.6235, 0.4510), indicesRope.size());

    //handle back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.2, 3.1, 2.1));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.3f, 0.5f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(0.7725, 0.6235, 0.4510), indicesRope.size());

    //handle front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.6, 3.1, 2.6));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.3f, 0.5f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(0.7725, 0.6235, 0.4510), indicesRope.size());

    //handle back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.6, 3.1, 2.1));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.3f, 0.5f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(0.7725, 0.6235, 0.4510), indicesRope.size());

    //pillar1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 3.1, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.5f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);

    //handle middle left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.2, 4.0, 2.1));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.16f, 0.5f));
    model = translateMatrix* rotateXMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(0.7725, 0.6235, 0.4510), indicesRope.size());

    //handle middle right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.6, 4.0, 2.1));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.16f, 0.5f));
    model = translateMatrix * rotateXMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(0.7725, 0.6235, 0.4510), indicesRope.size());

    //pillar2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 3.1, 1.8));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.5f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), 36);


    //slider_wall1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.2, 3.1, 1.8));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.6f, 1.4f, 0.1f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix* model, glm::vec3(0.7725, 0.6235, 0.4510), 36);

    //pillar3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.8, 3.1, 1.8));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.5f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix* model, glm::vec3(0.169, 0.078, 0.024), 36);


    //slider_wall2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 3.1, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 1.4f, 0.1f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix* model, glm::vec3(0.7725, 0.6235, 0.4510), 36);

    //pillar4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0, 3.1, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.5f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix* model, glm::vec3(0.169, 0.078, 0.024), 36);

    //slider_wall3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.9, 3.1, -2.8));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.4f, 4.6f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix* model, glm::vec3(0.7725, 0.6235, 0.4510), 36);

    //pillar5
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0, 3.1, -3.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.5f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix* model, glm::vec3(0.169, 0.078, 0.024), 36);

    //slider_wall4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.2, 3.1, -3.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.6f, 1.4f, 0.1f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.7725, 0.6235, 0.4510, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix* model, glm::vec3(0.7725, 0.6235, 0.4510), 36);

    //pillar5
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.8, 3.1, -3.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.5f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix* model, glm::vec3(0.169, 0.078, 0.024), 36);


    //pillar6
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.8, 3.1, 1.8));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.5f, 0.2f));
    model = translateMatrix * scaleMatrix;
    //drawCube(cubeVAO, ourShader, model, glm::vec4(0.169, 0.078, 0.024, 1.0));
    lightEffect(cubeVAO, shader, moveMatrix* model, glm::vec3(0.169, 0.078, 0.024), 36);

    //rope1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.3, 0.0, 0.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 1.0, 0.5));
    model = translateMatrix * scaleMatrix;
    textureEffect(ropeVAO, texureShader, moveMatrix* model, indicesRope.size(), texture2);

    //rope_upper_1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1, 3.1, 0.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.13, 0.5));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(rotateXMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateZMatrix * scaleMatrix;
    textureEffect(ropeVAO, texureShader, moveMatrix* model, indicesRope.size(), texture2);


    //rope2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.3, 0.0, 1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 1.0, 0.5));
    model = translateMatrix * scaleMatrix;
    textureEffect(ropeVAO, texureShader, moveMatrix* model, indicesRope.size(), texture2);


    //rope_upper2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1, 3.1, 1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.13, 0.5));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(rotateXMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateZMatrix * scaleMatrix;
    textureEffect(ropeVAO, texureShader, moveMatrix* model, indicesRope.size(), texture2);


    translateMatrix = identityMatrix;
    float y = 0;
    for (int i = 0;i < 3;i++)
    {
        translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(-0.45, 0.5, 0.3));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.4f, 1.4f));
        model = translateMatrix * scaleMatrix;
        textureEffect(cubeVAO, texureShader, moveMatrix * model, 36, texture3);
        y += 1.0;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, y, 0.0));

    }

    //mashroom pilla1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0, 0.0, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8, 18.0, 0.8));
    model = translateMatrix * scaleMatrix;
    textureEffect(pillarVAO, texureShader, moveMatrix * model, indicesPillar.size(), texture4);

    //mashroom1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0, 2.2, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.5, 0.3));
    model = translateMatrix * scaleMatrix;
    textureEffect(hillVAO, texureShader, moveMatrix * model, indices.size(), texture5);

    //mashroom pilla2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0, 0.0, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8, 14.0, 0.8));
    model = translateMatrix * scaleMatrix;
    textureEffect(pillarVAO, texureShader, moveMatrix * model, indicesPillar.size(), texture4);


    //mashroom2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0, 1.8, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.5, 0.3));
    model = translateMatrix * scaleMatrix;
    textureEffect(hillVAO, texureShader, moveMatrix * model, indices.size(), texture5);



    //mashroom pilla3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8, 10.0, 0.8));
    model = translateMatrix * scaleMatrix;
    textureEffect(pillarVAO, texureShader, moveMatrix * model, indicesPillar.size(), texture4);

    //mashroom3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 1.0, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.5, 0.3));
    model = translateMatrix * scaleMatrix;
    textureEffect(hillVAO, texureShader, moveMatrix * model, indices.size(), texture5);
}

void alphabet(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp;

    //F lombo
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.8, 1.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //F top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.8, 3.05, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //F middle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.8, 2.15, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //U lombo left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.7, 1.5, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //U lombo right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.5, 1.8, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //U bottom
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.6, 1.7, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-72.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //N left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.6, 2.2, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix* scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //N middle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.6, 4.4, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-150.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //N right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.6, 2.5, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //T lombo
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.4, 2.6, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //T top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0, 4.8, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //A left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5, 2.9, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //A right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 2.9, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //A middle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.4, 3.5, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.35f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //S top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 4.9, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //S middle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 4.0, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //S bottom
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 3.1, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //S right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.6, 3.25, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.21f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //S left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65, 4.1, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.21f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //T lombo
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.9, 2.6, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //T top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.3, 4.7, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //I lombo
    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.4, 2.4, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    // C lombo
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.4, 2.1, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.5f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //C top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.4, 4.15, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //C bottom
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.4, 2.2, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.4f, 1.5f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix* model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());


}

unsigned int  generateSphere(float radius, int sectorCount, int stackCount, unsigned int &sphereIndicesSize) {
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal, lenginv is the inverse of the radius
    vector<float> vertices;
    vector<unsigned int> indices;

    float sectorStep = 2 * pi / sectorCount;
    float stackStep = pi / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = pi / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    // generate indices
    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (unsigned int)vertices.size() * sizeof(float),vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    sphereIndicesSize = indices.size();

    return VAO;
}

void roundTable(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp, translateToPivot, translateFromPivot;

    glm::vec4 pivot = glm::vec4(glm::vec3(3.0, 1.0, -8.5), 1.0f);

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    //table
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 1.5, -10.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.2f, 2.0f));
    model = translateMatrix * scaleMatrix;
    //lightEffect(sphereVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), sphereIndicesSize);
    textureEffect(sphereVAO, texureShader, moveMatrix * model, sphereIndicesSize, texture3);

    //middle pillar
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 0.0, -10.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.5f, 2.0f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //chair 1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 0.0));
    chair(shader, moveMatrix*translateMatrix, cubeVAO, texureShader);

    //chair2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 3.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
    model = translateFromPivot * rotateTemp * translateToPivot* translateMatrix;
    chair(shader, moveMatrix * model, cubeVAO, texureShader);

    //chair3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.0, 0.0, 1.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    model = translateFromPivot * rotateTemp * translateToPivot * translateMatrix;
    chair(shader, moveMatrix * model, cubeVAO, texureShader);

    //chair4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0, 0.0, 2.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
    model = translateFromPivot * rotateTemp * translateToPivot * translateMatrix;
    chair(shader, moveMatrix * model, cubeVAO, texureShader);

    //middle top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 1.2, -10.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, 1.1f, 0.8f));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indicesRope.size());

    //table roof
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 3.45, -10.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 1.2f, 1.2f));
    model = translateMatrix * scaleMatrix;
    //lightEffect(hillVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), indices.size());
    textureEffect(hillVAO, texureShader, moveMatrix * model, indices.size(), textureFlower);
    
}

void chair(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp;

    //chair seat1 
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 1.0, -8.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 0.1, 1.0));
    model = translateMatrix * scaleMatrix;
    //lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), 36);
    Cube cubeSeat = Cube(textureSeat, textureSeat, 32.0f, 0.0f, 0.0f, 5.0f, 5.0f);
    cubeSeat.drawCubeWithTexture(texureShader, moveMatrix * model);

    //chair seat1 pillar1 top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.6, 1.1, -7.1));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.3, 0.5));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //chair seat1 pillar2 top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.4, 1.1, -7.1));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.3, 0.5));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //chair seat1 back 
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 1.6, -7.2));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 0.5, 0.1));
    model = translateMatrix * scaleMatrix;
    //lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), 36);
    Cube cubeBack = Cube(textureSeat, textureSeat, 32.0f, 0.0f, 0.0f, 2.0f, 1.0f);
    cubeBack.drawCubeWithTexture(texureShader, moveMatrix * model);

    //chair seat1 pillar1 down
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.6, 1.0, -7.9));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, -0.3, 0.5));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //chair seat1 pillar2 down
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.4, 1.0, -7.9));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, -0.3, 0.5));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //chair seat1 pillar1 bottom
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.6, 0.05, -7.9));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
    rotateTemp = glm::rotate(rotateTemp, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, -0.3, 0.5));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //chair seat1 pillar2 bottom
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.4, 0.05, -7.9));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
    rotateTemp = glm::rotate(rotateTemp, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, -0.3, 0.5));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //chair seat1 pillar3 bottom
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.57, 0.05, -7.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    //rotateTemp = glm::rotate(rotateTemp, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, -0.27, 0.5));
    model = translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());
}

void environment(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader, Shader ourshader)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp, translateToPivot, translateFromPivot;

    glm::vec4 pivot = glm::vec4(glm::vec3(3.0, 1.0, -8.5), 1.0f);

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    //bench1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 0.0));
    bench(shader, moveMatrix * translateMatrix, cubeVAO, texureShader);

    //bench2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, -12.0));
    bench(shader, moveMatrix * translateMatrix, cubeVAO, texureShader);

    //bench3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.5, 0.0, 0.0));
    bench(shader, moveMatrix * translateMatrix, cubeVAO, texureShader);

    //bench4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.5, 0.0, -12.0));
    bench(shader, moveMatrix * translateMatrix, cubeVAO, texureShader);

    //light1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0, 0.0, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 1.0, 0.5));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());
    //Cube cubeSeat = Cube(texture3, texture3, 32.0f, 0.0f, 0.0f, 1.0f, 3.0f);
    //cubeSeat.drawCubeWithTexture(texureShader, moveMatrix * model);

    //light 2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0, 0.0, -7.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 1.0, 0.5));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());

    //light 3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0, 0.0, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 1.0, 0.5));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());
    
    //light 4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0, 0.0, -7.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 1.0, 0.5));
    model = translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), indicesRope.size());


    //tree1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0, 0.0, 4.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 1.0, 1.0));
    model = translateMatrix * scaleMatrix;
    //lightEffect(treeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), treeIndices.size());
    drawTree(treeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.6));

    //tree2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0, 0.0, 4.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 1.0, 1.0));
    model = translateMatrix * scaleMatrix;
    //lightEffect(treeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), treeIndices.size());
    drawTree(treeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.6));

    glm::vec4 color(0.5, 0.5, 0.5, 1.0);
    if (pointLightOn)
        color = glm::vec4(1.0, 1.0, 1.0, 1.0);
    
    //pointlights
    ourshader.use();

    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0, 3.5, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.5, 0.5));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourshader.setMat4("model", moveMatrix*model);
    ourshader.setVec4("color", color);
    glBindVertexArray(coneVAO);
    glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, 0);
    
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0, 3.5, -7.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.5, 0.5));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourshader.setMat4("model", moveMatrix * model);
    ourshader.setVec4("color", color);
    glBindVertexArray(coneVAO);
    glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, 0);

    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0, 3.5, -1.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.5, 0.5));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourshader.setMat4("model", moveMatrix * model);
    ourshader.setVec4("color", color);
    glBindVertexArray(coneVAO);
    glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, 0);

    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0, 3.5, -7.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.5, 0.5));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourshader.setMat4("model", moveMatrix * model);
    ourshader.setVec4("color", color);
    glBindVertexArray(coneVAO);
    glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, 0);

    //spotlight
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(12.0, 1.6, -22.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.3, 0.2));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-45.0f), glm::vec3(0.0, 0.0, 1.0));
    rotateTemp = glm::rotate(rotateTemp, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourshader.setMat4("model", moveMatrix * model);
    ourshader.setVec4("color", glm::vec4(0.0, 0.0, 0.0, 1.0));
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0, 1.0, -21.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5, 0.3, 0.2));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
    rotateTemp = glm::rotate(rotateTemp, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourshader.setMat4("model", moveMatrix * model);
    ourshader.setVec4("color", glm::vec4(0.0, 0.0, 0.0, 1.0));
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    
}
void bench(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp, translateToPivot, translateFromPivot;

    //bench seat1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5, 1.0, 3.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 0.1, -3.0));
    model = translateMatrix * scaleMatrix;
    //lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), 36);
    Cube cubeSeat = Cube(texture3, texture3, 32.0f, 0.0f, 0.0f, 1.0f, 3.0f);
    cubeSeat.drawCubeWithTexture(texureShader, moveMatrix * model);

    //bench lag1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.65, 1.0, 2.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7, -0.9, 0.1));
    model = translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), 36);

    //bench lag2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.65, 1.0, 0.5));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7, -0.9, 0.1));
    model = translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(1.0, 1.0, 1.0), 36);
}

void gate(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model, rotateYMatrix, rotateTemp, translateToPivot, translateFromPivot;
    glm::vec4 pivot1 = glm::vec4(glm::vec3(0.2, 2.25, 0.0), 1.0f);
    glm::vec4 pivot2 = glm::vec4(glm::vec3(6.8, 2.25, 0.0), 1.0f);

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot1));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot1));

    gateAngle += isGateOpening * gateSpeed;
    if (isGateOpening == -1.0f)
        gateAngle = max(0.0f, gateAngle);
    if (isGateOpening == 1.0f)
        gateAngle = min(90.0f, gateAngle);

    rotateYMatrix= glm::rotate(identityMatrix, glm::radians(gateAngle), glm::vec3(0.0, 1.0, 0.0));
    //gate pillar 1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.2, 0.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2, 6.0, 0.2));
    model = translateMatrix * scaleMatrix;
    Cube cubeGate1 = Cube(texture3, texture3, 32.0f, 0.0f, 0.0f, 1.0f, 30.0f);
    cubeGate1.drawCubeWithTexture(texureShader, moveMatrix * model);

    //gate pillar 2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.8, 0.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2, 6.0, 0.2));
    model = translateMatrix * scaleMatrix;
    //Cube cubeSeat = Cube(texture3, texture3, 32.0f, 0.0f, 0.0f, 1.0f, 30.0f);
    cubeGate1.drawCubeWithTexture(texureShader, moveMatrix * model);

    //gate pillar 3 top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2, 5.5, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2, 7.3, 0.2));
    model = translateMatrix * rotateTemp * scaleMatrix;
    //Cube cubeSeat = Cube(texture3, texture3, 32.0f, 0.0f, 0.0f, 1.0f, 30.0f);
    cubeGate1.drawCubeWithTexture(texureShader, moveMatrix * model);

    //welcome
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 4.5, 0.0));
    //rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0, 0.7, 0.1));
    model = translateMatrix * scaleMatrix;
    Cube cubeWel = Cube(textureWel, textureWel, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    cubeWel.drawCubeWithTexture(texureShader, moveMatrix * model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.7, 5.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.2, 0.3));
    model = translateMatrix * scaleMatrix;
    //Cube cubeWel = Cube(textureWel, textureWel, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //cubeWel.drawCubeWithTexture(texureShader, moveMatrix * model);
    textureEffect(ropeVAO, texureShader, moveMatrix * model,indicesRope.size(),texture2);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, 5.0, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3, 0.2, 0.3));
    model = translateMatrix * scaleMatrix;
    //Cube cubeWel = Cube(textureWel, textureWel, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //cubeWel.drawCubeWithTexture(texureShader, moveMatrix * model);
    textureEffect(ropeVAO, texureShader, moveMatrix * model, indicesRope.size(), texture2);


    int i;
    float ext = 0.0,ty=0.0,ext2=0.0;
    for (i = 0;i < 6;i++)
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5+ty, 0.0, 0.1));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2, 4.5-ext, 0.1));
        model = translateFromPivot * rotateYMatrix * translateToPivot* translateMatrix * scaleMatrix;
        lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);
        
        if (i == 0)
        {
            ext += 0.5;
            ty += 0.5;
            continue;
        }

        ext += 0.25;
        ty += 0.5;

        

    
    }

    //left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.2, 4.0, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.2, 0.2, 0.1));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.2, 0.4, 0.0));
    //rotateTemp = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.1, 0.2, 0.1));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);


    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot2));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot2));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-gateAngle), glm::vec3(0.0, 1.0, 0.0));
    ext = 4.5 - ext+.25;
    for (i = 0;i < 7;i++)
    {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5 + ty, 0.0, 0.1));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2, ext+ ext2, 0.1));
        model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
        lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);

        ext2 += 0.25;
        ty += 0.5;
    }

    //left2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.8, 4.3, 0.0));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(210.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.8, 0.2, 0.1));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);

    

    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.8, 0.4, 0.0));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-3.4, 0.2, 0.1));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);
    

}

void rotor(Shader shader, glm::mat4 moveMatrix, unsigned int cubeVAO, Shader texureShader)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp, rotateYMatrix;

    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec4 pivot = glm::vec4(glm::vec3(2.5f, 0.2f, -8.0f), 1.0f);

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    if (isRotorOn)
    {
        rotorSpeed += rotorIncrement;
        rotorSpeed = min(3.0f, rotorSpeed);
    }
    else
    {
        rotorSpeed -= rotorIncrement;
        rotorSpeed = max(0.02f, rotorSpeed);
    }

    if (rotorSpeed != 0.02f)
        rotorAngle += rotorSpeed;

    rotorAngle = fmod(rotorAngle, 360);
    //rotateAngleTest_Z = rotorAngle;

    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotorAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    //base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.0f, -8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.2f, 0.5f));
    //model = translateMatrix * rotateYMatrix * scaleMatrix;
    model = translateMatrix * scaleMatrix;
    lightEffect(cubeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.8), 36);

    //round base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.2f, -8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 1.0f));
    //model = translateMatrix * rotateYMatrix * scaleMatrix;
    model = translateFromPivot* rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(sphereVAO, shader, moveMatrix * model, glm::vec3(0.169, 0.078, 0.024), sphereIndicesSize);

    //lombo
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.2f, -8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.5f, 0.4f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.6), indicesRope.size());


    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.9f, 1.8f, -8.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.25f, 0.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * rotateTemp * scaleMatrix;
    lightEffect(ropeVAO, shader, moveMatrix * model, glm::vec3(0.4, 0.5, 0.6), indicesRope.size());
}

// Function to generate fractal tree branches with normals in 3D
void generateTree(std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec3 start, glm::vec3 direction, float length, int depth, int maxDepth) {
    if (depth == 0 || length < 0.01f) return;

    // Calculate the end point of the current branch
    glm::vec3 end = start + direction * length;
    glm::vec3 normal = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 0.0f, 1.0f)));

    // Add vertices and normals for the branch
    unsigned int startIndex = vertices.size() / 6;
    vertices.push_back(start.x);
    vertices.push_back(start.y);
    vertices.push_back(start.z);
    vertices.push_back(normal.x);
    vertices.push_back(normal.y);
    vertices.push_back(normal.z);

    vertices.push_back(end.x);
    vertices.push_back(end.y);
    vertices.push_back(end.z);
    vertices.push_back(normal.x);
    vertices.push_back(normal.y);
    vertices.push_back(normal.z);

    // Add indices for the branch
    indices.push_back(startIndex);
    indices.push_back(startIndex + 1);

    // Recursive branching (left, right, and up branches)
    float angleOffset = glm::radians(30.0f + (rand() % 10 - 5)); // Randomize angle slightly
    int numBranches = 3; // Three branches: left, right, and up

    for (int i = 0; i < numBranches; ++i) {
        glm::vec3 newDirection = direction; // Start with the current direction

        // Apply rotation based on the branch index
        if (i == 0) {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleOffset, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around x-axis
            newDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 0.0f));
        }
        else if (i == 1) {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleOffset, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around y-axis
            newDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 0.0f));
        }
        else {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleOffset, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around z-axis
            newDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 0.0f));
        }

        // Random length factor to ensure it doesn't go to zero
        float lengthFactor = 0.6f + static_cast<float>(rand()) / RAND_MAX * 0.2f; // Random length factor
        generateTree(vertices, indices, end, newDirection, length * lengthFactor, depth - 1, maxDepth);
    }
}
unsigned int treeSetUp()
{
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (unsigned int)treeVertices.size() * sizeof(float), treeVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)treeIndices.size() * sizeof(float), treeIndices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    //sphereIndicesSize = indices.size();

    return VAO;
}
unsigned int generateCone(float radius, float height, int segments, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float angleStep = 2.0f * pi / segments;

    // Generate vertices and normals for the base circle
    for (int i = 0; i < segments; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Base circle vertex
        vertices.push_back(x);
        vertices.push_back(0.0f); // y = 0 for the base
        vertices.push_back(z);

        // Base circle normal (pointing downwards)
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
    }

    // Add the tip vertex of the cone
    int tipIndex = vertices.size() / 6; // Index of the tip
    vertices.push_back(0.0f);
    vertices.push_back(height); // Tip is at y = height
    vertices.push_back(0.0f);

    // Tip normal (pointing upwards)
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);

    // Add the center vertex of the base circle
    int baseCenterIndex = vertices.size() / 6; // Index of the base center
    vertices.push_back(0.0f);
    vertices.push_back(0.0f); // Center is at y = 0
    vertices.push_back(0.0f);

    // Base center normal (pointing downwards)
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);

    // Generate indices and normals for the sides
    for (int i = 0; i < segments; ++i) {
        int nextIndex = (i + 1) % segments;

        // Side triangle
        indices.push_back(i);
        indices.push_back(nextIndex);
        indices.push_back(tipIndex);

        // Calculate normals for the side vertices
        glm::vec3 v1(vertices[6 * i], vertices[6 * i + 1], vertices[6 * i + 2]);
        glm::vec3 v2(vertices[6 * nextIndex], vertices[6 * nextIndex + 1], vertices[6 * nextIndex + 2]);
        glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, glm::vec3(0.0f, height, 0.0f) - v1));

        // Add normals for the side vertices
        vertices.push_back(v1.x);
        vertices.push_back(v1.y);
        vertices.push_back(v1.z);
        vertices.push_back(normal.x);
        vertices.push_back(normal.y);
        vertices.push_back(normal.z);

        vertices.push_back(v2.x);
        vertices.push_back(v2.y);
        vertices.push_back(v2.z);
        vertices.push_back(normal.x);
        vertices.push_back(normal.y);
        vertices.push_back(normal.z);
    }

    // Generate indices for the base circle
    for (int i = 0; i < segments; ++i) {
        int nextIndex = (i + 1) % segments;

        indices.push_back(baseCenterIndex);
        indices.push_back(i);
        indices.push_back(nextIndex);
    }

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (unsigned int)vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    //sphereIndicesSize = indices.size();

    return VAO;
}