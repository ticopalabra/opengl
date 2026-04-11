#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void error_callback_glfw(int error, const char* description) {
  fprintf( stderr, "GLFW ERROR: code %i msg: %s.\n", error, description );
}

int main( void ) {
  // Start OpenGL context and OS window using the GLFW helper library.
  printf( "Starting GLFW %s.\n", glfwGetVersionString() );

  // Register the error callback function that we wrote earlier.
  glfwSetErrorCallback( error_callback_glfw );

  if ( !glfwInit() ) {
    fprintf( stderr, "ERROR: could not start GLFW3.\n" );
    return 1;
  }

  // Request an OpenGL 4.1, core, context from GLFW.
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
  glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  //MSAA
  glfwWindowHint( GLFW_SAMPLES, 16 );

  // Create a window on the operating system, then tie the OpenGL context to it.
  /*
  GLFWwindow* window = glfwCreateWindow( 800, 600, "Hello Triangle", NULL, NULL );
  if ( !window ) {
    fprintf( stderr, "ERROR: Could not open window with GLFW3.\n" );
    glfwTerminate();
    return 1;
  }
  */
  // Set this to false to go back to windowed mode.
  //bool full_screen = true; 
  bool full_screen = false; 

  GLFWmonitor *mon = NULL;
  int win_w = 800, win_h = 600; // Our window dimensions, in pixels.

  if ( full_screen ) {
    mon = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode( mon );

    // Hinting these properties lets us use "borderless full screen" mode.
    glfwWindowHint( GLFW_RED_BITS, mode->redBits );
    glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
    glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
    glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );

    win_w = mode->width;  // Use our 'desktop' resolution for window size
    win_h = mode->height; // to get a 'full screen borderless' window.
  }

  GLFWwindow *window = glfwCreateWindow(
    win_w,
    win_h,
    "Extended OpenGL Init",
    mon,
    NULL
  );

  glfwMakeContextCurrent( window );
                                  
  // Start Glad, so we can call OpenGL functions.
  //int version_glad = gladLoadGL( glfwGetProcAddress );
  //int version_glad = gladLoadGL( (GLADloadproc) glfwGetProcAddress );
  
  int version_glad = gladLoadGL();
  if ( version_glad == 0 ) {
    fprintf( stderr, "ERROR: Failed to initialize OpenGL context.\n" );
    return 1;
  }
  //printf( "Loaded OpenGL %i.%i\n", GLAD_VERSION_MAJOR( version_glad ), GLAD_VERSION_MINOR( version_glad ) );
  

  // Try to call some OpenGL functions, and print some more version info.
  printf( "Renderer: %s.\n", glGetString( GL_RENDERER ) );
  printf( "OpenGL version supported %s.\n", glGetString( GL_VERSION ) );

  // Triangle definition  
  float t_points[] = {
   0.0f,  0.5f,  0.0f, // x,y,z of first point.
   0.5f, -0.5f,  0.0f, // x,y,z of second point.
  -0.5f, -0.5f,  0.0f  // x,y,z of third point.
  };


  // Hexagon definition with center plus 6 outer vertices, repeating the first outer vertex to close the fan.
  float hex_points[] = {
    0.0f,  0.0f,  0.0f, // center
    0.5f,  0.0f,  0.0f,
    0.25f,  0.4330127f,  0.0f,
   -0.25f,  0.4330127f,  0.0f,
   -0.5f,  0.0f,  0.0f,
   -0.25f, -0.4330127f,  0.0f,
    0.25f, -0.4330127f,  0.0f,
    0.5f,  0.0f,  0.0f  // repeat first outer vertex to close the fan
  };
  // Set a vertex Buffer Object (VBO) and Vertex Array Object (VAO) for the triangle.
  GLuint triangle_vbo = 0;
  GLuint triangle_vao = 0;
  glGenBuffers( 1, &triangle_vbo );
  glBindBuffer( GL_ARRAY_BUFFER, triangle_vbo );
  glBufferData( GL_ARRAY_BUFFER, 3 * 3 * sizeof( float ), t_points, GL_STATIC_DRAW );
  glGenVertexArrays( 1, &triangle_vao );
  glBindVertexArray( triangle_vao );
  glEnableVertexAttribArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, triangle_vbo );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
  glBindVertexArray( 0 );

  // Set a vertex Buffer Object (VBO) for the hexagon.
  GLuint  hex_vbo = 0;
  glGenBuffers( 1, &hex_vbo );
  glBindBuffer( GL_ARRAY_BUFFER, hex_vbo );
  glBufferData( GL_ARRAY_BUFFER, 8 * 3 * sizeof( float ), hex_points, GL_STATIC_DRAW );

  // Set a Vertex Array Object (VAO) for the hexagon.
  GLuint hex_vao = 0;
  glGenVertexArrays( 1, &hex_vao );
  glBindVertexArray( hex_vao );
  glEnableVertexAttribArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, hex_vbo );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
  glBindVertexArray( 0 );


  // Load Vertex shader from file
  FILE* vs_file = fopen("bouncing.vert", "r");
  if (!vs_file) {
    fprintf(stderr, "ERROR: Could not open bouncing.vert\n");
    glfwTerminate();
    return 1;
  }
  fseek(vs_file, 0, SEEK_END);
  long vs_size = ftell(vs_file);
  fseek(vs_file, 0, SEEK_SET);
  char* vertex_shader = (char*)malloc(vs_size + 1);
  fread(vertex_shader, 1, vs_size, vs_file);
  vertex_shader[vs_size] = '\0';
  fclose(vs_file);



  // Load and compile the bouncing vertex shader
  GLuint bouncing_vs = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( bouncing_vs, 1, (const GLchar **) &vertex_shader, NULL );
  glCompileShader( bouncing_vs );

  // After glCompileShader check for errors.
  int params = -1;
  glGetShaderiv( bouncing_vs, GL_COMPILE_STATUS, &params );

  // On error, capture the log and print it.
  if ( GL_TRUE != params ) {
    int max_length    = 2048, actual_length = 0;
    char slog[2048];
    glGetShaderInfoLog( bouncing_vs, max_length, &actual_length, slog );
    fprintf( stderr, "ERROR: Bouncing vertex shader did not compile.\n%s\n", slog );
    return 1;
  }

  // Load Fragment shader from file
  FILE* fs_file = fopen("bouncing.frag", "r");
  if (!fs_file) {
    fprintf(stderr, "ERROR: Could not open bouncing.frag\n");
    glfwTerminate();
    return 1;
  }
  fseek(fs_file, 0, SEEK_END);
  long fs_size = ftell(fs_file);
  fseek(fs_file, 0, SEEK_SET);
  char* fragment_shader = (char*)malloc(fs_size + 1);
  fread(fragment_shader, 1, fs_size, fs_file);
  fragment_shader[fs_size] = '\0';
  fclose(fs_file);

  // Load and compile the bouncing fragment shader
  GLuint bouncing_fs = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( bouncing_fs, 1, (const GLchar **) &fragment_shader, NULL );
  glCompileShader( bouncing_fs );

  // After glCompileShader check for errors.
  params = -1;
  glGetShaderiv( bouncing_fs, GL_COMPILE_STATUS, &params );

  // On error, capture the log and print it.
  if ( GL_TRUE != params ) {
    int max_length    = 2048, actual_length = 0;
    char slog[2048];
    glGetShaderInfoLog( bouncing_fs, max_length, &actual_length, slog );
    fprintf( stderr, "ERROR: Bouncing fragment shader did not compile.\n%s\n", slog );
    return 1;
  }

  // Link the bouncing shaders into a program
  GLuint bouncing_program = glCreateProgram();
  glAttachShader( bouncing_program, bouncing_vs );
  glAttachShader( bouncing_program, bouncing_fs );
  glLinkProgram( bouncing_program );

  glDeleteShader( bouncing_vs );
  glDeleteShader( bouncing_fs );
  free( vertex_shader );
  free( fragment_shader );

  // Load static vertex shader from file
  vs_file = fopen("static.vert", "r");
  if (!vs_file) {
    fprintf(stderr, "ERROR: Could not open static.vert\n");
    glfwTerminate();
    return 1;
  }
  fseek(vs_file, 0, SEEK_END);
  vs_size = ftell(vs_file);
  fseek(vs_file, 0, SEEK_SET);
  char* static_vertex_shader = (char*)malloc(vs_size + 1);
  fread(static_vertex_shader, 1, vs_size, vs_file);
  static_vertex_shader[vs_size] = '\0';
  fclose(vs_file);

  GLuint static_vs = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( static_vs, 1, (const GLchar **) &static_vertex_shader, NULL );
  glCompileShader( static_vs );

  params = -1;
  glGetShaderiv( static_vs, GL_COMPILE_STATUS, &params );
  if ( GL_TRUE != params ) {
    int max_length    = 2048, actual_length = 0;
    char slog[2048];
    glGetShaderInfoLog( static_vs, max_length, &actual_length, slog );
    fprintf( stderr, "ERROR: Static vertex shader did not compile.\n%s\n", slog );
    return 1;
  }

  fs_file = fopen("static.frag", "r");
  if (!fs_file) {
    fprintf(stderr, "ERROR: Could not open static.frag\n");
    glfwTerminate();
    return 1;
  }
  fseek(fs_file, 0, SEEK_END);
  fs_size = ftell(fs_file);
  fseek(fs_file, 0, SEEK_SET);
  char* static_fragment_shader = (char*)malloc(fs_size + 1);
  fread(static_fragment_shader, 1, fs_size, fs_file);
  static_fragment_shader[fs_size] = '\0';
  fclose(fs_file);

  GLuint static_fs = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( static_fs, 1, (const GLchar **) &static_fragment_shader, NULL );
  glCompileShader( static_fs );

  params = -1;
  glGetShaderiv( static_fs, GL_COMPILE_STATUS, &params );
  if ( GL_TRUE != params ) {
    int max_length    = 2048, actual_length = 0;
    char slog[2048];
    glGetShaderInfoLog( static_fs, max_length, &actual_length, slog );
    fprintf( stderr, "ERROR: Static fragment shader did not compile.\n%s\n", slog );
    return 1;
  }

  GLuint static_program = glCreateProgram();
  glAttachShader( static_program, static_vs );
  glAttachShader( static_program, static_fs );
  glLinkProgram( static_program );

  glDeleteShader( static_vs );
  glDeleteShader( static_fs );
  free( static_vertex_shader );
  free( static_fragment_shader );
  
  
  double prev_s =  glfwGetTime(); // Set the initial 'previous time'.
  double title_countdown_s = 0.1;
  
  int time_loc = glGetUniformLocation( bouncing_program, "time" );
  assert( time_loc > -1 ); // If this assert fails, check the name of the uniform variable in the shader, and that it's actually used in the shader code.

  while ( !glfwWindowShouldClose( window ) ) {
    double curr_s     = glfwGetTime();   // Get the current time.
    double elapsed_s  = curr_s - prev_s; // Work out the time elapsed over the last frame.
    prev_s            = curr_s;          // Set the 'previous time' for the next frame to use.

    // Print the FPS, but not every frame, so it doesn't flicker too much.
    title_countdown_s -= elapsed_s;
    if ( title_countdown_s <= 0.0 && elapsed_s > 0.0 ) {
      double fps        = 1.0 / elapsed_s;

      // Create a string and put the FPS as the window title.
      char tmp[256];
      sprintf( tmp, "FPS %.2lf", fps );
      glfwSetWindowTitle(window, tmp );
      title_countdown_s = 0.1;
    }

    // Update window events.
    glfwPollEvents();

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, 1);
    }

    // Check if the window resized.
    glfwGetWindowSize( window, &win_w, &win_h );
    // Update the viewport (drawing area) to fill the window dimensions.
    // try to comment out to se what happens
		glViewport( 0, 0, win_w, win_h );
    
    // Wipe the drawing surface clear.
    glClearColor( 0.6f, 0.6f, 0.8f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Draw the background triangle with static shaders.
    glUseProgram( static_program );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBindVertexArray( triangle_vao );
    glDrawArrays( GL_TRIANGLES, 0, 3 );

    // Draw the bouncing hexagon wireframe with bouncing shaders.
    glUseProgram( bouncing_program );
    glUniform1f( time_loc, (float)curr_s );
    glBindVertexArray( hex_vao );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glLineWidth( 2.0f );
    glDrawArrays( GL_TRIANGLE_FAN, 0, 8 );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glBindVertexArray( 0 );
    glfwSwapInterval( 0 ); // The value of 0 means "swap immediately".
    //glfwSwapInterval( 1 ); // Lock to normal refresh rate for your monitor.
    
    // Put the stuff we've been drawing onto the visible area.
    glfwSwapBuffers( window );
  }


  // Close OpenGL window, context, and any other GLFW resources.
  glfwTerminate();
  return 0;
}