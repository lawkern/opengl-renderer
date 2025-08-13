/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

static const GLchar *Vertex_Shader_Code =
   "#version 330 core\n"
   "layout(location = 0) in vec3 Vertex_Position;\n"
   "layout(location = 1) in vec4 Vertex_Color;\n"
   "out vec4 Fragment_Color;\n"
   "void main(void)\n"
   "{\n"
   "   Fragment_Color = Vertex_Color;\n"
   "   gl_Position = vec4(Vertex_Position.x, Vertex_Position.y, Vertex_Position.z, 1.0f);\n"
   "}\n";

static const GLchar *Fragment_Shader_Code =
   "#version 330 core\n"
   "in vec4 Fragment_Color;\n"
   "out vec4 Pixel_Color;\n"
   "void main(void)\n"
   "{\n"
   "   Pixel_Color = Fragment_Color;\n"
   "}\n";

static GLuint VBO;
static GLuint VAO;
static GLuint Vertex_Shader;
static GLuint Fragment_Shader;
static GLuint Shader_Program;

static INITIALIZE_OPENGL(Initialize_Opengl)
{
   GLint Shader_Status;
   GLchar Message[256];

   Vertex_Shader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(Vertex_Shader, 1, &Vertex_Shader_Code, 0);
   glCompileShader(Vertex_Shader);

   glGetShaderiv(Vertex_Shader, GL_COMPILE_STATUS, &Shader_Status);
   if(!Shader_Status)
   {
      glGetShaderInfoLog(Vertex_Shader, sizeof(Message), 0, Message);
      fprintf(stderr, "%s\n", Message);
      Assert(0);
   }

   Fragment_Shader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(Fragment_Shader, 1, &Fragment_Shader_Code, 0);
   glCompileShader(Fragment_Shader);

   glGetShaderiv(Fragment_Shader, GL_COMPILE_STATUS, &Shader_Status);
   if(!Shader_Status)
   {
      glGetShaderInfoLog(Fragment_Shader, sizeof(Message), 0, Message);
      fprintf(stderr, "%s\n", Message);
      Assert(0);
   }

   Shader_Program = glCreateProgram();
   glAttachShader(Shader_Program, Vertex_Shader);
   glAttachShader(Shader_Program, Fragment_Shader);
   glLinkProgram(Shader_Program);

   glGetProgramiv(Shader_Program, GL_LINK_STATUS, &Shader_Status);
   if(!Shader_Status)
   {
      glGetProgramInfoLog(Shader_Program, sizeof(Message), 0, Message);
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

   glGenBuffers(1, &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);

   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)sizeof(vec2));
   glEnableVertexAttribArray(1);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);

   return(true);
}

static RESIZE_OPENGL(Resize_Opengl)
{
   glViewport(0, 0, Width, Height);
}

static RENDER_WITH_OPENGL(Render_With_Opengl)
{
   glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);

   glUseProgram(Shader_Program);
   glBindVertexArray(VAO);
   glDrawArrays(GL_TRIANGLES, 0, 3);
}
