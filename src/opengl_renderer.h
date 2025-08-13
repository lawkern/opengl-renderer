/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

// NOTE: Renderer API.

typedef struct {
   GLuint VBO;
   GLuint VAO;
   GLuint Shader_Program;
} opengl_context;

#define INITIALIZE_OPENGL(Name) void Name(opengl_context *GL)
static INITIALIZE_OPENGL(Initialize_Opengl);

#define RESIZE_OPENGL(Name) void Name(int Width, int Height)
static RESIZE_OPENGL(Resize_Opengl);

#define RENDER_WITH_OPENGL(Name) void Name(opengl_context *GL)
static RENDER_WITH_OPENGL(Render_With_Opengl);

// NOTE: We want to support platforms like Windows, where gl functions beyond a
// few basics from OpenGL version 1 will be unavailable by default. For now, we
// just forward declare them here, and let the platforms GetProcAddress them as
// needed.
GLenum glGetError(void);
void glGenBuffers(GLsizei, GLuint *);
void glBindBuffer(GLenum, GLuint);
void glBufferData(GLenum, GLsizeiptr, const GLvoid *, GLenum);
GLuint glCreateShader(GLenum);
void glShaderSource(GLuint, GLsizei, const GLchar **, const GLint *);
void glCompileShader(GLuint);
void glGetShaderiv(GLuint, GLenum, GLint *);
void glGetShaderInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
GLuint glCreateProgram(void);
void glAttachShader(GLuint, GLuint);
void glLinkProgram(GLuint);
void glGetProgramiv(GLuint, GLenum, GLint *);
void glGetProgramInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
void glUseProgram(GLuint);
void glDeleteShader(GLuint);
void glVertexAttribPointer(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
void glEnableVertexAttribArray(GLuint);
void glGenVertexArrays(GLsizei, GLuint *);
void glBindVertexArray(GLuint);
void glDrawArrays(GLenum, GLint, GLsizei);
