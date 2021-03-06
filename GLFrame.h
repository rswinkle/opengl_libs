// Frame.h
// Implementation of the GLFrame Class
// Richard S. Wright Jr.
// Code by Richard S. Wright Jr.
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

#include "utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef _ORTHO_FRAME_
#define _ORTHO_FRAME_

// The GLFrame (OrthonormalFrame) class. Possibly the most useful little piece of 3D graphics
// code for OpenGL immersive environments.
// Richard S. Wright Jr.



class GLFrame
{
public:
	glm::vec3 origin;	// Where am I?
	glm::vec3 forward;	// Where am I going?
	glm::vec3 up;		// Which way is up?

	// Default position and orientation. At the origin, looking
	// down the positive Z axis (right handed coordinate system).
	GLFrame(bool camera=false, glm::vec3 origin = glm::vec3(0.0f)) {
		// At origin
		this->origin = origin;

		// Up is up (+Y)
		up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

		// Forward is Z unless this is a camera frame
		if (!camera) {
			forward.x = 0.0f; forward.y = 0.0f; forward.z = 1.0f;
		} else {
			forward.x = 0.0f; forward.y = 0.0f; forward.z = -1.0f;
		}
	}


	/////////////////////////////////////////////////////////////
	// Set Location
	inline void set_origin(const glm::vec3 vPoint) { origin = vPoint; }
	inline void set_origin(float x, float y, float z) { origin.x = x; origin.y = y; origin.z = z; }

	inline glm::vec3 get_origin() { return origin; }


	/////////////////////////////////////////////////////////////
	// Set Forward Direction
	inline void set_forward(const glm::vec3 vDirection) { forward = vDirection; }
	inline void set_forward(float x, float y, float z) { forward.x = x; forward.y = y; forward.z = z; }

	inline glm::vec3 get_forward() { return forward; }

	/////////////////////////////////////////////////////////////
	// Set Up Direction
	inline void set_up(const glm::vec3 vDirection) { up = vDirection; }
	inline void set_up(float x, float y, float z) { up.x = x; up.y = y; up.z = z; }

	inline glm::vec3 get_up() { return up; }


	/////////////////////////////////////////////////////////////
	// Get Axes
	inline glm::vec3 get_z() { return forward; }
	inline glm::vec3 get_y() { return up; }
	inline glm::vec3 get_x() { return glm::cross(up, forward); }


	/////////////////////////////////////////////////////////////
	// Translate along orthonormal axis... world or local
	inline void translate_world(float x, float y, float z) { origin.x += x; origin.y += y; origin.z += z; }

	inline void translate_local(float x, float y, float z) { move_forward(z); move_up(y); move_right(x);	}


	/////////////////////////////////////////////////////////////
	// Move Forward (along Z axis)
	inline void move_forward(float delta) { origin += forward * delta; }


	// Move along Y axis
	inline void move_up(float delta) { origin += up * delta; }


	// Move along X axis
	inline void move_right(float delta) { origin += glm::cross(up, forward) * delta; }
	


	///////////////////////////////////////////////////////////////////////
	// Just assemble the matrix
	glm::mat4 get_matrix(bool bRotationOnly = false)
	{
		// Calculate the right side (x) vector, drop it right into the matrix
		glm::vec3 vXAxis = glm::cross(up, forward);

		//note: glm matrices are column major (stupid) and are actually just an array of columns
		//ie for mat4 (4x4), vec4 value[4] where each vector is a column
		//in reality as with all of glm there's a lot more complex template magic obfuscating what's happening
		//and giving far more flexibility that almost anyone will ever need imo.
		
		// Set matrix columns..
		glm::mat4 matrix;
		matrix[0] = glm::vec4(vXAxis, 0);

		// Y Column
		matrix[1] = glm::vec4(up, 0);

		// Z Column
		matrix[2] = glm::vec4(forward, 0);
		
		// Translation (already done)
		if(bRotationOnly == true) {
			matrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		} else {
			matrix[3] = glm::vec4(origin, 1.0f);
		}
	}



