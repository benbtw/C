#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// variable defines
bool escapeCanClose = false;

int screenWidth = 0;
int screenHeight = 0;

////////////////////////////////////////
// window handling
////////////////////////////////////////

// Function to handle GLFW errors
void errorCallback(int error, const char *description)
{
    (void)error; // supress warning may cause issues idk
    printf("Error: %s\n", description);
}

void rendererInit()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(1);
    }
}

void rendererPollEvents(GLFWwindow *window)
{
    glfwPollEvents();
    glfwWaitEventsTimeout(0.007); // fix weird input lag/choppy-ness

    if (escapeCanClose)
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void cleanRenderer(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow *initWindow(const char *title, int w, int h)
{
    // Set GLFW error callback
    glfwSetErrorCallback(errorCallback);

    // Set GLFW to use OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW windowed mode window and its OpenGL context
    screenWidth = w;
    screenHeight = h;
    GLFWwindow *window = glfwCreateWindow(w, h, title, NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        exit(1);
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    glfwSetWindowPos(window, 450, 250); // centering for me (1980x1020?)

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(1);
    }

    glViewport(0, 0, w, h);

    return window;
}

void clearRenderer()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool Running(GLFWwindow *window)
{
    if (glfwWindowShouldClose(window))
        return false;

    return true;
}

////////////////////////////////////////
// 2D Rendering
////////////////////////////////////////

// 2D Vertex shader source code
const char *vertexShaderSource2D = {
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "}\0"};

// 2D Fragment shader source code
const char *fragmentShaderSource2D = {
    "#version 330 core\n"
    "uniform vec4 shapeColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = shapeColor;\n"
    "}\0"};

// Image Fragment shader source code
const char *fragmentShaderSourceIMG = {
    "#version 330 core\n"
    "uniform sampler2D imageTexture;\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = texture(imageTexture, TexCoord);\n"
    "}\0"};

// 3d shaders
const char *vertexShaderSource3D = "#version 330 core\n"
                                   "layout (location = 0) in vec3 aPos;\n"
                                   "out vec3 FragPos;\n"
                                   "uniform mat4 model;\n"
                                   "uniform mat4 view;\n"
                                   "uniform mat4 projection;\n"
                                   "void main()\n"
                                   "{\n"
                                   "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                                   "    FragPos = vec3(model * vec4(aPos, 1.0));\n"
                                   "}\0";

const char *fragmentShaderSource3D = "#version 330 core\n"
                                     "out vec4 FragColor;\n"
                                     "uniform vec4 shapeColor;\n"
                                     "in vec3 FragPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "    FragColor = shapeColor;\n"
                                     "}\n\0";

// Function to compile shader
GLuint compileShader(GLenum shaderType, const char *shaderSource)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Shader compilation error: %s\n", infoLog);
    }

    return shader;
}

// Function to link shader program
GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking status
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader program linking error: %s\n", infoLog);
    }

    return shaderProgram;
}

////////////////////////////////////////
// 2D rendering functions
////////////////////////////////////////

// Function to draw a triangle
void drawTriangle(bTriangle triangle)
{
    // Create vertex and fragment shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource2D);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource2D);

    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    // delete shaders so no mem leak
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Calculate full width and full height
    float fullWidth = triangle.w;
    float fullHeight = triangle.h;

    // Convert screen coordinates to normalized device coordinates
    float normalizedX = (2.0f * triangle.x) / screenWidth - 1.0f;
    float normalizedY = 1.0f - (2.0f * triangle.y) / screenHeight;

    // Define the vertices of the triangle using normalized coordinates and scale
    float vertices[] = {
        // Vertex 1
        normalizedX - fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, 0.0f,
        // Vertex 2
        normalizedX + fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, 0.0f,
        // Vertex 3
        normalizedX, normalizedY + fullHeight / screenHeight, 0.0f};

    // Create and bind VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO and buffer data
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Set uniform color variable in the shader
    GLint triangleColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
    glUniform4f(triangleColorLocation, triangle.r, triangle.g, triangle.b, triangle.alpha);

    // Draw the triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Clean up
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
}

