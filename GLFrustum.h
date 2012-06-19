// GLFrustum.h
// Code by Richard S. Wright Jr.
// Encapsulates a frustum... works in conjunction
// with GLFrame
/* Copyright (c) 2005-2009, Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list 
of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list 
of conditions and the following disclaimer in the documentation and/or other 
materials provided with the distribution.

Neither the name of Richard S. Wright Jr. nor the names of other contributors may be used 
to endorse or promote products derived from this software without specific prior 
written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <glm/glm.hpp>
#include "GLFrame.h"

#ifndef __GL_FRAME_CLASS
#define __GL_FRAME_CLASS


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GLFrustum
{
public:
	GLFrustum(void)       // Set some Reasonable Defaults
	{ set_orthographic(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f); }

	// Set the View Frustum
	GLFrustum(float fov, float aspect, float near, float far)
	{ set_perspective(fov, aspect, near, far); }

	GLFrustum(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
	{ set_orthographic(xmin, xmax, ymin, ymax, zmin, zmax); }

	// Get the projection matrix for this guy
	const glm::mat4& GetProjectionMatrix(void) { return proj_mat; }

	// Calculates the corners of the Frustum and sets the projection matrix.
	// Orthographics Matrix Projection    
	void set_orthographic(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
	{
		rm::MakeOrthographicMatrix(proj_mat, xmin, xmax, ymin, ymax, zmin, zmax);


		// Fill in values for untransformed Frustum corners
		// Near Upper Left
		nearUL = glm::vec4(xmin, ymax, zmin, 1.0f);

		// Near Lower Left
		nearLL = glm::vec4(xmin, ymin, zmin, 1.0f);

		// Near Upper Right
		nearUR = glm::vec4(xmax, ymax, zmin, 1.0f);

		// Near Lower Right
		nearLR = glm::vec4(xmax, ymin, zmin, 1.0f);

		// Far Upper Left
		farUL = glm::vec4(xmin, ymax, zmax, 1.0f);

		// Far Lower Left
		farLL = glm::vec4(xmin, ymin, zmax, 1.0f);

		// Far Upper Right
		farUR = glm::vec4(xmax, ymax, zmax, 1.0f);

		// Far Lower Right
		farLR = glm::vec4(xmax, ymin, zmax, 1.0f);
	}


// Calculates the corners of the Frustum and sets the projection matrix.
// Perspective Matrix Projection (fov is radians)
void set_perspective(float fov, float aspect, float near, float far)
{
	float xmin, xmax, ymin, ymax;       // Dimensions of near clipping plane
	float xFmin, xFmax, yFmin, yFmax;   // Dimensions of far clipping plane

	// Do the Math for the near clipping plane
	//ymax = fNear * float(tan( fFov * RM_PI / 360.0 ));
	ymax = near * float(tan( fov * 0.5f ));
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = -xmin;

	// Construct the projection matrix
	//it's already identity (why duplicate code when I have this function)
	proj_mat = glm::perspective(fov, float(width)/float(height), zmin, zmax);
	

	// Do the Math for the far clipping plane
// 	yFmax = fFar * float(tan(fFov * RM_PI / 360.0));
	yFmax = far * float(tan(fov * 0.5f));
	yFmin = -yFmax;
	xFmin = yFmin * aspect;
	xFmax = -xFmin;


	// Fill in values for untransformed Frustum corners
	// Near Upper Left
	nearUL = glm::vec4(xmin, ymax, -near, 1.0f);

	// Near Lower Left
	nearLL = glm::vec4(xmin, ymin, -near, 1.0f);

	// Near Upper Right
	nearUR = glm::vec4(xmax, ymax, -near, 1.0f);

	// Near Lower Right
	nearLR = glm::vec4(xmax, ymin, -near, 1.0f);

	// Far Upper Left
	farUL = glm::vec4(xFmin, yFmax, -far, 1.0f);

	// Far Lower Left
	farLL = glm::vec4(xFmin, yFmin, -far, 1.0f);

	// Far Upper Right
	farUR = glm::vec4(xFmax, yFmax, -far, 1.0f);

	// Far Lower Right
	farLR = glm::vec4(xFmax, yFmin, -far, 1.0f);
}


// Builds a transformation matrix and transforms the corners of the Frustum,
// then derives the plane equations
glm::mat4 Transform(GLFrame& Camera)
{
	// Workspace
	glm::mat4 matrix;
	glm::vec3 vForward, vUp, vCross;
	glm::vec3 vOrigin;

	///////////////////////////////////////////////////////////////////
	// Create the transformation matrix. This was the trickiest part
	// for me. The default view from OpenGL is down the negative Z
	// axis. However, building a transformation axis from these 
	// directional vectors points the frustum the wrong direction. So
	// You must reverse them here, or build the initial frustum
	// backwards - which to do is purely a matter of taste. I chose to
	// compensate here to allow better operability with some of my other
	// legacy code and projects. RSW
	vForward = Camera.GetForwardVector();
	vForward = -vForward;

	vUp = Camera.GetUpVector();
	vOrigin = Camera.GetOrigin();
	
	
	//std::cout<<matrix<<"\n";
	// Calculate the right side (x) vector, drop it right into the matrix
	vCross = glm::cross(vUp, vForward);

	//std::cout<<vXAxis<<"\n";
	// Set matrix column 1
	matrix.setc1(vCross);

	// Y Column
	matrix.setc2(vUp);

	// Z Column
	matrix.setc3(vForward);

	// Translation (already done)
	matrix.setc4(vOrigin);
	
	

	////////////////////////////////////////////////////
	// Transform the frustum corners
	nearULT = matrix * nearUL;
	nearLLT = matrix * nearLL;
	nearURT = matrix * nearUR;
	nearLRT = matrix * nearLR;
	farULT = matrix * farUL;
	farLLT = matrix * farLL;
	farURT = matrix * farUR;
	farLRT = matrix * farLR;

// 	////////////////////////////////////////////////////
// 	// Derive Plane Equations from points... Points given in
// 	// counter clockwise order to make normals point inside 
// 	// the Frustum
// 	// Near and Far Planes
// 	m3dGetPlaneEquation(nearPlane, nearULT, nearLLT, nearLRT);
// 	m3dGetPlaneEquation(farPlane, farULT, farURT, farLRT);
// 
// 	// Top and Bottom Planes
// 	m3dGetPlaneEquation(topPlane, nearULT, nearURT, farURT);
// 	m3dGetPlaneEquation(bottomPlane, nearLLT, farLLT, farLRT);
// 
// 	// Left and right planes
// 	m3dGetPlaneEquation(leftPlane, nearLLT, nearULT, farULT);
// 	m3dGetPlaneEquation(rightPlane, nearLRT, farLRT, farURT);
	
	
	return matrix;
}



// 	// Allow expanded version of sphere test
// 	bool TestSphere(float x, float y, float z, float fRadius)
// 	{
// 		glm::vec3f vPoint;
// 		vPoint[0] = x;
// 		vPoint[1] = y;
// 		vPoint[2] = z;
// 
// 		return TestSphere(vPoint, fRadius);
// 	}
// 
// 	// Test a point against all frustum planes. A negative distance for any
// 	// single plane means it is outside the frustum. The radius value allows
// 	// to test for a point (radius = 0), or a sphere. Possibly there might
// 	// be some gain in an alternative function that saves the addition of 
// 	// zero in this case.
// 	// Returns false if it is not in the frustum, true if it intersects
// 	// the Frustum.
// 	bool TestSphere(glm::vec3f vPoint, float fRadius)
// 	{
// 		float fDist;
// 
// 		// Near Plane - See if it is behind me
// 		fDist = m3dGetDistanceToPlane(vPoint, nearPlane);
// 		if(fDist + fRadius <= 0.0)
// 		return false;
// 
// 		// Distance to far plane
// 		fDist = m3dGetDistanceToPlane(vPoint, farPlane);
// 		if(fDist + fRadius <= 0.0)
// 		return false;
// 
// 		fDist = m3dGetDistanceToPlane(vPoint, leftPlane);
// 		if(fDist + fRadius <= 0.0)
// 		return false;
// 
// 		fDist = m3dGetDistanceToPlane(vPoint, rightPlane);
// 		if(fDist + fRadius <= 0.0)
// 		return false;
// 
// 		fDist = m3dGetDistanceToPlane(vPoint, bottomPlane);
// 		if(fDist + fRadius <= 0.0)
// 		return false;
// 
// 		fDist = m3dGetDistanceToPlane(vPoint, topPlane);
// 		if(fDist + fRadius <= 0.0)
// 		return false;
// 
// 		return true;
// 	}

//protected:
	// The projection matrix for this frustum
	glm::mat4 proj_mat;	

	// Untransformed corners of the frustum
	glm::vec4 nearUL, nearLL, nearUR, nearLR;
	glm::vec4 farUL,  farLL,  farUR,  farLR;

	// Transformed corners of Frustum
	glm::vec4 nearULT, nearLLT, nearURT, nearLRT;
	glm::vec4 farULT,  farLLT,  farURT,  farLRT;

	// Base and Transformed plane equations
	glm::vec4 nearPlane, farPlane, leftPlane, rightPlane;
	glm::vec4 topPlane, bottomPlane;
};



#endif
