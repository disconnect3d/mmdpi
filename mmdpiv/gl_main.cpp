﻿

#include "../libmmdpi/mmdpi.h"
//#include "../gl_xfile/gl_xfile.h"

#ifdef _DEBUG
	#pragma comment( lib, "../Debug/libmmdpi.lib" )
#else
	#pragma comment( lib, "../Release/libmmdpi.lib" )
#endif

//#ifdef _DEBUG
//	#pragma comment( lib, "../Debug/gl_xfile.lib" )
//#else
//	#pragma comment( lib, "../Release/gl_xfile.lib" )
//#endif


#include <iostream>
#include "GL/glut.h"


const int		_zoom_default_	= -1024 * 2 * 0.1f;// * 16;
float			_y_pos_			= 11 * 0.1f;
static mmdpi*	p = NULL;
int				_fps_			= 30;
int 			motion_flag = 0;
float			Zoom;
float			Rotate;

float			RotationAxis[ 3 ];

int				screen_width, screen_height;

int				Argc;
char**			Argv;

#include "fps.h"

Fps* fps = NULL;

void end( void );

void display( void )
{
	GLfloat light0pos[] = { 4.0, 16.0, -8.0, 1.0 };
	
	glEnable( GL_DEPTH_TEST );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	
	//glClearColor( 0.0, 0.0, 1.0, 1.0 );	//	bule
	//glClearColor( 0.0, 0.2, 0.0, 1.0 );	//	bule
	//glClearColor( 0.0, 0.0, 0.0, 1.0 );	//	black
	glClearColor( 1.0, 1.0, 1.0, 1.0 );	//	white
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glColor3d( 1.0, 0.0, 0.0 );

	glLightfv( GL_LIGHT0, GL_POSITION, light0pos );

	float diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	float ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );

	static float rot = 0.0f;

	// カメラ
	glMatrixMode( GL_MODELVIEW );
   
	glPushMatrix();
	{
	//	glRotatef( rot, 1, 0, 0 );
		//glRotatef( 3.14f, 0, 1, 0 );
		//glRotatef( rot, 0, 1, 0 );
		//glTranslatef( 0, 0, Zoom );

		//glutSolidTeapot( 0.5 );

		glRotatef( RotationAxis[ 0 ], 1, 0, 0 );
		glRotatef( RotationAxis[ 1 ], 0, 1, 0 );

		glScalef( 0.1f, 0.1f, 0.1f );
		p->draw();
	}
	glPopMatrix();

	//rot += 0.1f;

	glutSwapBuffers();
}

void keyboard( unsigned char key, int x, int y )
{
	switch( key )
	{
	case 'q':
	case 'Q':
	case '\033':  /* '\033' は ESC の ASCII コード */
		end();
		exit( 0 );
	case 'u':
		//	fps
		if( fps->get_fps() < 120 )
		{
			fps->set_fps( fps->get_fps() + 5 );
			p->set_fps( fps->get_fps() );
		}
		break;
	case 'd':
		if( fps->get_fps() > 10 )
		{
			fps->set_fps( fps->get_fps() - 5 );
			p->set_fps( fps->get_fps() );
		}
		break;
	return ;

	default:
		break;
	}
}

void sp_keyboard( int key, int x, int y )
{
	//static float	rotate	= 0;
	//static float	zoom	= _zoom_default_;

	switch( key )
	{
		case GLUT_KEY_PAGE_UP: 	_y_pos_ += 0.1f;	break;
		case GLUT_KEY_PAGE_DOWN:_y_pos_ -= 0.1f;	break;
		case GLUT_KEY_LEFT	:	Rotate += -0.05f;	break;		//	←
		case GLUT_KEY_UP	:	Zoom += +4;			break;		//	↑
		case GLUT_KEY_RIGHT	:	Rotate += +0.05f;	break;		//	→
		case GLUT_KEY_DOWN	:	Zoom += -4;			break;		//	↓
	}

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( sin( Rotate ) * Zoom, _y_pos_, cos( Rotate ) * Zoom, 0, _y_pos_, 0, 0, 1, 0 );
}

void idle( void )
{
	//glutPostRedisplay();
}

