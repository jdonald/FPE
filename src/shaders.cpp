#include "shaders.h"

extern cgShaderSet *currentShaderSet = NULL;

CGcontext cgShaderSet::cgContext = 0;
CGprofile cgShaderSet::vert_profile = CG_PROFILE_VP30;		// Profiles are latest supported by NVIDIA FX 5200
CGprofile cgShaderSet::frag_profile = CG_PROFILE_FP30;

void cgShaderSet::errorCallback() {
	CGerror error = cgGetError();
	//if( error ) {
	//	const char *listing = cgGetLastListing( cgContext );
	//	fprintf( stderr, "---------------------------------------------------------\n" );
	//	fprintf( stderr, "ERROR: Cg error\n" );
	//	fprintf( stderr, "%s\n\n", cgGetErrorString( error ) );
	//	fprintf( stderr, "%s\n", listing );
	//	fprintf( stderr, "---------------------------------------------------------\n" );
	//	fgetc( stdin );
	//	exit( 1 );
	//}
}

void cgShaderSet::load(char* vname, char* vfunc, char* fname, char* ffunc) {
	if ( cgContext == 0 ) {
		cgSetErrorCallback( cgShaderSet::errorCallback );
		cgContext = cgCreateContext();
	}

	// Select profiles
	vert_profile = cgGLGetLatestProfile( CG_GL_VERTEX );
	cgGLSetOptimalOptions( vert_profile );
	frag_profile = cgGLGetLatestProfile( CG_GL_FRAGMENT );
	cgGLSetOptimalOptions( frag_profile );

	vertex = cgCreateProgramFromFile( cgContext, CG_SOURCE, vname, vert_profile, vfunc, NULL );
	fragment = cgCreateProgramFromFile( cgContext, CG_SOURCE, fname, frag_profile, ffunc, NULL );
	cgGLLoadProgram( vertex );
	cgGLLoadProgram( fragment );
}

void cgShaderSet::enable(const camera &c) { enable(); updateMatrix(c); }

void cgShaderSet::enable() {
	currentShaderSet = this;
	cgGLEnableProfile( vert_profile );
	cgGLEnableProfile( frag_profile );
	cgGLBindProgram( vertex );		
	cgGLBindProgram( fragment );

	uModelViewProj	= cgGetNamedParameter( vertex, "uModelViewProj" );
	uModel			= cgGetNamedParameter( vertex, "uModel" );
	//uTime			= cgGetNamedParameter( vertex, "uTime" );

	uLightFrom		= cgGetNamedParameter( fragment, "uLightFrom" );
	uLightTo		= cgGetNamedParameter( fragment, "uLightTo" );
	uLightClr		= cgGetNamedParameter( fragment, "uLightClr" );
	uLightAngle		= cgGetNamedParameter( fragment, "uLightAngle" );
	uCameraFrom		= cgGetNamedParameter( fragment, "uCameraFrom" );	
	uTexture		= cgGetNamedParameter( fragment, "uTexture" );
	//uTimef			= cgGetNamedParameter( fragment, "uTime" );
}

void cgShaderSet::disableAll() {
	cgGLDisableProfile( vert_profile );
	cgGLDisableProfile( frag_profile );
}

void cgShaderSet::updateMatrix(const camera &c) {
	matrix m; m.loadModelView();
	cgGLSetStateMatrixParameter( uModel, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY );
	c.preMultView();
	cgGLSetStateMatrixParameter( uModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY ); 
	glLoadMatrixf(m.m);

	updateCameraPos(c.getPos()); //consolidated (might aswell do this too and simplify api)
}

void cgShaderSet::updateCameraPos(const vector3 &v) {
	cgGLSetParameter3f( uCameraFrom, v.x, v.y, v.z );
}

void cgShaderSet::updateLightPos(const vector3 &v) {
	cgGLSetParameter3f( uLightFrom, v.x, v.y, v.z );
}

void cgShaderSet::updateLightDir(const vector3 &v) {
	cgGLSetParameter3f( uLightTo, v.x, v.y, v.z );
}

void cgShaderSet::updateLightColor(const vector3 &c) {
	cgGLSetParameter3f( uLightClr, c.x, c.y, c.z);
}

void cgShaderSet::updateLightAngle(float a) {
	cgGLSetParameter1f( uLightAngle, cos(a) );
}

void cgShadowShaderSet::enable() {
	currentShaderSet = this;
	cgGLEnableProfile( vert_profile );
	cgGLEnableProfile( frag_profile );
	cgGLBindProgram( vertex );		
	cgGLBindProgram( fragment );

	uModelViewProj	= cgGetNamedParameter( vertex, "uModelViewProj" );
	uModel			= cgGetNamedParameter( vertex, "uModel" );
	uLightFrom		= cgGetNamedParameter( vertex, "uLightFrom" );

	//uCameraFrom		= cgGetNamedParameter( fragment, "uCameraFrom" ); //cg wont let me take this out?
}

void cgShadowShaderSet::enable(const light &l) { enable(); updateLightPos(l.pos); }
