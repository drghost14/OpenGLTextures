// code here
#include <raylib/raylib.h>
#include <glad/glad.h>

#include <iostream>
#include <string>
#include <vector>

#include <fstream>
#include <sstream>

#define WIDTH  1280
#define HEIGHT 720
using std::cout, std::string, std::ifstream, std::ostringstream, std::vector;

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

enum TEXTURE_PARAMETER
{
    TEXTURE_LINEAR  = 1,
    TEXTURE_NEAREST = 2
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
    vector<float>       vertex =
    {// vertices        tex cords
        -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
         0.5f, -0.5f,   1.0f, 0.0f, // bottom right
         0.5f,  0.5f,   1.0f, 1.0f, // top right
        -0.5f,  0.5f,   0.0f, 1.0f, // top left
    };

    vector<unsigned int> index =
    {
        0, 1, 2,
        0, 2, 3
    };
    SetConfigFlags (FLAG_WINDOW_RESIZABLE);
    InitWindow     (WIDTH, HEIGHT, "texture");

    ShaderClass  shaders;
    TextureClass texture;
    VaoClass         vao;
    BufferClass vbo, ebo;

    shaders.Initialize ("shaders/vertex.glsl", "shaders/fragment.glsl");
    texture.Initialize ("data/car.png", 0, TEXTURE_NEAREST);
    vao.Initialize ();
    vao.Bind       ();

    vbo.Initialize (GL_ARRAY_BUFFER, vertex.size () * sizeof (float), vertex.data ());
    ebo.Initialize (GL_ELEMENT_ARRAY_BUFFER, index.size () * sizeof (unsigned int), index.data ());

    GLsizei stride = sizeof (float) * 4;
    vao.SetData (0, 2, stride, ((void*) 0));
    vao.SetData (1, 2, stride, ((void*) ((sizeof (float) * 2))));

    while (!WindowShouldClose ())
    {
        BeginDrawing ();
            glClearColor (245/255.0f, 245/255.0f, 250/255.0f, 1.0f);
            glClear      (GL_COLOR_BUFFER_BIT);
            
            vao.Bind                  ();
            shaders.Use               ();
            shaders.SetUniformInteger ("uTexture", 0);
            glDrawElements            (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            vao.Unbind                ();

        EndDrawing   ();
    }
    CloseWindow ();
}
