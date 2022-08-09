#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

uint32_t VBO;
uint32_t VAO;
uint32_t EBO;
uint32_t shaderProgram;

int main() {
    float size = 0.5;
    float sin60 = sin(M_PI/3);
    float a = (1-sin60)/2;
    
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
        0,0,size*0.5, size*0.5,0,-size*sin60*0.5, -size*0.5,0,-size*sin60*0.5,
        0,-size*sin60,size, size*-0.25,-size*sin60,size*a, size*0.25,-size*sin60,size*a, size*-0.5,-size*sin60,-size*sin60, 0,-size*sin60,-size*sin60, size*0.5,-size*sin60,-size*sin60
    };

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



    for(int i=0;i<16*3;i++){
        indices[i*3]=tempind[i]*3;
        indices[i*3+1]=tempind[i]*3+1;
        indices[i*3+2]=tempind[i]*3+2;
    }

    const char* vertexShaderSrc = "#version 330 core\n"
    "layout (location=0) in vec3 pos;\n"
    "void main(){gl_Position=vec4(pos,1);}\0";

    const char* fragmentShaderSrc = "#version 330 core\n"
    "out vec4 color;\n"
    "void main(){color=vec4(1,1,0,1);}\0";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(600,600,"test",NULL,NULL);
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

    uint32_t fragmentShader = glCreateShader(GL_VERTEX_SHADER);
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

    glGetShaderiv(shaderProgram,GL_LINK_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(shaderProgram,512,NULL,infoLog);
        printf(infoLog);
        return 1;
    }

    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(triforce),triforce,GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);\
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        //if(glfwGetKey(window))

        //render
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES,0,3);
        //glDrawElements(GL_TRIANGLES,48,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        usleep(1000000/100); //100fps
    }




    return 0;
}
