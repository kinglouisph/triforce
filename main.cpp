#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

uint32_t VBO;
uint32_t VAO;
uint32_t EBO;
unsigned int shaderProgram;

const char* GetGLErrorStr(GLenum err)
{
    switch (err)
    {
    case GL_NO_ERROR:          return "No error";
    case GL_INVALID_ENUM:      return "Invalid enum";
    case GL_INVALID_VALUE:     return "Invalid value";
    case GL_INVALID_OPERATION: return "Invalid operation";
    case GL_STACK_OVERFLOW:    return "Stack overflow";
    case GL_STACK_UNDERFLOW:   return "Stack underflow";
    case GL_OUT_OF_MEMORY:     return "Out of memory";
    default:                   return "Unknown error";
    }
}

int main() {
    float size = 1;
    float sin30 = 0.5; 
    float cos30 = cos(M_PI/6);
    float a = 0.75/2;
    float b = cos30*sin(7*M_PI/6);
    float c = b/2;
    float d = (b+cos30)/2;
    uint32_t width=600;
    uint32_t height=600;

    float camxr = -M_PI/2;
    float camyr = 0.0f;
    const float camSpeed = 0.05f;
    const float camRotSpeed = 0.04f;
    
    /*float tetrahedron[4][3] = {{0,1,0},{0,-1,1},{-1,-1,-1},{1,-1,-1}};
    for (int i=0;i<4;i++){
        for (int ii=0;ii<3;ii++){
            tetrahedron[i][ii]=tetrahedron[i][ii]*size;
        }
    }
    float triforce[16][3];
    
    for (int i=0;i<4;i++){
        triforce[i][0]=tetrahedron[i][0];
        triforce[i][1]=tetrahedron[i][1]+size;
        triforce[i][2]=tetrahedron[i][2];
    }
    for (int i=0;i<4;i++){
        triforce[i+4][0]=tetrahedron[i][0];
        triforce[i+4][1]=tetrahedron[i][1]-size;
        triforce[i+4][2]=tetrahedron[i][2]+size;
    }
    for (int i=0;i<4;i++){
        triforce[i+8][0]=tetrahedron[i][0]-size;
        triforce[i+8][1]=tetrahedron[i][1]-size;
        triforce[i+8][2]=tetrahedron[i][2]-size;
    }
    for (int i=0;i<4;i++){
        triforce[i+12][0]=tetrahedron[i][0]+size;
        triforce[i+12][1]=tetrahedron[i][1]-size;
        triforce[i+12][2]=tetrahedron[i][2]-size;
    }*/

    float triforce[]={
        0,size,0,
        0,size*0.25f,size*cos30/2, size*-a,size*0.25f,size*c, size*a,size*0.25f,size*c,
        0,size*-0.5f,size*cos30, size*-a,size*-0.5f,size*d, size*a,size*-0.5f,size*d, size*-0.75f,size*-0.5f,size*b, 0,size*-0.5f,size*b, size*0.75f,size*-0.5f,size*b
    };
    
    /*for (int i=0;i<10;i++){
            triforce[i*3+2]=triforce[i*3+2];
    }*/

    unsigned int tempind[] = {
        0,1,2,
        0,1,3,
        0,2,3,
        1,2,3,
        1,4,5,
        1,4,6,
        1,5,6,
        4,5,6,
        2,5,7,
        2,5,8,
        2,7,8,
        5,7,8,
        3,6,8,
        3,6,9,
        3,8,9,
        6,8,9
    };

    unsigned int indices[9*16];



    for(int i=0;i<48;i++){
        indices[i*3]=tempind[i]*3;
        indices[i*3+1]=tempind[i]*3+1;
        indices[i*3+2]=tempind[i]*3+2;
        //printf("%d %f, %f, %f\n",i, triforce[indices[i*3+0]],triforce[indices[i*3+1]],triforce[indices[i*3+2]]);
    }

    for(int i=0;i<10;i++){
        //printf("%d %f, %f, %f\n",i, triforce[i*3],triforce[i*3+1],triforce[i*3+2]);
    }

    


    float color[] = {1.0f,1.0f,0.0f};




    const char* vertexShaderSrc = "#version 330 core\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "uniform float time;\n"
    "layout (location=0) in vec3 pos;\n"
    "void main(){\n"
    "   gl_Position=projection * view * model * vec4(pos.x,pos.y+time,pos.z,1);}\0";

    const char* fragmentShaderSrc = "#version 330 core\n"
    "out vec4 outColor;\n"
    "uniform vec3 color;\n"
    "void main(){outColor=vec4(color,1);}\0";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width,height,"test",NULL,NULL);
    if(window==NULL){
        printf("no window :(");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    if(GLEW_OK!=glewInit()){
        printf("glew init error :(");
        return 1;
    }

    glViewport(0,0,600,600);

    int success;
    char infoLog[512];
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSrc,NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
        printf(infoLog);
        return 1;
    }

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSrc,NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
        printf(infoLog);
        return 1;
    }



    shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
    
    
    if(!success){
        glGetProgramInfoLog(shaderProgram,512,NULL,infoLog);
        printf(infoLog);
        return 1;
    }


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int modelLocation = glGetUniformLocation(shaderProgram, "model");
    int viewLocation = glGetUniformLocation(shaderProgram, "view");
    int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    int colorLocation = glGetUniformLocation(shaderProgram, "color");
    int timeLocation = glGetUniformLocation(shaderProgram, "time");
    /*printf("%d", projectionLocation);
    fflush(stdout);*/


    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    

    
    projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);



    glm::vec3 camPos = glm::vec3(0.0f,0.0f,3.0f);
    glm::vec3 camDir = glm::vec3(1.0f,0.0f,0.0f);
    glm::vec3 camUp = glm::vec3(0.0f,1.0f,0.0f);






    
    
    glUseProgram(shaderProgram);
    
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(triforce),triforce,GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tempind), tempind, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    //glDisable(GL_CULL_FACE);
    
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        //if(glfwGetKey(window))
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camPos += camDir * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camPos -= glm::normalize(glm::cross(camDir, camUp)) * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camPos -= camDir * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camPos += glm::normalize(glm::cross(camDir, camUp)) * camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            camPos.y -= camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            camPos.y += camSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
            camyr += camRotSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
            camxr -= camRotSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
            camyr -= camRotSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            camxr += camRotSpeed;
        }


        //render
        model = glm::rotate(model, glm::radians(0.25f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::translate(model, glm::vec3(0.0f,(float)sin(time(NULL)/100),0.0f));
        //view = glm::translate(view, glm::vec3(-camx, -camy, camz));
        
        
        camDir.x = cos(camxr) * cos(camyr);
        camDir.y = sin(camyr);
        camDir.z = sin(camxr) * cos(camyr);
        camDir = glm::normalize(camDir);

        view = glm::lookAt(camPos, camPos + camDir, camUp);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        
        struct timeval tv;
        gettimeofday(&tv, NULL);
        float time = (float)tv.tv_usec + (float)(1000000 * (tv.tv_sec%(3600*24)));
        

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(colorLocation, color[0],color[1],color[2]);
        glUniform1f(timeLocation, (float)sin(time/1000000.0f)/10.0f);

        //printf("%f\n", (float)sin(time(NULL)));
        //fflush(stdout);

        
        

        //draw
        glClearColor(0.2f, 0.2f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES,48,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        usleep(1000000/100); //100fps
    }

    return 0;
}