void timer( int value ) 
{
	//glutTimerFunc( fps->get_wait_time() * 1000.0f, timer, 0 );
	glutTimerFunc( 1000.0f / fps->get_fps(), timer, 0 );
	fps->draw();
	fps->update();
	
	if( motion_flag )
	{
		float	frame = 30.0f / fps->get_mfps();	//fps->get_dframe();
		//	フレームを進める関数
		//（MMD は１秒間に３０フレームがデフォルト）
		//	60fpsで実行の場合、0.5frame ずつフレームにたいしてモーションを進める
		( *p->get_vmd( 0 ) ) += frame;
		//( *p->get_vmd( 0 ) ) ++;

		if( p->get_vmd( 0 )->is_end() )
			exit( 0 );
	}
	
	mmdpiMatrix	matrix;
	static float dy = 3.14f;
	//dy += 0.01f;
	//matrix.rotation( 0, 1, 0, dy );
	//matrix.rotation( 0, 1, 0, 3.14f );
	p->set_bone_matrix( 0, matrix );

	glutPostRedisplay();
}

void resize( int w, int h )
{
	glViewport( 0, 0, w, h );
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	//gluLookAt( 0.0f, _y_pos_, _zoom_default_, 0, _y_pos_, 0, 0, 1, 0 );
	gluLookAt( sin( Rotate ) * Zoom, _y_pos_, cos( Rotate ) * Zoom, 0, _y_pos_, 0, 0, 1, 0 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	//	透視投影
	gluPerspective( 2.0f / 3, ( GLfloat )w / ( GLfloat )h, 1.0f, 65536.0f );
	//	正投影
	//glOrtho( -1.5, 1.5, -1.5, 1.5, -1.0, 1.0 );

	glMatrixMode( GL_MODELVIEW );

	screen_width = w;
	screen_height = h;
}

int		MousePushFlag = 0;
int		MousePosX = 0, MousePosY = 0;
void mouse_func( int button, int state, int x, int y )
{			
	switch( button )
	{
	case GLUT_LEFT_BUTTON:
		{
			switch( state )
			{
			case GLUT_DOWN:
				{
					MousePushFlag = 1;
					MousePosX = x;
					MousePosY = y;
				} break;
			case GLUT_UP:
				{
					MousePushFlag = 0;
				} break;
			}
		} break;
	}
}

//	押してないとき
void mouse_passive_func( int x, int y )
{
}

//	押してるとき
void mouse_motion( int x, int y )
{
	if( MousePushFlag )
	{
		float	dy = ( float )( x - MousePosX ) / 4.0f;
		float	dx = ( float )( y - MousePosY ) / 4.0f;

		MousePosX = x;
		MousePosY = y;

		RotationAxis[ 0 ] -= dx;
		RotationAxis[ 1 ] += dy;
	}
}

void init( void )
{
	p = new mmdpi();	
	if( p == NULL )
		exit( 0 );
	//p->set_physics_engine( 0 );
	
	// Test
	puts( Argv[ 1 ] );
	if( p->load( Argv[ 1 ] ) )
	//if( p->load( "udon/udon.pmx" ) )
	//if( p->load( "sakuya/sakuya.pmx" ) )
		exit( 0 );
	//p->load( "../Release/reimu/reimu.pmd"/*Argv[ 1 ]*/ );
	
	//	Sound
	if( Argc > 4 )
		system( Argv[ 4 ] );
	
	if( Argc > 2 )
	{
		motion_flag = 1;
		puts( Argv[ 2 ] );
		if( p->vmd_load( Argv[ 2 ] ) )
			motion_flag = 0;
	}
	//motion_flag = 1;
	//if( p->vmd_load( "vmd/nolifequeen.vmd" ) )
	//	motion_flag = 0;

	if( Argc > 3 )
	{
		_fps_ = atoi( Argv[ 3 ] );
		_fps_ = ( _fps_ < 6 || 480 < _fps_ )? 30.0f : _fps_ ;
	}
	fps = new Fps();
	fps->set_fps( _fps_ );

	p->set_fps( _fps_ );

	puts( "END Loading." );
}

void end( void )
{
	delete fps;
	delete p;
}

int main( int argc, char *argv[] )
{
	if( argc < 2 )
	{
		printf( "モデル、モーションを!\n\n" );
		return 0;
	}
	
	Zoom = _zoom_default_;
	Rotate = 0;

	Argc = argc;
	Argv = argv;
	
	glutInitWindowPosition( 200, 200 );
	glutInitWindowSize( 640, 480 );
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutCreateWindow( argv[ 0 ] );

	glutDisplayFunc( display );
	glutReshapeFunc( resize );
	
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( sp_keyboard );
	glutMouseFunc( mouse_func );
	glutPassiveMotionFunc( mouse_passive_func );
	glutMotionFunc( mouse_motion );

	init();

	//glutIdleFunc( idle );
	glutTimerFunc( 1000.0f / 30.0f , timer, 0 );

	glutMainLoop();

	end();

	return 0;
}