// Function to draw mutiple triangles
void drawTriangles(bTriangle *triangles, int size)
{
    // Create vertex and fragment shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource2D);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource2D);

    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    // delete shaders so no mem leak
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create and bind VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Loop through each triangle
    for (int i = 0; i < size; ++i)
    {
        bTriangle triangle = triangles[i];

        // Set uniform color variable in the shader
        GLint triangleColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
        glUniform4f(triangleColorLocation, triangle.r, triangle.g, triangle.b, triangle.alpha);

        // Calculate full width and full height
        float fullWidth = triangle.w;
        float fullHeight = triangle.h;

        // Convert screen coordinates to normalized device coordinates
        float normalizedX = (2.0f * triangle.x) / screenWidth - 1.0f;
        float normalizedY = 1.0f - (2.0f * triangle.y) / screenHeight;

        // Define the vertices of the triangle using normalized coordinates and scale
        float vertices[] = {
            // Vertex 1
            normalizedX - fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, 0.0f,
            // Vertex 2
            normalizedX + fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, 0.0f,
            // Vertex 3
            normalizedX, normalizedY + fullHeight / screenHeight, 0.0f};

        // Buffer data
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Draw the triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    // Clean up
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
}

// Function to draw a rectangle/square
void drawRect(bRect rect)
{
    // Create vertex and fragment shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource2D);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource2D);

    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    // delete shaders so no mem leak
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Normalize window coordinates to clip space coordinates
    float normalizedX = (2.0f * rect.x) / screenWidth - 1.0f;
    float normalizedY = 1.0f - (2.0f * rect.y) / screenHeight;

    // Calculate full width and full height
    float fullWidth = rect.w;
    float fullHeight = rect.h;

    // Define the vertices of the square using normalized coordinates and scale
    float vertices[] = {
        // Positions       // Texture Coords
        normalizedX + fullWidth / screenWidth, normalizedY + fullHeight / screenHeight, 0.0f,
        normalizedX + fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, 0.0f,
        normalizedX - fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, 0.0f,
        normalizedX - fullWidth / screenWidth, normalizedY + fullHeight / screenHeight, 0.0f};

    // Create and bind VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO and buffer data
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Set uniform color variable in the shader
    GLint squareColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
    glUniform4f(squareColorLocation, rect.r, rect.g, rect.b, rect.alpha);

    // Draw the square
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Draw using triangle strip with 4 vertices

    // Clean up
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
}

// Function to draw multiple rectangle/square
void drawRects(bRect *rects, int size)
{
    // Create vertex and fragment shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource2D);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource2D);

    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    // delete shaders so no mem leak
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create and bind VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // set color location
    GLint triangleColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");

    // Loop through each rectangle
    for (int i = 0; i < size; ++i)
    {
        bRect rect = rects[i];

        // Set uniform color variable in the shader
        glUniform4f(triangleColorLocation, rect.r, rect.g, rect.b, rect.alpha);

        // Normalize window coordinates to clip space coordinates
        float normalizedX = (2.0f * rect.x) / screenWidth - 1.0f;
        float normalizedY = 1.0f - (2.0f * rect.y) / screenHeight;

        // Calculate full width and full height (no longer full width turns out im a dumbass)
        float fullWidth = rect.w;
        float fullHeight = rect.h;

        // Define the vertices of the square using normalized coordinates and scale
        float vertices[] = {
            // Positions       // Texture Coords
            normalizedX + fullWidth / screenWidth, normalizedY + fullHeight / screenHeight, 0.0f,
            normalizedX + fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, 0.0f,
            normalizedX - fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, 0.0f,
            normalizedX - fullWidth / screenWidth, normalizedY + fullHeight / screenHeight, 0.0f};

        // Buffer data
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Draw the triangle
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    // Clean up
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
}

// Function to draw a point
void drawPoint(bPoint point)
{
    // Create vertex and fragment shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource2D);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource2D);

    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    // delete shaders so no mem leak
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Normalize window coordinates to clip space coordinates for the point
    float normalizedX = (2.0f * point.x) / screenWidth - 1.0f;
    float normalizedY = 1.0f - (2.0f * point.y) / screenHeight;

    // Define the vertex of the point using normalized coordinates
    float pointVertex[] = {
        normalizedX, normalizedY, 0.0f};

    // Create and bind VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO and buffer data for the point
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertex), pointVertex, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Set uniform color variable in the shader for the point
    GLint pointColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
    glUniform4f(pointColorLocation, point.r, point.g, point.b, point.alpha);

    // Draw the point
    glPointSize(point.size / 2);
    glDrawArrays(GL_POINTS, 0, 1); // Draw a single point

    // Clean up
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
}

// Function to draw multiple points
void drawPoints(bPoint *points, int size)
{
    // Create vertex and fragment shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource2D);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource2D);

    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    // delete shaders so no mem leak
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create and bind VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO for points
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Set uniform color variable in the shader for the points
    GLint pointColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");

    // Draw each point
    for (int i = 0; i < size; ++i)
    {
        bPoint point = points[i];

        // Normalize window coordinates to clip space coordinates for the point
        float normalizedX = (2.0f * point.x) / screenWidth - 1.0f;
        float normalizedY = 1.0f - (2.0f * point.y) / screenHeight;

        // Define the vertex of the point using normalized coordinates
        float pointVertex[] = {normalizedX, normalizedY, 0.0f};

        // Buffer data for the current point
        glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertex), pointVertex, GL_STATIC_DRAW);

        // Set uniform color variable for the current point
        glUniform4f(pointColorLocation, point.r, point.g, point.b, point.alpha);

        // Set point size for the current point
        glPointSize(point.size / 2);

        // Draw the current point
        glDrawArrays(GL_POINTS, 0, 1);
    }

    // Clean up
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
}

