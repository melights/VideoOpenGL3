#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shaders/shader.hpp>


cv::VideoCapture cap("../../data/scale/left.avi");

GLuint programID;
GLuint MatrixID;
GLuint TextureID;
GLuint Texture1;

GLuint videocolorbuffer;
GLuint videocubebuffer;


GLFWwindow* initWindow(const int resX, const int resY)
{
    if(!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(resX, resY, "Video", NULL, NULL);

    if(window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();

    }

    glClearColor(0.0, 0.0, 0.4, 1.0);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
    glEnable(GL_CULL_FACE);
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    // Init Video's VBO buffer
    static const GLfloat v_vertex_buffer_data[] = {
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f,  -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  -1.0f, 0.0f,
    };

    static const GLfloat v_uv_buffer_data[] = {
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f,  0.0f,
            1.0f, 1.0f,
            0.0f,  1.0f,
            0.0f,  0.0f,
    };

    glGenBuffers(1, &videocubebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, videocubebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v_vertex_buffer_data), v_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &videocolorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, videocolorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v_uv_buffer_data), v_uv_buffer_data, GL_STATIC_DRAW);







	// Create and compile our GLSL program from the shaders
   	programID = LoadShaders( "../Shaders/TransformVertexShader.vertexshader", "../Shaders/TextureFragmentShader.fragmentshader" );
	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

    MatrixID = glGetUniformLocation(programID, "MVP");
    TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    
    glGenTextures(1, &Texture1);

    return window;
}

void DrawVideo(){
    cv::Mat frame;
    cap.grab();

    cap.retrieve(frame);

    glActiveTexture(GL_TEXTURE0);

    //Bind Video to Texture1
    cv::waitKey(30);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Texture1);
    
    glUniform1i(TextureID, 0);
    glm::mat4 MVP = glm::mat4(1.0);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    glDisable(GL_DEPTH_TEST);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, videocubebuffer);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, videocolorbuffer);
    glVertexAttribPointer(
        1,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, 6 );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

}



void display( GLFWwindow* window )
{
    while(!glfwWindowShouldClose(window))
    {
        // Scale to window size
        GLint windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        // Draw stuff

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

         DrawVideo();

        glfwPollEvents();
        glfwSwapBuffers(window);

    }
}


int main(){


    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    GLFWwindow* window = initWindow(640, 480);
    if( NULL != window )
    {
        display( window );
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


