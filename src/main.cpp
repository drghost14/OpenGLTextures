// code here
#include <raylib/raylib.h>
#include <glad/glad.h>

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

#define WIDTH  1280
#define HEIGHT 720
using std::cout, std::string, std::ifstream, std::ostringstream, std::vector, glm::value_ptr,
    glm::vec2, glm::vec3, glm::mat4, glm::normalize, glm::lookAt, glm::cross, glm::radians,
    glm::cos, glm::sin, glm::perspective, glm::translate;

enum TEXTURE_PARAMETER
{
    TEXTURE_LINEAR  = 1,
    TEXTURE_NEAREST = 2
};

class CameraClass
{
public:
    CameraClass  () {}
    ~CameraClass () {}

    void Initialize ()
    {
        cameraPos   = vec3 ( 0.0f,  0.0f,  3.0f);
        cameraUp    = vec3 ( 0.0f,  1.0f,  0.0f);
        cameraFront = vec3 ( 0.0f, -1.0f,  0.0f);
    }

    void Update     ()
    {
        HandleInput ();
        cameraView = lookAt (cameraPos, cameraPos + cameraFront, cameraUp);
    }

    mat4 GetCameraView () const
    {
        return cameraView;
    }
private:
    vec3 cameraPos;
    vec3 cameraUp;
    vec3 cameraFront;
    mat4 cameraView;

    const float cameraMovementSpeed = 0.5f;
    const float cameraRotationSpeed = 45.0f;

    float cameraYaw   = -90.0f;
    float cameraPitch =  0.0f;
    // private functions
    void HandleInput ()
    {
        float dt = GetFrameTime ();
        HandleMovement  (dt);
        HandleRotation  (dt);
        UpdateDirection ();
    }
    void HandleMovement (float dt) 
    {
        if (IsKeyDown (KEY_W)) cameraPos += cameraFront * cameraMovementSpeed * dt;
        if (IsKeyDown (KEY_S)) cameraPos -= cameraFront * cameraMovementSpeed * dt;
        if (IsKeyDown (KEY_A)) cameraPos -= normalize (cross (cameraFront, cameraUp)) * cameraMovementSpeed * dt;
        if (IsKeyDown (KEY_D)) cameraPos += normalize (cross (cameraFront, cameraUp)) * cameraMovementSpeed * dt;
    }
    void HandleRotation (float dt) 
    {
        if (IsKeyDown (KEY_RIGHT)) cameraYaw += cameraRotationSpeed * dt;
        if (IsKeyDown (KEY_LEFT )) cameraYaw -= cameraRotationSpeed * dt;
        if (IsKeyDown (KEY_UP  )) cameraPitch += cameraRotationSpeed * dt;
        if (IsKeyDown (KEY_DOWN)) cameraPitch -= cameraRotationSpeed * dt;

        if (cameraPitch >  89.0f) cameraPitch =  89.0f;
        if (cameraPitch < -89.0f) cameraPitch = -89.0f;
    }

    void UpdateDirection ()
    {
        float tempYaw   = radians (cameraYaw);
        float tempPitch = radians (cameraPitch);

        vec3 tempFront;
        tempFront.x = cos (tempYaw  ) * cos (tempPitch);
        tempFront.y = sin (tempPitch);
        tempFront.z = sin (tempYaw  ) * cos (tempPitch);

        cameraFront = normalize (tempFront);
    }

};

class TextureClass
{
public:
    TextureClass  () {}
    ~TextureClass ()
    {
        UnloadTexture (texture);
    }

    void Initialize (const char *filePath, int textureUnit, TEXTURE_PARAMETER textureParameter)
    {
        Image tempImage = LoadImage (filePath);
        ImageFlipVertical (&tempImage);

        texture = LoadTextureFromImage (tempImage);
        glActiveTexture (GL_TEXTURE0 + textureUnit);
        glBindTexture   (GL_TEXTURE_2D, texture.id);

        switch (textureParameter)
        {
            case TEXTURE_LINEAR :
                glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case TEXTURE_NEAREST:
                glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
        }
        glGenerateMipmap (GL_TEXTURE_2D);
        UnloadImage (tempImage);
    }
private:
    Texture2D texture;
};