// Function to draw a line
void drawLine(bLine line)
{
    // Create vertex and fragment shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource2D);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource2D);

    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    // delete shaders so no mem leak
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Normalize window coordinates to clip space coordinates for the points
    float startX = (2.0f * line.x1) / screenWidth - 1.0f;
    float startY = 1.0f - (2.0f * line.y1) / screenHeight;
    float endX = (2.0f * line.x2) / screenWidth - 1.0f;
    float endY = 1.0f - (2.0f * line.y2) / screenHeight;

    // Define the vertices of the line segment using normalized coordinates
    float lineVertices[] = {
        startX, startY, 0.0f,
        endX, endY, 0.0f};

    // Create and bind VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO and buffer data for the line vertices
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Set uniform color variable in the shader for the line
    GLint lineColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
    glUniform4f(lineColorLocation, line.r, line.g, line.b, line.alpha); // You can use either startPoint or endPoint color

    // Draw the line
    glLineWidth(line.width);
    glDrawArrays(GL_LINES, 0, 2); // Draw the line with two vertices

    // Clean up
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
}

// Function to draw multiple lines
void drawLines(bLine *lines, int size)
{
    // Create vertex and fragment shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource2D);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource2D);

    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    // delete shaders so no mem leak
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create and bind VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO for the line vertices
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    for (int i = 0; i < size; ++i)
    {
        bLine line = lines[i];

        // Normalize window coordinates to clip space coordinates for the points
        float startX = (2.0f * line.x1) / screenWidth - 1.0f;
        float startY = 1.0f - (2.0f * line.y1) / screenHeight;
        float endX = (2.0f * line.x2) / screenWidth - 1.0f;
        float endY = 1.0f - (2.0f * line.y2) / screenHeight;

        // Define the vertices of the line segment using normalized coordinates
        float lineVertices[] = {
            startX, startY, 0.0f,
            endX, endY, 0.0f};

        // Buffer data for the line vertices
        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

        // Set uniform color variable in the shader for the line
        GLint lineColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
        glUniform4f(lineColorLocation, line.r, line.g, line.b, line.alpha); // You can use either startPoint or endPoint color

        // Draw the line
        glLineWidth(line.width);
        glDrawArrays(GL_LINES, 0, 2); // Draw the line with two vertices
    }

    // Clean up
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
}

void drawImage(bImage image)
{
    // Load the image
    int width, height, channels;
    unsigned char *image_data = stbi_load(image.fileName, &width, &height, &channels, 0);
    if (!image_data)
    {
        fprintf(stderr, "Failed to load image: %s\n", image.fileName);
        return;
    }

    // Generate OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Create vertex and fragment shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource2D);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSourceIMG);

    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    // delete shaders so no mem leak
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Calculate full width and full height
    float fullWidth = image.width;
    float fullHeight = image.height;

    // Convert screen coordinates to normalized device coordinates
    float normalizedX = (2.0f * image.x) / screenWidth - 1.0f;
    float normalizedY = 1.0f - (2.0f * image.y) / screenHeight;

    float texX1 = (float)image.sX / image.imageW;
    float texY1 = (float)image.sY / image.imageH;
    float texX2 = (float)(image.sX + image.sW) / image.imageW;
    float texY2 = (float)(image.sY + image.sH) / image.imageH;

    // Specify the vertex data
    float vertices[] = {
        // Positions       // Texture Coords
        normalizedX + fullWidth / screenWidth, normalizedY + fullHeight / screenHeight, texX2, texY1, // Top Right
        normalizedX + fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, texX2, texY2, // Bottom Right
        normalizedX - fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, texX1, texY2, // Bottom Left
        normalizedX - fullWidth / screenWidth, normalizedY + fullHeight / screenHeight, texX1, texY1  // Top Left
    };

    unsigned int indices[] = {
        0, 1, 3, // First triangle
        1, 2, 3  // Second triangle
    };

    // Create Vertex Buffer Object (VBO) and Vertex Array Object (VAO)
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Render the texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(shaderProgram);
    stbi_image_free(image_data);
}