	////////////////////////////////////////////////////////////////////////
	// Assemble the camera matrix
	glm::mat4 get_camera_matrix(bool bRotationOnly = false)
	{
		glm::mat4 m;
		glm::vec3 x, z;

		// Make rotation matrix
		// Z vector is reversed
		z = -forward;

		// X vector = Y cross Z 
		x = glm::cross(up, z);
		
		// Matrix has no translation information and is
		// transposed.... (rows instead of columns)
		//FYI: transposed orthonormal matrix is inverse, inverse transformation matrix
		//is opposite transformation so this takes points and does the transformation that would takes
		//the camera frame back to canonical frame -Robert Winkler
		
		m[0] = glm::vec4(x.x, up.x, z.x, 0.0f);
		m[1] = glm::vec4(x.y, up.y, z.y, 0.0f);
		m[2] = glm::vec4(x.z, up.z, z.z, 0.0f);
		m[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


		if(bRotationOnly) {
			return m;
		}
		
		// Apply translation too
		glm::mat4 trans;
		trans[3] = glm::vec4(-origin, 1.0f);	//default constructor loads identity so this is all we need to form translation

		//could instead of having the previous 2 lines just mat*(-vOrigin) and drop the result
		//in column 4 of mat.  I think that actually saves mult ops
		
		//std::cout<<mat*trans<<"\t%\n";
		//int blah;
		//std::cin>>blah;
		return m*trans;
		
	}


	// Rotate around local Y
	void rotate_local_y(float angle)
	{
		// Just Rotate around the up vector
		// Create a rotation matrix around my Up (Y) vector
		//mat constructor is identity, first argument of glm transformation funcs are pre-multiplied
		//ie returns arg1 * the rotation matrix it builds, so we just pass in identity
		glm::mat3 rot_mat = glm::mat3(glm::rotate(glm::mat4(), angle, up));
		
// 		std::cout<<rot_mat
		forward = rot_mat * forward;
		
	}


	// Rotate around local Z
	void rotate_local_z(float angle)
	{
		// Just Rotate around the forward vector
		glm::mat3 rot_mat = glm::mat3(glm::rotate(glm::mat4(), angle, forward));
		
		up = rot_mat * up;
		
		//std::cout<<rot_mat<<"\n\n"<<up<<"\n\n";

	}
	
	
	void rotate_local_x(float angle)
	{
		//get x then rotate up and forward
		glm::vec3 x = glm::cross(up, forward);
		
		glm::mat3 rot_mat = glm::mat3(glm::rotate(glm::mat4(), angle, x));
		
		up = rot_mat * up;
		forward = rot_mat * forward;
	}


	// Reset axes to make sure they are orthonormal. This should be called on occasion
	// if the matrix is long-lived and frequently transformed.
	void normalize(void)
	{
		glm::vec3 vCross = glm::cross(up, forward);
		
		forward = glm::cross(vCross, up);
		
		up = glm::normalize(up);
		forward = glm::normalize(forward);
	}

	//THIS IS WHERE I AM TODO
	
	// Rotate in world coordinates...
	void rotate_world(float angle, float x, float y, float z)
	{
		glm::mat3 rot_mat = glm::mat3(glm::rotate(glm::mat4(), angle, glm::vec3(x,y,z)));
		
		up = rot_mat * up;
		forward = rot_mat * forward;
	}


	// Rotate around a local axis
	void rotate_local(float angle, float x, float y, float z) 
	{
		glm::vec3 world_vec;
		glm::vec3 local_vec(x, y, z);

		world_vec = local_to_world(local_vec, true);
		rotate_world(angle, world_vec.x, world_vec.y, world_vec.z);
	}


	// Convert Coordinate Systems
	// This is pretty much, do the transformation represented by the rotation
	// and position on the point

	glm::vec3 local_to_world(const glm::vec3 local, bool bRotOnly = false)
	{
		// Create the rotation matrix based on the vectors
		glm::mat4 rot_mat = get_matrix(true);

		// Do the rotation
		glm::vec3 world = glm::vec3(rot_mat * glm::vec4(local, 1));

		// Translate the point
		if(!bRotOnly) {
			world += origin;
		}
		
		return world;
	}



	// Change world coordinates into "local" coordinates
	glm::vec3 world_to_local(const glm::vec3 world)
	{
		////////////////////////////////////////////////
		// Translate the origin
		glm::vec3 new_world = world - origin;

		// Create the rotation matrix based on the vectors
		mat4 rot_mat, inv_mat;
		rot_mat = get_matrix(true);

		// Do the rotation based on inverted matrix
		inv_mat = glm::inverse(rot_mat);

		return mat3(inv_mat) * new_world;
	}

// 
// 	/////////////////////////////////////////////////////////////////////////////
// 	// Transform a point by frame matrix
// 	void TransformPoint(glm::vec3 vPointSrc, glm::vec3 vPointDst)
// 	{
// 		M3DMatrix44f m;
// 		GetMatrix(m, false);    // Rotate and translate
// 		vPointDst[0] = m[0] * vPointSrc[0] + m[4] * vPointSrc[1] + m[8] *  vPointSrc[2] + m[12];// * v[3];
// 		vPointDst[1] = m[1] * vPointSrc[0] + m[5] * vPointSrc[1] + m[9] *  vPointSrc[2] + m[13];// * v[3];
// 		vPointDst[2] = m[2] * vPointSrc[0] + m[6] * vPointSrc[1] + m[10] * vPointSrc[2] + m[14];// * v[3];
// 	}
// 
// 	////////////////////////////////////////////////////////////////////////////
// 	// Rotate a vector by frame matrix
// 	void RotateVector(glm::vec3 vVectorSrc, glm::vec3 vVectorDst)
// 	{
// 		M3DMatrix44f m;
// 		GetMatrix(m, true);    // Rotate only
// 
// 		vVectorDst[0] = m[0] * vVectorSrc[0] + m[4] * vVectorSrc[1] + m[8] *  vVectorSrc[2];	
// 		vVectorDst[1] = m[1] * vVectorSrc[0] + m[5] * vVectorSrc[1] + m[9] *  vVectorSrc[2];
// 		vVectorDst[2] = m[2] * vVectorSrc[0] + m[6] * vVectorSrc[1] + m[10] * vVectorSrc[2];
// 	}
};


#endif
