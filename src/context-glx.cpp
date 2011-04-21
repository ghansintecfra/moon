/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * context-glx.cpp
 *
 * Copyright 2010 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 *
 */

#include <config.h>

#define __MOON_GLX__

#include "context-glx.h"

#define GETPROCADDR(type, name)						\
	(name) = (type) glXGetProcAddressARB ((const GLubyte*) # name)

namespace Moonlight {

GLXContext::GLXContext (GLXSurface *surface) : OpenGLContext (surface)
{
	dpy = surface->GetDisplay ();
	drawable = surface->GetGLXDrawable ();
	vid = surface->GetVisualID ();
	ctx = (_XxGLXContext) 0;
}

GLXContext::~GLXContext ()
{
	if (ctx)
		glXDestroyContext (dpy, ctx);
}

bool
GLXContext::Initialize ()
{
	XVisualInfo templ, *visinfo;
	int         n;

	templ.visualid = vid;
	visinfo = XGetVisualInfo (dpy, VisualIDMask, &templ, &n);
	if (visinfo == NULL) {
		g_warning ("Found no visuals matching VisualID 0x%x, "
			   "disabling GLX", (int) vid);
		return false;
	}

	GLXSurface::X11ErrorTrapPush (dpy);
	ctx = glXCreateContext (dpy, visinfo, 0, True);
	XFree (visinfo);

	if (GLXSurface::X11ErrorTrapPop (dpy) != Success) {
		g_warning ("Failed to create GLX context for VisualID: 0x%x",
			   (int) vid);
		return false;
	}

	GLXSurface::X11ErrorTrapPush (dpy);
	glXMakeCurrent (dpy, drawable, ctx);
	if (GLXSurface::X11ErrorTrapPop (dpy) != Success) {
		g_warning ("Failed to make GLX context current for drawable: "
			   "0x%x", (int) drawable);
		return false;
	}

	if (!OpenGLContext::Initialize ())
		return false;

	GETPROCADDR (PFNGLCREATESHADERPROC, glCreateShader);
	GETPROCADDR (PFNGLSHADERSOURCEPROC, glShaderSource);
	GETPROCADDR (PFNGLCOMPILESHADERPROC, glCompileShader);
	GETPROCADDR (PFNGLGETSHADERIVPROC, glGetShaderiv);
	GETPROCADDR (PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
	GETPROCADDR (PFNGLDELETESHADERPROC, glDeleteShader);
	GETPROCADDR (PFNGLCREATEPROGRAMPROC, glCreateProgram);
	GETPROCADDR (PFNGLATTACHSHADERPROC, glAttachShader);
	GETPROCADDR (PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation);
	GETPROCADDR (PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
	GETPROCADDR (PFNGLUNIFORM4FPROC, glUniform4f);
	GETPROCADDR (PFNGLUNIFORM4FVPROC, glUniform4fv);
	GETPROCADDR (PFNGLUNIFORM1IPROC, glUniform1i);
	GETPROCADDR (PFNGLLINKPROGRAMPROC, glLinkProgram);
	GETPROCADDR (PFNGLUSEPROGRAMPROC, glUseProgram);
	GETPROCADDR (PFNGLDELETEPROGRAMPROC, glDeleteProgram);
	GETPROCADDR (PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
	GETPROCADDR (PFNGLENABLEVERTEXATTRIBARRAYARBPROC,
		     glEnableVertexAttribArray);
	GETPROCADDR (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC,
		     glDisableVertexAttribArray);
	GETPROCADDR (PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
	GETPROCADDR (PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers);
	GETPROCADDR (PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
	GETPROCADDR (PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
	GETPROCADDR (PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
	GETPROCADDR (PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
	GETPROCADDR (PFNGLCHECKFRAMEBUFFERSTATUSPROC,
		     glCheckFramebufferStatus);

	return true;
}

void
GLXContext::MakeCurrent ()
{
	g_assert (ctx);
	glXMakeCurrent (dpy, drawable, ctx);
}

};