void drawImages(bImage *images, int size)
{
    for (int i = 0; i < size; ++i)
    {
        bImage image = images[i];

        // Load the image
        int width, height, channels;
        unsigned char *image_data = stbi_load(image.fileName, &width, &height, &channels, 0);
        if (!image_data)
        {
            fprintf(stderr, "Failed to load image: %s\n", image.fileName);
            stbi_image_free(image_data);
            continue; // Skip to the next image if loading fails
        }

        // Generate OpenGL texture
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Create and compile the vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource2D, NULL);
        glCompileShader(vertexShader);

        // Create and compile the fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSourceIMG, NULL);
        glCompileShader(fragmentShader);

        // Create the shader program
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glBindFragDataLocation(shaderProgram, 0, "outColor");
        glLinkProgram(shaderProgram);
        glUseProgram(shaderProgram);

        // delete shaders so no mem leak
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Calculate full width and full height
        float fullWidth = image.width;
        float fullHeight = image.height;

        // Convert screen coordinates to normalized device coordinates
        float normalizedX = (2.0f * image.x) / screenWidth - 1.0f;
        float normalizedY = 1.0f - (2.0f * image.y) / screenHeight;

        float texX1 = (float)image.sX / image.imageW;
        float texY1 = (float)image.sY / image.imageH;
        float texX2 = (float)(image.sX + image.sW) / image.imageW;
        float texY2 = (float)(image.sY + image.sH) / image.imageH;

        // Specify the vertex data
        float vertices[] = {
            // Positions       // Texture Coords
            normalizedX + fullWidth / screenWidth, normalizedY + fullHeight / screenHeight, texX2, texY1, // Top Right
            normalizedX + fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, texX2, texY2, // Bottom Right
            normalizedX - fullWidth / screenWidth, normalizedY - fullHeight / screenHeight, texX1, texY2, // Bottom Left
            normalizedX - fullWidth / screenWidth, normalizedY + fullHeight / screenHeight, texX1, texY1  // Top Left
        };

        unsigned int indices[] = {
            0, 1, 3, // First triangle
            1, 2, 3  // Second triangle
        };

        // Create Vertex Buffer Object (VBO) and Vertex Array Object (VAO)
        GLuint VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Texture attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Render the texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Cleanup
        glDeleteTextures(1, &textureID);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteProgram(shaderProgram);
        stbi_image_free(image_data);
    }
}

////////////////////////////////////////
// 3D Rendering
////////////////////////////////////////

