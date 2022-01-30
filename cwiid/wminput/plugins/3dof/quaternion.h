#ifndef QUATERNION_H
#define QUATERNION_H

/*
 * struct Quaternion arithmetic:
 *	qadd(q, r)	returns q+r
 *	qsub(q, r)	returns q-r
 *	qneg(q)		returns -q
 *	qmul(q, r)	returns q*r
 *	qdiv(q, r)	returns q/r, can divide check.
 *	qinv(q)		returns 1/q, can divide check.
 *	qrotation(angle, vector) returns quaternion corresponding to rotation around vector of angle.
 *	rotate(vector, rotation) returns to vector after rotation.
 *	qfromangles(roll, yaw, pitch) returns orientation quaternion based on Euler Angles
 *	qtoangles(q , &roll, &yaw, &pitch) feeds Euler angle with orientation quaternion.
 *	qnew_rotation_axis(r, axis) Corrects axis error if you know the correct axis.
 *	qcross(q, r) returns qxr
 *	double qlen(p)	returns modulus of p
 *	qunit(q)	returns a unit quaternion parallel to q
 * The following only work on unit quaternions and rotation matrices:
 *	slerp(q, r, a)	returns q*(r*q^-1)^a
 *	qmid(q, r)	slerp(q, r, .5)
 *	qsqrt(q)	qmid(q, (struct Quaternion){1,0,0,0})
 */

struct quaternion {
	double r;
	double i;
	double j;
	double k;
};

struct quaternion qnull();
struct quaternion qidentity();

struct quaternion qfromangles(double roll, double yaw, double pitch);
void qtoangles(struct quaternion q, double* roll, double* yaw, double* pitch);
struct quaternion rotate( struct quaternion vector, struct quaternion rotation );
struct quaternion qrotation(double angle, struct quaternion vector);
struct quaternion qadd(struct quaternion q, struct quaternion r);
struct quaternion qsub(struct quaternion q, struct quaternion r);
struct quaternion qneg(struct quaternion q);
struct quaternion qconj(struct quaternion q);
struct quaternion qmul(struct quaternion q, struct quaternion r);
struct quaternion qscal(double coeff, struct quaternion q);
struct quaternion qcross(struct quaternion p, struct quaternion q);
struct quaternion qdiv(struct quaternion q, struct quaternion r);
struct quaternion qunit(struct quaternion q);

struct quaternion qnew_rotation_axis(struct quaternion rotation, struct quaternion axis);
/*
 * Bug?: takes no action on divide check
 */
struct quaternion qinv(struct quaternion q);
int qequals(struct quaternion p, struct quaternion q);
double qlen(struct quaternion p);
struct quaternion slerp(struct quaternion q, struct quaternion r, double a);
/*
 * Only works if qlen(q)==qlen(r)==1
 */
struct quaternion qmid(struct quaternion q, struct quaternion r);
/*
 * Only works if qlen(q)==1
 */
struct quaternion qsqrt(struct quaternion q);

#endif // QUATERNION_H
