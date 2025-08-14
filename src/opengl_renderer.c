/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

static char *Opengl_Error_Names[] =
{
   [GL_NO_ERROR]          = "GL_NO_ERROR",
   [GL_INVALID_ENUM]      = "GL_INVALID_ENUM",
   [GL_INVALID_VALUE]     = "GL_INVALID_VALUE",
   [GL_INVALID_OPERATION] = "GL_INVALID_OPERATION",
   [GL_STACK_OVERFLOW]    = "GL_STACK_OVERFLOW",
   [GL_STACK_UNDERFLOW]   = "GL_STACK_UNDERFLOW",
   [GL_OUT_OF_MEMORY]     = "GL_OUT_OF_MEMORY",
   [GL_TABLE_TOO_LARGE]   = "GL_TABLE_TOO_LARGE",
};

#define GL_CHECK Check_For_Opengl_Errors(__FILE__, __LINE__)
static inline void Check_For_Opengl_Errors(char *Path, int Line)
{
   GLenum Error = glGetError();
   while(Error != GL_NO_ERROR)
   {
      Assert(Error < Array_Count(Opengl_Error_Names));
      fprintf(stderr, "%s:%d: error: %s\n", Path, Line, Opengl_Error_Names[Error]);
      Error = glGetError();
   }
}

static INITIALIZE_OPENGL(Initialize_Opengl)
{
   GLint Shader_Status;
   GLchar Message[256];

   // TODO: Better shader managment.
   const GLchar *Vertex_Shader_Code = Read_Entire_File("shaders/basic.vert");
   Assert(Vertex_Shader_Code);

   GLuint Vertex_Shader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(Vertex_Shader, 1, &Vertex_Shader_Code, 0);
   glCompileShader(Vertex_Shader);

   glGetShaderiv(Vertex_Shader, GL_COMPILE_STATUS, &Shader_Status);
   if(!Shader_Status)
   {
      glGetShaderInfoLog(Vertex_Shader, sizeof(Message), 0, Message);
      fprintf(stderr, "%s\n", Message);
      Assert(0);
   }

   // TODO: Better shader managment.
   const GLchar *Fragment_Shader_Code = Read_Entire_File("shaders/basic.frag");
   Assert(Fragment_Shader_Code);

   GLuint Fragment_Shader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(Fragment_Shader, 1, &Fragment_Shader_Code, 0);
   glCompileShader(Fragment_Shader);

   glGetShaderiv(Fragment_Shader, GL_COMPILE_STATUS, &Shader_Status);
   if(!Shader_Status)
   {
      glGetShaderInfoLog(Fragment_Shader, sizeof(Message), 0, Message);
      fprintf(stderr, "%s\n", Message);
      Assert(0);
   }

   GL->Shader_Program = glCreateProgram();
   glAttachShader(GL->Shader_Program, Vertex_Shader);
   glAttachShader(GL->Shader_Program, Fragment_Shader);
   glLinkProgram(GL->Shader_Program);

   glGetProgramiv(GL->Shader_Program, GL_LINK_STATUS, &Shader_Status);
   if(!Shader_Status)
   {
      glGetProgramInfoLog(GL->Shader_Program, sizeof(Message), 0, Message);
      fprintf(stderr, "%s\n", Message);
      Assert(0);
   }

   glDeleteShader(Vertex_Shader);
   glDeleteShader(Fragment_Shader);

   typedef struct {
      vec2 Position;
      vec4 Color;
   } vertex;

   vertex Vertices[] =
      {
         {{-0.5f, -0.5f}, {1, 1, 0, 1}},
         {{+0.5f, -0.5f}, {0, 1, 1, 1}},
         {{+0.0f, +0.5f}, {1, 0, 1, 1}},
      };

   glGenBuffers(1, &GL->VBO);
   glBindBuffer(GL_ARRAY_BUFFER, GL->VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

   glGenVertexArrays(1, &GL->VAO);
   glBindVertexArray(GL->VAO);

   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)sizeof(vec2));
   glEnableVertexAttribArray(1);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

static RESIZE_OPENGL(Resize_Opengl)
{
   glViewport(0, 0, Width, Height);
}

static RENDER_WITH_OPENGL(Render_With_Opengl)
{
   glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);

   glUseProgram(GL->Shader_Program);
   glBindVertexArray(GL->VAO);
   glDrawArrays(GL_TRIANGLES, 0, 3);
}