void drawCube(bCube cube, mat4 model, mat4 view, mat4 projection, bColor color)
{
    // create and add shaders to the program
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource3D);
    GLuint fragmentSahder = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource3D);

    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentSahder);

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentSahder);

    // Calculate the size of the cube in OpenGL coordinates based on the window size
    float cubeSizeX = cube.w / screenWidth * 2.0f;
    float cubeSizeY = cube.h / screenHeight * 2.0f;

    // Calculate the position of the cube in OpenGL coordinates
    float cubePosX = cube.x / screenWidth * 2.0f - 1.0f;
    float cubePosY = 1.0f - cube.y / screenHeight * 2.0f;
    float cubePosZ = cube.z; // Z-coordinate remains the same in OpenGL coordinates

    // Adjust the vertices based on the calculated position and size
    float vertices[] = {
        // Vertex positions
        cubePosX - cubeSizeX / 2, cubePosY - cubeSizeY / 2, cubePosZ - cubeSizeX / 2, // 0
        cubePosX + cubeSizeX / 2, cubePosY - cubeSizeY / 2, cubePosZ - cubeSizeX / 2, // 1
        cubePosX + cubeSizeX / 2, cubePosY + cubeSizeY / 2, cubePosZ - cubeSizeX / 2, // 2
        cubePosX - cubeSizeX / 2, cubePosY + cubeSizeY / 2, cubePosZ - cubeSizeX / 2, // 3
        cubePosX - cubeSizeX / 2, cubePosY - cubeSizeY / 2, cubePosZ + cubeSizeX / 2, // 4
        cubePosX + cubeSizeX / 2, cubePosY - cubeSizeY / 2, cubePosZ + cubeSizeX / 2, // 5
        cubePosX + cubeSizeX / 2, cubePosY + cubeSizeY / 2, cubePosZ + cubeSizeX / 2, // 6
        cubePosX - cubeSizeX / 2, cubePosY + cubeSizeY / 2, cubePosZ + cubeSizeX / 2  // 7
    };

    unsigned int indices[] = {
        0, 1, 2, // Front face
        2, 3, 0,
        1, 5, 6, // Right face
        6, 2, 1,
        4, 7, 6, // Back face
        6, 5, 4,
        0, 3, 7, // Left face
        7, 4, 0,
        3, 2, 6, // Top face
        6, 7, 3,
        0, 4, 5, // Bottom face
        5, 1, 0};

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // make each object take this in, also take in a bCube which handles angle, x, y, z
    glm_rotate_make(model, glm_rad(cube.angle), (vec3){cube.x, cube.y, cube.z});

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (GLfloat *)projection);

    GLint cubeColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
    glUniform4f(cubeColorLocation, color.r, color.g, color.b, color.a);

    glBindVertexArray(0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}

void drawCubes(bCube *cubes, int cubeSize, mat4 model, mat4 view, mat4 projection)
{
    // create and add shaders to the program
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource3D);
    GLuint fragmentSahder = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource3D);

    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentSahder);

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentSahder);

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    for (int i = 0; i < cubeSize; i++)
    {
        bCube cube = cubes[i];
        
        // Calculate the size of the cube in OpenGL coordinates based on the window size
        float cubeSizeX = cube.w / screenWidth * 2.0f;
        float cubeSizeY = cube.h / screenHeight * 2.0f;

        // Calculate the position of the cube in OpenGL coordinates
        float cubePosX = cube.x / screenWidth * 2.0f - 1.0f;
        float cubePosY = 1.0f - cube.y / screenHeight * 2.0f;
        float cubePosZ = cube.z; // Z-coordinate remains the same in OpenGL coordinates

        // Adjust the vertices based on the calculated position and size
        float vertices[] = {
            // Vertex positions
            cubePosX - cubeSizeX / 2, cubePosY - cubeSizeY / 2, cubePosZ - cubeSizeX / 2, // 0
            cubePosX + cubeSizeX / 2, cubePosY - cubeSizeY / 2, cubePosZ - cubeSizeX / 2, // 1
            cubePosX + cubeSizeX / 2, cubePosY + cubeSizeY / 2, cubePosZ - cubeSizeX / 2, // 2
            cubePosX - cubeSizeX / 2, cubePosY + cubeSizeY / 2, cubePosZ - cubeSizeX / 2, // 3
            cubePosX - cubeSizeX / 2, cubePosY - cubeSizeY / 2, cubePosZ + cubeSizeX / 2, // 4
            cubePosX + cubeSizeX / 2, cubePosY - cubeSizeY / 2, cubePosZ + cubeSizeX / 2, // 5
            cubePosX + cubeSizeX / 2, cubePosY + cubeSizeY / 2, cubePosZ + cubeSizeX / 2, // 6
            cubePosX - cubeSizeX / 2, cubePosY + cubeSizeY / 2, cubePosZ + cubeSizeX / 2  // 7
        };

        unsigned int indices[] = {
            0, 1, 2, // Front face
            2, 3, 0,
            1, 5, 6, // Right face
            6, 2, 1,
            4, 7, 6, // Back face
            6, 5, 4,
            0, 3, 7, // Left face
            7, 4, 0,
            3, 2, 6, // Top face
            6, 7, 3,
            0, 4, 5, // Bottom face
            5, 1, 0};

        glm_rotate_make(model, glm_rad(cube.angle), (vec3){cube.x, cube.y, cube.z});

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (GLfloat *)projection);

        GLint cubeColorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
        glUniform4f(cubeColorLocation, cube.r, cube.g, cube.b, cube.a);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}