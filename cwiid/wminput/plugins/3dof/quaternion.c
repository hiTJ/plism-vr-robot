#include "quaternion.h"
#include <math.h>
#include <stdio.h>


struct quaternion qnull(){
	struct quaternion qnull;
	qnull.r=0;
	qnull.i=0;
	qnull.j=0;
	qnull.k=0;
	return qnull;
}

struct quaternion qidentity(){
	struct quaternion qidentity;
	qidentity.r=1;
	qidentity.i=0;
	qidentity.j=0;
	qidentity.k=0;
	return qidentity;
}

struct quaternion qfromangles( double roll, double yaw, double pitch){
	//Formula taken from http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	struct quaternion q;

	q.r = cos(roll/2)*cos(pitch/2)*cos(yaw/2) + sin(roll/2)*sin(pitch/2)*sin(yaw/2);
	q.i = sin(roll/2)*cos(pitch/2)*cos(yaw/2) - cos(roll/2)*sin(pitch/2)*sin(yaw/2);
	q.j = cos(roll/2)*sin(pitch/2)*cos(yaw/2) + sin(roll/2)*cos(pitch/2)*sin(yaw/2);
	q.k = cos(roll/2)*cos(pitch/2)*sin(yaw/2) - sin(roll/2)*sin(pitch/2)*cos(yaw/2);
	return qunit(q);
}

void qtoangles( struct quaternion q, double* roll, double* yaw, double* pitch ){
	//	q = qunit(q);
	//Formula taken from http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	*roll = atan2( 2*( q.r*q.i + q.j*q.k ), 1 - 2*( q.i*q.i + q.j*q.j ) );
	*yaw = atan2( 2*( q.r*q.k + q.i*q.j ), 1 - 2*( q.j*q.j + q.k*q.k ));
	*pitch = asin( 2*( q.r*q.j - q.i*q.k) );
}

struct quaternion rotate( struct quaternion vector, struct quaternion rotation ){
	rotation = qunit(rotation);
	return qmul(rotation, qmul(vector, qinv(rotation)));
}

struct quaternion qrotation(double angle, struct quaternion vector){
	vector = qscal( sin(angle/2) , qunit(vector) );
	vector.r = cos(angle/2);
	return vector;
}


struct quaternion qadd(struct quaternion q, struct quaternion r){
	q.r+=r.r;
	q.i+=r.i;
	q.j+=r.j;
	q.k+=r.k;
	return q;
}
struct quaternion qsub(struct quaternion q, struct quaternion r){
	q.r-=r.r;
	q.i-=r.i;
	q.j-=r.j;
	q.k-=r.k;
	return q;
}
struct quaternion qneg(struct quaternion q){
	q.r=-q.r;
	q.i=-q.i;
	q.j=-q.j;
	q.k=-q.k;
	return q;
}

struct quaternion qconj(struct quaternion q){
	q.r=q.r;
	q.i=-q.i;
	q.j=-q.j;
	q.k=-q.k;
	return q;
}
struct quaternion qmul(struct quaternion q, struct quaternion r){
	struct quaternion s;
	s.r=q.r*r.r - q.i*r.i - q.j*r.j - q.k*r.k;
	s.i=q.r*r.i + r.r*q.i + q.j*r.k - q.k*r.j;
	s.j=q.r*r.j + r.r*q.j + q.k*r.i - q.i*r.k;
	s.k=q.r*r.k + r.r*q.k + q.i*r.j - q.j*r.i;
	return s;
}

struct quaternion qscal(double coeff, struct quaternion q){
	q.r*=coeff;
	q.i*=coeff;
	q.j*=coeff;
	q.k*=coeff;
	return q;
}
struct quaternion qdiv(struct quaternion q, struct quaternion r){
	return qmul(q, qinv(r));
}
struct quaternion qunit(struct quaternion q){
	double l=qlen(q);
	q.r/=l;
	q.i/=l;
	q.j/=l;
	q.k/=l;
	return q;
}
/*
 * Bug?: takes no action on divide check
 */
struct quaternion qinv(struct quaternion q){
	double l=q.r*q.r+q.i*q.i+q.j*q.j+q.k*q.k;
	q.r/=l;
	q.i=-q.i/l;
	q.j=-q.j/l;
	q.k=-q.k/l;
	return q;
}
double qlen(struct quaternion p){
	return sqrt( p.r*p.r + p.i*p.i + p.j*p.j + p.k*p.k );
}

int qequals(struct quaternion p, struct quaternion q){
	return p.r == q.r && p.i == q.i && p.j==q.j && p.k == q.k;
}

struct quaternion qcross(struct quaternion p, struct quaternion q){
	struct quaternion r;
	r.r = 0;
	r.i = p.j*q.k - p.k*q.j;
	r.j = p.k*q.i - p.i*q.k;
	r.k = p.i*q.j - p.j*q.i;
	return r;
}

struct quaternion slerp(struct quaternion q, struct quaternion r, double a){
	double u, v, ang, s;
	double dot=q.r*r.r+q.i*r.i+q.j*r.j+q.k*r.k;
	ang=dot<-1?M_PI:dot>1?0:acos(dot); /* acos gives NaN for dot slightly out of range */
	s=sin(ang);
	if(s==0) return ang<M_PI/2?q:r;
	u=sin((1-a)*ang)/s;
	v=sin(a*ang)/s;
	q.r=u*q.r+v*r.r;
	q.i=u*q.i+v*r.i;
	q.j=u*q.j+v*r.j;
	q.k=u*q.k+v*r.k;
	return q;
}


struct quaternion qnew_rotation_axis(struct quaternion rotation, struct quaternion axis){
	//check axis is unitary vector
	if (axis.r !=0 || qlen(axis) != 1){
		printf("the given new axis is  not a proper vector");
		return rotation;
	}
	double sin_ = sqrt(rotation.i*rotation.i + rotation.j*rotation.j + rotation.k*rotation.k);
	struct quaternion vec;
	vec.r = 0;
	vec.i = rotation.i/sin_;
	vec.j = rotation.j/sin_;
	vec.k = rotation.k/sin_;

	//Important to get full angle rotations otherwise limited to 0 PI.
	double dot_product = qmul( vec, qconj(axis) ).r;

	if (dot_product< 0)
		sin_=-sin_;

	rotation.i = axis.i*sin_;
	rotation.j = axis.j*sin_;
	rotation.k = axis.k*sin_;
	return rotation;
}
/*
 * Only works if qlen(q)==qlen(r)==1
 */
struct quaternion qmid(struct quaternion q, struct quaternion r){
	double l;
	q=qadd(q, r);
	l=qlen(q);
	if(l<1e-12){
		q.r=r.i;
		q.i=-r.r;
		q.j=r.k;
		q.k=-r.j;
	}
	else{
		q.r/=l;
		q.i/=l;
		q.j/=l;
		q.k/=l;
	}
	return q;
}

static struct quaternion qident={1,0,0,0};
/*
 * Only works if qlen(q)==1
 */
struct quaternion qsqrt(struct quaternion q){
	return qmid(q, qident);
}