// graphics pipeline
class ShaderClass
{
public:
    ShaderClass  () {}
    ~ShaderClass ()
    {
        glDeleteProgram (shaderProgram);
    }

    void Initialize (const char *vertexFilePath, const char *fragmentFilePath)
    {
        string vertexSTR   = LoadShaderSource (vertexFilePath  );
        string fragmentSTR = LoadShaderSource (fragmentFilePath);
        const char *vertexShaderSource   =    vertexSTR.c_str ();
        const char *fragmentShaderSource =  fragmentSTR.c_str ();

        unsigned int      vertexShader, fragmentShader;
        int             vertexSuccess, fragmentSuccess;
        char vertexInfoLog [512], fragmentInfoLog [512];

        vertexShader = glCreateShader (GL_VERTEX_SHADER);
        glShaderSource  (vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader (vertexShader);

        fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
        glShaderSource  (fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader (fragmentShader);

        glGetShaderiv (vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
        if (!vertexSuccess)
        {
            glGetShaderInfoLog (vertexShader, 512, NULL, vertexInfoLog);
            cout << "\nERROR: VERTEX SHADER:\n" << vertexInfoLog << "\n";
        }

        glGetShaderiv (fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
        if (!fragmentSuccess)
        {
            glGetShaderInfoLog (fragmentShader, 512, NULL, fragmentInfoLog);
            cout << "\nERROR: FRAGMENT SHADER:\n" << fragmentInfoLog << "\n";
        }

        shaderProgram = glCreateProgram ();
        glAttachShader (shaderProgram, vertexShader);
        glAttachShader (shaderProgram, fragmentShader);
        glLinkProgram  (shaderProgram);

        int linkSuccess;
        glGetProgramiv (shaderProgram, GL_LINK_STATUS, &linkSuccess);
        if (!linkSuccess)
        {
            char linkInfoLog [512];
            glGetProgramInfoLog (shaderProgram, 512, NULL, linkInfoLog);
            cout << "\nERROR: PROGRAM {LINKING}:\n" << linkInfoLog << "\n";
        }

        glDeleteShader (vertexShader);
        glDeleteShader (fragmentShader);
    }

    void Use ()
    {
        glUseProgram (shaderProgram);
    }

    void SetUniformInteger (const string &name, int value)
    {
        signed int location = glGetUniformLocation (shaderProgram, name.c_str ());
        if (location == -1)
        {
            cout << "\nWARNING: UNIFORM INTEGER:\n" << location << "\n";
        }
        glUniform1i (location, value);
    }

    void SetUniformMat4 (const string &name, const mat4 &value)
    {
        signed int location = glGetUniformLocation (shaderProgram, name.c_str ());
        if (location == -1)
        {
            cout << "\nWARNING: UNIFORM MATRIX 4:\n" << location << "\n";
        }
        glUniformMatrix4fv (location, 1, GL_FALSE, value_ptr (value));
    }
private:
    unsigned int shaderProgram;

    // private functions
    string LoadShaderSource (const char *filePath)
    {
        ifstream shaderSource (filePath);
        ostringstream buffer;
        buffer <<  shaderSource.rdbuf ();
        string shader =    buffer.str ();
        return shader;
    }
};

class VaoClass
{
public:
    VaoClass  () {}
    ~VaoClass ()
    {
        Unbind ();
        glDeleteVertexArrays (1, &vao);
    }
    void Initialize ()
    {
        glGenVertexArrays (1, &vao);
        if (vao == 0) cout << "\nERROR: VERTEX ARRAYS {GENERATION}\n";
    }
    void SetData (unsigned int index, int size, GLsizei stride, const void *pointer)
    {
        glVertexAttribPointer     (index, size, GL_FLOAT, GL_FALSE, stride, pointer);
        glEnableVertexAttribArray (index);
    }

    void Bind ()
    {
        glBindVertexArray (vao);
    }

    void Unbind ()
    {
        glBindVertexArray (0);
    }
private: 
    unsigned int vao;
};

class BufferClass
{
public:
    BufferClass  () {}  
    ~BufferClass ()
    {
        glDeleteBuffers (1, &buffer);
    }

    void Initialize (GLenum target, GLsizeiptr size, const void *data)
    {
        glGenBuffers (1, &buffer);
        glBindBuffer (target, buffer);
        glBufferData (target, size, data, GL_STATIC_DRAW);
    }
private:
    unsigned int buffer;
};

int main ()
{
    mat4 projection = perspective (radians (45.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
    mat4 model      = mat4 (1.0f);
    vec3 modelPos   = vec3 ( 0.0f, -1.0f,  -2.0f);
    model           = translate (model, modelPos);

    vector<float>       vertex =
    {// vertices                normals                 tex coords
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f, 0.0f, // bottom left  (z+) 0
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     1.0f, 0.0f, // bottom right (z+) 1
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // top right    (z+) 2
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f, 1.0f, // top left     (z+) 3

        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom left  (z-) 4
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     1.0f, 0.0f, // bottom right (z-) 5
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top right    (z-) 6
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f, 1.0f, // top left     (z-) 7

         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,     0.0f, 0.0f, // bottom left  (x+) 8
         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // bottom right (x+) 9
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // top right    (x+) 10
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // top left     (x+) 11

        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,     0.0f, 0.0f, // bottom left  (x-) 12
        -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // bottom right (x-) 13
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // top right    (x-) 14
        -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // top left     (x-) 15

        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f, 0.0f, // bottom left  (y+) 16
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // bottom right (y+) 17
         0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     1.0f, 1.0f, // top right    (y+) 18
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f, 1.0f, // top left     (y+) 19

        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f, 0.0f, // bottom left  (y-) 20
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // bottom right (y-) 21
         0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     1.0f, 1.0f, // top right    (y-) 22
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f, 1.0f, // top left     (y-) 23
    };

    vector<unsigned int> index =
    {
         0,  1,  2, // (z+)
         0,  2,  3, // (z+)

         4,  5,  6, // (z-)
         4,  6,  7, // (z-)

         8,  9, 10, // (x+)
         8, 10, 11, // (x+)

        12, 13, 14, // (x-)
        12, 14, 15, // (x-)

        16, 17, 18, // (y+)
        16, 18, 19, // (y+)

        20, 21, 22, // (y-)
        20, 22, 23  // (y-)
    };

    SetConfigFlags (FLAG_WINDOW_RESIZABLE);
    InitWindow     (WIDTH, HEIGHT, "texture");
    glEnable       (GL_DEPTH_TEST);
    glDisable      (GL_CULL_FACE);

    CameraClass   camera;
    TextureClass texture;

    ShaderClass  shaders;
    VaoClass         vao;
    BufferClass vbo, ebo;

    camera. Initialize ();
    texture.Initialize ("data/car.png", 0, TEXTURE_NEAREST);

    shaders.Initialize ("shaders/vertex.glsl", "shaders/fragment.glsl");
    vao.Initialize ();
    vao.Bind       ();

    vbo.Initialize (GL_ARRAY_BUFFER, vertex.size () * sizeof (float), vertex.data ());
    ebo.Initialize (GL_ELEMENT_ARRAY_BUFFER, index.size () * sizeof (unsigned int), index.data ());

    GLsizei stride = sizeof (float) * 8;
    vao.SetData (0, 3, stride, ((void*) 0));
    vao.SetData (1, 3, stride, ((void*) ((sizeof (float) * 3))));
    vao.SetData (2, 2, stride, ((void*) ((sizeof (float) * 6))));
    vao.Unbind  ();

    while (!WindowShouldClose ())
    {
        camera.Update ();
        BeginDrawing  ();
            glClearColor (245/255.0f, 245/255.0f, 250/255.0f, 1.0f);
            glClear      (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            vao.Bind                  ();
            shaders.Use               ();
            shaders.SetUniformInteger ("uTexture",   0);
            shaders.SetUniformMat4    ("model",      model);
            shaders.SetUniformMat4    ("cameraView", camera.GetCameraView ());
            shaders.SetUniformMat4    ("projection", projection);
            glDrawElements            (GL_TRIANGLES, index.size (), GL_UNSIGNED_INT, 0);
            vao.Unbind                ();

        EndDrawing    ();
    }
    CloseWindow ();
}
