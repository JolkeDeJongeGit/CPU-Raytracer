// Template, BUAS version https://www.buas.nl/games
// IGAD/BUAS(NHTV)/UU - Jacco Bikker - 2006-2020

// Note:
// this version of the template uses SDL2 for all frame buffer interaction
// see: https://www.libsdl.org

#include "precomp.h"

#ifdef _MSC_VER
#pragma warning (disable : 4530) // complaint about exception handler
#pragma warning (disable : 4311) // pointer truncation from HANDLE to long
#endif

#define MAYBE_UNUSED(x) (void)(x)

//#define FULLSCREEN
#define ADVANCEDGL

#include "../game.h"

#include <fcntl.h>
#include <io.h>
#include <corecrt_math.h>
#include <SDL.h>
#include "surface.h"
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef ADVANCEDGL
#define GLEW_BUILD
extern "C"
{
#include "glew.h" 
}
#include "gl.h"
#include "wglext.h"
#endif

namespace Template { 

double timer::inv_freq = 1;

timer::timer(): start(get())
{
	init();
}

float timer::elapsed() const
{
	return static_cast<float>(static_cast<double>(get() - start) * inv_freq);
}

timer::value_type timer::get()
{
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	return c.QuadPart;
}

double timer::to_time(const value_type vt)
{
	return double(vt) * inv_freq;
}

void timer::reset()
{
	start = get();
}

void timer::init()
{
	LARGE_INTEGER f;
	QueryPerformanceFrequency(&f);
	inv_freq = 1000. / double(f.QuadPart);
}

void NotifyUser( char* s )
{
	HWND hApp = FindWindow(nullptr, TemplateVersion);
	MessageBox( hApp, s, "ERROR", MB_OK );
	if (IsDebuggerPresent())
	{
		__debugbreak();
	}
	exit( 0 );
}

}

using namespace Template;
using namespace std;

#ifdef ADVANCEDGL

PFNGLGENBUFFERSPROC glGenBuffers = 0;
PFNGLBINDBUFFERPROC glBindBuffer = 0;
PFNGLBUFFERDATAPROC glBufferData = 0;
PFNGLMAPBUFFERPROC glMapBuffer = 0;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = 0;
typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALFARPROC)(int);
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
unsigned int framebufferTexID[2];
GLuint fbPBO[2];
unsigned char* framedata = 0;

#endif

int ACTWIDTH, ACTHEIGHT;
static bool firstframe = true;

Surface* surface = 0;
Game* game = 0;
SDL_Window* window = 0;

#ifdef _MSC_VER
bool redirectIO()
{
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	AllocConsole();
	GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &coninfo );
	coninfo.dwSize.Y = 500;
	SetConsoleScreenBufferSize( GetStdHandle( STD_OUTPUT_HANDLE ), coninfo.dwSize );
	HANDLE h1 = GetStdHandle( STD_OUTPUT_HANDLE );
	int h2 = _open_osfhandle( (intptr_t)h1, _O_TEXT );
	FILE* fp = _fdopen( h2, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	h1 = GetStdHandle( STD_INPUT_HANDLE ), h2 = _open_osfhandle( (intptr_t)h1, _O_TEXT );
	fp = _fdopen( h2, "r" ), *stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	h1 = GetStdHandle( STD_ERROR_HANDLE ), h2 = _open_osfhandle( (intptr_t)h1, _O_TEXT );
	fp = _fdopen( h2, "w" ), *stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	ios::sync_with_stdio();
    FILE* stream;
    if ((stream = freopen("CON", "w", stdout)) == NULL)
        return false;
    if ((stream = freopen("CON", "w", stderr)) == NULL)
        return false;
    return true;
}
#endif

#ifdef ADVANCEDGL

bool createFBtexture()
{
	glGenTextures( 2, framebufferTexID );
	if (glGetError()) return false;
	for ( int i = 0; i < 2; i++ ) 
	{
		glBindTexture( GL_TEXTURE_2D, framebufferTexID[i] );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ScreenWidth, ScreenHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );
		glBindTexture(GL_TEXTURE_2D, 0);
		if (glGetError()) return false;
	}
	const int sizeMemory = 4 * ScreenWidth * ScreenHeight;
	glGenBuffers( 2, fbPBO );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[0] );	
	glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, sizeMemory, NULL, GL_STREAM_DRAW_ARB );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[1] );	
	glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, sizeMemory, NULL, GL_STREAM_DRAW_ARB );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	glBindTexture( GL_TEXTURE_2D, framebufferTexID[0] );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[0] );
	framedata = (unsigned char*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
	if (!framedata) return false;
	memset( framedata, 0, ScreenWidth * ScreenHeight * 4 );
	return (glGetError() == 0);
}

