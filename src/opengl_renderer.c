/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

static void Resize_Opengl(int Width, int Height)
{
   glViewport(0, 0, Width, Height);
}

static void Render_With_Opengl(void)
{
   glClearColor(0, 0, 1, 1);
   glClear(GL_COLOR_BUFFER_BIT);
}
