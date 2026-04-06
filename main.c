#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

int main( void ) {
  // Start OpenGL context and OS window using the GLFW helper library.
  if ( !glfwInit() ) {
    fprintf( stderr, "ERROR: could not start GLFW3.\n" );
    return 1;
  }

  // Request an OpenGL 4.1, core, context from GLFW.
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
  glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

  // Create a window on the operating system, then tie the OpenGL context to it.
  GLFWwindow* window = glfwCreateWindow( 800, 600, "Hello Triangle", NULL, NULL );
  if ( !window ) {
    fprintf( stderr, "ERROR: Could not open window with GLFW3.\n" );
    glfwTerminate();
    return 1;
  }
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
  float points[] = {
   0.0f,  0.5f,  0.0f, // x,y,z of first point.
   0.5f, -0.5f,  0.0f, // x,y,z of second point.
  -0.5f, -0.5f,  0.0f  // x,y,z of third point.
  };

  // Set a vertex Buffer Ojbect (VBO)
  GLuint vbo = 0;
  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( float ), points, GL_STATIC_DRAW );

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

  // Define the shaders
  const char* vertex_shader =
  "#version 410 core\n"
  "in vec3 vp;"
  "void main() {"
  "  gl_Position = vec4(vp, 1.0/(abs(vp.x))+abs(vp.y));"
  "}";

  //"  gl_Position = vec4( vp, 1.0 );"                     //centered
  //"  gl_Position = vec4(vp.x, vp.y + 1.0, vp.z, 1.0);"   //offset to the top
  //"  gl_Position = vec4(vp, 1.0/(vp.x+vp.y+vp.z));"      //perspective: fails as sum can be zero hence divide by zero!
  //"  gl_Position = vec4(vp, 1.0/(abs(vp.x))+abs(vp.y));" // perspective: it works better!

  const char* fragment_shader =
  "#version 410 core\n"
  "out vec4 frag_colour;"
  "void main() {"
  "  frag_colour = vec4( 0.5, 0.0, 0.5, 1.0 );"
  "}";

  // Create and compile the vertex shader
  GLuint vs = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( vs, 1, &vertex_shader, NULL );
  glCompileShader( vs );

  // Create and compile the fragment shader
  GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( fs, 1, &fragment_shader, NULL );
  glCompileShader( fs );

  // Link the vertex and fragment shader into a shader program
  GLuint shader_program = glCreateProgram();
  glAttachShader( shader_program, vs );
  glAttachShader( shader_program, fs );
  glLinkProgram( shader_program);

  while ( !glfwWindowShouldClose( window ) ) {
    // Update window events.
    glfwPollEvents();
    
    // Wipe the drawing surface clear.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Put the shader program, and the VAO, in focus in OpenGL's state machine.
    glUseProgram( shader_program );
    glBindVertexArray( vao );

    // Draw points 0-3 from the currently bound VAO with current in-use shader.
    glDrawArrays( GL_TRIANGLES, 0, 3 );
    
    // Put the stuff we've been drawing onto the visible area.
    glfwSwapBuffers( window );
  }

  // Delete the now-unused shader objects
  glDeleteShader( vs );
  glDeleteShader( fs );
  
  // Close OpenGL window, context, and any other GLFW resources.
  glfwTerminate();
  return 0;
}