bool init()
{
	fbPBO[0] = fbPBO[1] = (GLuint)-1;
	glGenBuffers =  (PFNGLGENBUFFERSPROC)wglGetProcAddress( "glGenBuffersARB" );
	glBindBuffer =  (PFNGLBINDBUFFERPROC)wglGetProcAddress( "glBindBufferARB" );
	glBufferData =  (PFNGLBUFFERDATAPROC)wglGetProcAddress( "glBufferDataARB" );
	glMapBuffer  =  (PFNGLMAPBUFFERPROC)wglGetProcAddress( "glMapBufferARB" );
	glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress( "glUnmapBufferARB" );
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
	if ((!glGenBuffers) || (!glBindBuffer) || (!glBufferData) || (!glMapBuffer) || (!glUnmapBuffer)) return false;
	if (glGetError()) return false;
	glViewport( 0, 0, ScreenWidth, ScreenHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
 	glOrtho( 0, 1, 0, 1, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	if (!createFBtexture()) return false;
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	if (wglSwapIntervalEXT) wglSwapIntervalEXT( 0 );
	surface = new Surface( ScreenWidth, ScreenHeight, 0, ScreenWidth );
	return true;
}

void swap()
{
	static int index = 0;
	int nextindex;
	glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB );
	glBindTexture( GL_TEXTURE_2D, framebufferTexID[index] );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[index] );
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, ScreenWidth, ScreenHeight, GL_BGRA, GL_UNSIGNED_BYTE, 0 ); 
    nextindex = (index + 1) % 2;
	index = (index + 1) % 2;
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[nextindex] );	
	framedata = (unsigned char*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
    glColor3f( 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
	glNormal3f( 0, 0, 1 );
	glTexCoord2f( 0.0f, 0.0f );
	glVertex2f  ( 0.0f, 1.0f );
	glTexCoord2f( 1.0f, 0.0f );
	glVertex2f  ( 1.0f, 1.0f );
	glTexCoord2f( 1.0f, 1.0f );
	glVertex2f  ( 1.0f, 0.0f );
	glTexCoord2f( 0.0f, 1.0f );
	glVertex2f  ( 0.0f, 0.0f );
    glEnd();
    glBindTexture( GL_TEXTURE_2D, 0 );	
   	SDL_GL_SwapWindow( window ); 
}

#endif

int main( int argc, char **argv ) 
{
	MAYBE_UNUSED(argc);
	MAYBE_UNUSED(argv);
#ifdef _MSC_VER
    if (!redirectIO())
        return 1;
#endif
	printf( "application started.\n" );
	SDL_Init( SDL_INIT_VIDEO );
#ifdef ADVANCEDGL
#ifdef FULLSCREEN
	window = SDL_CreateWindow(TemplateVersion, 100, 100, ScreenWidth, ScreenHeight, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL );
#else
	window = SDL_CreateWindow(TemplateVersion, 100, 100, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL );
#endif
	SDL_GL_CreateContext( window);
	init();
	ShowCursor( false );
#else
#ifdef FULLSCREEN
	window = SDL_CreateWindow(TemplateVersion, 100, 100, ScreenWidth, ScreenHeight, SDL_WINDOW_FULLSCREEN );
#else
	window = SDL_CreateWindow(TemplateVersion, 100, 100, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN );
#endif
	surface = new Surface( ScreenWidth, ScreenHeight );
	surface->Clear( 0 );
	SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	SDL_Texture* frameBuffer = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, ScreenWidth, ScreenHeight );
#endif
	int exitapp = 0;
	game = new Game();
	game->SetTarget( surface );
	timer t;
	t.reset();
	while (!exitapp) 
	{
	#ifdef ADVANCEDGL
		swap();
		surface->SetBuffer( (Pixel*)framedata );
	#else
		void* target = 0;
		int pitch;
		SDL_LockTexture( frameBuffer, NULL, &target, &pitch );
		if (pitch == (surface->GetWidth() * 4))
		{
			memcpy( target, surface->GetBuffer(), ScreenWidth * ScreenHeight * 4 );
		}
		else
		{
			unsigned char* targetBuffer = static_cast<unsigned char*>(target);
			for( int i = 0; i < ScreenHeight; i++ )
			{
				memcpy(targetBuffer, surface->GetBuffer() + i * ScreenWidth, ScreenWidth * 4 );
				targetBuffer += pitch;
			}
		}
		SDL_UnlockTexture( frameBuffer );
		SDL_RenderCopy( renderer, frameBuffer, nullptr, nullptr);
		SDL_RenderPresent( renderer );
	#endif
		if (firstframe)
		{
			game->Init();
			firstframe = false;
		}
        // calculate frame time and pass it to game->Tick
        float elapsedTimeSeconds = t.elapsed();
        t.reset();
        game->Tick( elapsedTimeSeconds / 1000.0f );
        // event loop
		SDL_Event event;
		while (SDL_PollEvent( &event )) 
		{
			switch (event.type)
			{
			case SDL_QUIT:
				exitapp = 1;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) 
				{
					exitapp = 1;
					// find other keys here: http://sdl.beuc.net/sdl.wiki/SDLKey
				}
				game->KeyDown( event.key.keysym.scancode );
				break;
			case SDL_KEYUP:
				//game->KeyUp( event.key.keysym.scancode );
				break;
			case SDL_MOUSEMOTION:
				//game->MouseMove( event.motion.xrel, event.motion.yrel );
				break;
			case SDL_MOUSEBUTTONUP:
				//game->MouseUp( event.button.button );
				break;
			case SDL_MOUSEBUTTONDOWN:
				//game->MouseDown( event.button.button );
				break;
			default:
				break;
			}
		}
	}
	game->Shutdown();

	delete game;
	delete surface;

	SDL_Quit();
	return 0;
}