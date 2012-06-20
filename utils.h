#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <glm/glm.hpp>


inline std::ostream& operator<<(std::ostream& stream, const glm::ivec2& a)
{
	return stream <<"("<<a.x<<", "<<a.y<<")";
}


inline std::ostream& operator<<(std::ostream& stream, const glm::ivec3& a)
{
	return stream <<"("<<a.x<<", "<<a.y<<", "<<a.z<<")";
}

inline std::ostream& operator<<(std::ostream& stream, const glm::ivec4& a)
{
	return stream <<"("<<a.x<<", "<<a.y<<", "<<a.z<<", "<<a.w<<")";
}

inline std::ostream& operator<<(std::ostream& stream, const glm::vec3& a)
{
	return stream <<"("<<a.x<<", "<<a.y<<", "<<a.z<<")";
}

inline std::ostream& operator<<(std::ostream& stream, const glm::vec2& a)
{
	return stream <<"("<<a.x<<", "<<a.y<<")";
}


inline std::istream& operator>>(std::istream& stream, glm::vec3& a)
{
	//no error checking for now
	//ignore junk ie whitespace till open paren, 1000 is arbitrary
	//but probably adequate
	stream.ignore(1000, '(');
	stream >> a.x;
	stream.get();
	stream >> a.y;
	stream.get();
	stream >> a.z;
	stream.get();
	return stream;
}


inline std::ostream& operator<<(std::ostream& stream, const glm::vec4& a)
{
	return stream <<"("<<a.x<<", "<<a.y<<", "<<a.z<<", "<<a.w<<")";
}



//transpose the matrices because they're column major
inline std::ostream& operator<<(std::ostream& stream, const glm::mat4& mat)
{
	glm::mat4 t = glm::transpose(mat);
	return stream <<"["<<t[0]<<"\n"<<t[1]<<"\n"<<t[2]<<"\n"<<t[3]<<"]";
}


inline std::ostream& operator<<(std::ostream& stream, const glm::mat3& mat)
{
	glm::mat3 t = glm::transpose(mat);
	return stream <<"["<<t[0]<<"\n"<<t[1]<<"\n"<<t[2]<<"]";
}


//for now just put these here

class Plane
{
	public:
	glm::vec3 n;	//normal  points x on plane satisfy n dot x = d
	float d; //d = n dot p
	
	Plane() {}
	Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c)	//ccw winding
	{
		n = glm::normalize(glm::cross(b-a, c-a));   //cross_product(b-a, c-a).norm();
		d = glm::dot(n, a);
	}
	
	int intersect_segment(glm::vec3 a, glm::vec3 b, float& t, glm::vec3& q)
	{
		glm::vec3 ab = b - a;
		t = (d - glm::dot(n, a)) / glm::dot(n, ab);
		
		//if t in [0-1] compute point
		if (t >= 0.0f && t <= 1.0f) {
			q = a + t*ab;
			return 1;
		}
		
		//else no intersection
		return 0;
	}
	
};


//int intersect_segment_plane(glm::vec3 a, glm::vec3 b, Plane p, float& t, glm::vec3& q);


















#endif