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
  float points[] = {
    0.0f,  0.0f,  0.0f, // center
    0.5f,  0.0f,  0.0f,
    0.25f,  0.4330127f,  0.0f,
   -0.25f,  0.4330127f,  0.0f,
   -0.5f,  0.0f,  0.0f,
   -0.25f, -0.4330127f,  0.0f,
    0.25f, -0.4330127f,  0.0f,
    0.5f,  0.0f,  0.0f  // repeat first outer vertex to close the fan
  };
  // Set a vertex Buffer Object (VBO)
  GLuint vbo = 0;
  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, 8 * 3 * sizeof( float ), points, GL_STATIC_DRAW );

  // Set a Vertex Array Object (VAO)
  GLuint vao = 0;
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
  glEnableVertexAttribArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

  /*
  unsigned int VAO, VBO;
  glGenVertexArrays( 1, &VAO );
  glGenBuffers( 1, &VBO );
  // Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray( VAO );
  glBindBuffer( GL_ARRAY_BUFFER, VBO );
  glBufferData( GL_ARRAY_BUFFER, sizeof( points ), points, GL_STATIC_DRAW );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), ( void * ) 0 );
  glEnableVertexAttribArray( 0 );
  // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray( 0 );
  */

  // Load Vertex shader from file
  FILE* vs_file = fopen("shader2.vert", "r");
  if (!vs_file) {
    fprintf(stderr, "ERROR: Could not open shader.vert\n");
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



  // Create and compile the vertex shader
  GLuint vs = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( vs, 1, (const GLchar **) &vertex_shader, NULL );
  glCompileShader( vs );

  // After glCompileShader check for errors.
  int params = -1;
  glGetShaderiv( vs, GL_COMPILE_STATUS, &params );

  // On error, capture the log and print it.
  if ( GL_TRUE != params ) {
    int max_length    = 2048, actual_length = 0;
    char slog[2048];
    glGetShaderInfoLog( vs, max_length, &actual_length, slog );
    fprintf( stderr, "ERROR: Vertex Shader (index %u) did not compile.\n%s\n", vs, slog );
    return 1;
  }

  // Load Fragment shader from file
  FILE* fs_file = fopen("shader2.frag", "r");
  if (!fs_file) {
    fprintf(stderr, "ERROR: Could not open shader.frag\n");
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

  // Create and compile the fragment shader
  GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( fs, 1, (const GLchar **) &fragment_shader, NULL );
  glCompileShader( fs );

  // After glCompileShader check for errors.
  params = -1;
  glGetShaderiv( fs, GL_COMPILE_STATUS, &params );

  // On error, capture the log and print it.
  if ( GL_TRUE != params ) {
    int max_length    = 2048, actual_length = 0;
    char slog[2048];
    glGetShaderInfoLog( fs, max_length, &actual_length, slog );
    fprintf( stderr, "ERROR: Fragment Shader (index %u) did not compile.\n%s\n", fs, slog );
    return 1;
  }

  // Link the vertex and fragment shader into a shader program
  GLuint shader_program = glCreateProgram();
  glAttachShader( shader_program, vs );
  glAttachShader( shader_program, fs );
  glLinkProgram( shader_program);

  // Delete the now-unused shader objects
  glDeleteShader( vs );
  glDeleteShader( fs );
  
  
  double prev_s =  glfwGetTime(); // Set the initial 'previous time'.
  double title_countdown_s = 0.1;
  
  int time_loc = glGetUniformLocation( shader_program, "time" );
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

    // Put the shader program, and the VAO, in focus in OpenGL's state machine.
    glUseProgram( shader_program );
    
    // Update the 'time' uniform in the shader to the current time in seconds.
    glUniform1f( time_loc, (float)curr_s );
        
    glBindVertexArray( vao );

    // Draw the hexagon wireframe with center spokes.
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glLineWidth( 2.0f );
    glDrawArrays( GL_TRIANGLE_FAN, 0, 8 );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glfwSwapInterval( 0 ); // The value of 0 means "swap immediately".
    //glfwSwapInterval( 1 ); // Lock to normal refresh rate for your monitor.
    
    // Put the stuff we've been drawing onto the visible area.
    glfwSwapBuffers( window );
  }


  // Close OpenGL window, context, and any other GLFW resources.
  glfwTerminate();
  return 0;
}