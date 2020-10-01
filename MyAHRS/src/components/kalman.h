//
// Created by damien.lachouette on 29/09/2020.
//

#pragma once

struct vec3d{
	float x=0,y=0,z=0;
	vec3d& operator+=(const vec3d& o){x+=o.x;y+=o.y;z+=o.z;return *this;}
	vec3d operator+(const vec3d& o){vec3d{x+o.x,y+o.y,z+o.z};}
	vec3d& operator-=(const vec3d& o){x-=o.x;y-=o.y;z-=o.z;return *this;}
	vec3d operator-(const vec3d& o){vec3d{x-o.x,y-o.y,z-o.z};}
	// float product
	vec3d& operator*=(const float o){x*=o;y*=o;z*=o;return *this;}
	vec3d operator*(const float o)const{return vec3d{x*o,y*o,z*o};}
	friend vec3d operator*(const float o, const vec3d a){return vec3d{a.x*o,a.y*o,a.z*o};}
};

struct mat3x3{
	float xx,xy,xz;
	float yx,yy,yz;
	float zx,zy,zz;
};

struct state{
	vec3d u,v,w;
	vec3d theta,thetapoin;
};

state evolve(const float deltaT, const state& previous){
	state s;
	// inertia conservation
	s.w = previous.w;
	s.v = previous.v + deltaT * s.w;
	s.u = previous.u + deltaT * s.v;
	s.thetapoin = previous.thetapoin;
	s.theta = previous.theta + deltaT * s.thetapoin;
	return s;
}

struct
