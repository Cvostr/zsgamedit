#include "math.h"

#include "headers/zs-math.h"

float DegToRad(float degrees){
	return degrees * ZS_PI / 180;
}

void vNormalize(ZSVECTOR3* v)
{
	ZSVECTOR3 result;

	float sqr = v->X * v->X + v->Y * v->Y + v->Z * v->Z;
	if (REAL_NUM_EQ(sqr, 1) || REAL_NUM_EQ(sqr, 0))
		return;

	float invrt = (1.0f / (float)sqrt(sqr));
	v->X *= invrt;
	v->Y *= invrt;
	v->Z *= invrt;

}

ZSVECTOR3 vCross(ZSVECTOR3 v1, ZSVECTOR3 v2)
{
	ZSVECTOR3 out = ZSVECTOR3(v1.Y*v2.Z - v1.Z*v2.Y, v1.Z*v2.X - v1.X*v2.Z, v1.X*v2.Y - v1.Y*v2.X);
	return out;
}

float vDot(ZSVECTOR3 v1, ZSVECTOR3 v2)
{
	return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
}

ZSMATRIX4x4 getIdentity() {
	ZSMATRIX4x4 Ret;

	Ret.m[0][0] = 1; Ret.m[0][1] = 0; Ret.m[0][2] = 0; Ret.m[0][3] = 0;
	Ret.m[1][0] = 0; Ret.m[1][1] = 1; Ret.m[1][2] = 0; Ret.m[1][3] = 0;
	Ret.m[2][0] = 0; Ret.m[2][1] = 0; Ret.m[2][2] = 1; Ret.m[2][3] = 0;
	Ret.m[3][0] = 0; Ret.m[3][1] = 0; Ret.m[3][2] = 0; Ret.m[3][3] = 1;

	return Ret;
}

ZSMATRIX4x4 matrixMM(ZSMATRIX4x4 l, ZSMATRIX4x4 r) {
	ZSMATRIX4x4 Ret;
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int b = 0; b < 4; b++) {
			Ret.m[i][b] = l.m[i][0] * r.m[0][b] +
				l.m[i][1] * r.m[1][b] +
				l.m[i][2] * r.m[2][b] +
				l.m[i][3] * r.m[3][b];

		}

	}
	return Ret;
}

ZSMATRIX4x4 getPerspective(float fovy, float aspect, float zNear, float zFar) {
	ZSMATRIX4x4 result;

	double range = tan(ZS_PI * (fovy / (2.0f * 180.f))) * zNear;
	double left = -range * aspect;
	double right = range * aspect;
	double bottom = -range;
	double top = range;

	result.m[0][0] = (2.0f * zNear) / (float)(right - left);
	result.m[1][1] = (2.0f * zNear) / (float)(top - bottom);
	result.m[2][2] = -(zFar + zNear) / (zFar - zNear);
	result.m[2][3] = -1.0f;
	result.m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
	
	return result;
}

ZSMATRIX4x4 transpose(ZSMATRIX4x4 mat) {
	ZSMATRIX4x4 result;

	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			result.m[i][j] = mat.m[j][i];
		}
	}
	return result;
}

ZSMATRIX4x4 matrixLookAt(ZSVECTOR3 eye, ZSVECTOR3 center, ZSVECTOR3 up)
{
	ZSMATRIX4x4 out;

	ZSVECTOR3 f = ZSVECTOR3(center.X - eye.X, center.Y - eye.Y, center.Z - eye.Z);
	vNormalize(&f);

	ZSVECTOR3 u;

	ZSVECTOR3 s = vCross(f, up);
	vNormalize(&s);
	u = vCross(s, f);

	out = getIdentity();
	out.m[0][0] = s.X;
	out.m[1][0] = s.Y;
	out.m[2][0] = s.Z;

	out.m[0][1] = u.X;
	out.m[1][1] = u.Y;
	out.m[2][1] = u.Z;

	out.m[0][2] = -f.X;
	out.m[1][2] = -f.Y;
	out.m[2][2] = -f.Z;

	out.m[3][0] = -vDot(s, eye);
	out.m[3][1] = -vDot(u, eye);
	out.m[3][2] = vDot(f, eye);

	return out;
}

ZSMATRIX4x4 getScaleMat(float scaleX, float scaleY, float scaleZ) {

	ZSMATRIX4x4 mat;

	mat.m[0][0] = scaleX; mat.m[0][1] = 0; mat.m[0][2] = 0; mat.m[0][3] = 0;
	mat.m[1][0] = 0; mat.m[1][1] = scaleY; mat.m[1][2] = 0; mat.m[1][3] = 0;
	mat.m[2][0] = 0; mat.m[2][1] = 0; mat.m[2][2] = scaleZ; mat.m[2][3] = 0;
	mat.m[3][0] = 0; mat.m[3][1] = 0; mat.m[3][2] = 0; mat.m[3][3] = 1;

	return mat;
}

ZSMATRIX4x4 getScaleMat(ZSVECTOR3 scale){
    return getScaleMat(scale.X, scale.Y, scale.Z);
}

ZSMATRIX4x4 getTranslationMat(float trX, float trY, float trZ) {

	ZSMATRIX4x4 mat;

	mat.m[0][0] = 1; mat.m[0][1] = 0; mat.m[0][2] = 0; mat.m[0][3] = 0;
	mat.m[1][0] = 0; mat.m[1][1] = 1; mat.m[1][2] = 0; mat.m[1][3] = 0;
	mat.m[2][0] = 0; mat.m[2][1] = 0; mat.m[2][2] = 1; mat.m[2][3] = 0;
	mat.m[3][0] = trX; mat.m[3][1] = trY; mat.m[3][2] = trZ; mat.m[3][3] = 1;

	return mat;
}

ZSMATRIX4x4 getTranslationMat(ZSVECTOR3 translation){
    return getTranslationMat(translation.X, translation.Y, translation.Z);
}

ZSMATRIX4x4 getRotationXMat(float thetaN) {
	double theta = DegToRad(thetaN);
	ZSMATRIX4x4 mat;

	mat.m[0][0] = 1; mat.m[0][1] = 0;               mat.m[0][2] = 0;                mat.m[0][3] = 0;
	mat.m[1][0] = 0; mat.m[1][1] = (float)cos(theta);      mat.m[1][2] = (float)sin(theta) * -1;  mat.m[1][3] = 0;
	mat.m[2][0] = 0; mat.m[2][1] = (float)sin(theta);      mat.m[2][2] = (float)cos(theta);       mat.m[2][3] = 0;
	mat.m[3][0] = 0; mat.m[3][1] = 0;               mat.m[3][2] = 0;                mat.m[3][3] = 1;

	return mat;
}
ZSMATRIX4x4 getRotationYMat(float thetaN) {
	double theta = DegToRad(thetaN);
	ZSMATRIX4x4 mat;

	mat.m[0][0] = (float)cos(theta);       mat.m[0][1] = 0;            mat.m[0][2] = (float)sin(theta);   mat.m[0][3] = 0;
	mat.m[1][0] = 0;                mat.m[1][1] = 1;            mat.m[1][2] = 0;            mat.m[1][3] = 0;
	mat.m[2][0] = (float)sin(theta) * -1;  mat.m[2][1] = 0;            mat.m[2][2] = (float)cos(theta);   mat.m[2][3] = 0;
	mat.m[3][0] = 0;                mat.m[3][1] = 0;            mat.m[3][2] = 0;            mat.m[3][3] = 1;

	return mat;
}
ZSMATRIX4x4 getRotationZMat(float thetaN) {
	double theta = DegToRad(thetaN);
	ZSMATRIX4x4 mat;

	mat.m[0][0] = (float)cos(theta);       mat.m[0][1] = (float)sin(theta) * -1;  mat.m[0][2] = 0;   mat.m[0][3] = 0;
	mat.m[1][0] = (float)sin(theta);       mat.m[1][1] = (float)cos(theta);       mat.m[1][2] = 0;   mat.m[1][3] = 0;
	mat.m[2][0] = 0;                mat.m[2][1] = 0;                mat.m[2][2] = 1;   mat.m[2][3] = 0;
	mat.m[3][0] = 0;                mat.m[3][1] = 0;                mat.m[3][2] = 0;   mat.m[3][3] = 1;

	return mat;
}

ZSMATRIX4x4 getOrthogonal(float left, float right, float bottom, float top)
{
	ZSMATRIX4x4 out = getIdentity();

	out.m[0][0] = 2.0f / (right - left);
	out.m[1][1] = 2.0f / (top - bottom);
	out.m[2][2] = -1.0f;
	out.m[3][0] = -(right + left) / (right - left);
	out.m[3][1] = -(top + bottom) / (top - bottom);

	return out;
}

ZSMATRIX4x4 getOrthogonal(float left, float right, float bottom, float top, float zNear, float zFar) {
	ZSMATRIX4x4 out = getIdentity();

	out.m[0][0] = 2.0f / (right - left);
	out.m[1][1] = 2.0f / (top - bottom);

	out.m[2][2] = -1.0f / (zFar - zNear);
	out.m[3][2] = -zNear / (zFar - zNear);

	out.m[3][0] = -(right + left) / (right - left);
	out.m[3][1] = -(top + bottom) / (top - bottom);


	return out;
}


void ZSRGBCOLOR::updateGL() {
	this->gl_r = (float)r / 255;
	this->gl_g = (float)g / 255;
	this->gl_b = (float)b / 255;
	this->gl_a = (float)a / 255;
}

ZSVECTOR3 _getDirection(float pitch, float yaw, float roll) {
	ZSVECTOR3 q;

	double cy = cos(DegToRad(yaw * 0.5f));
	double sy = sin(DegToRad(yaw * 0.5f));
	double cr = cos(DegToRad(roll * 0.5f));
	double sr = sin(DegToRad(roll * 0.5f));
	double cp = cos(DegToRad(pitch * 0.5f));
	double sp = sin(DegToRad(pitch * 0.5f));


	q.Z = (float)(cy * sr * cp - sy * cr * sp);
	q.X = (float)(cy * cr * sp + sy * sr * cp);
	q.Y = (float)(sy * cr * cp - cy * sr * sp);


	return q;
}

float getDistance(ZSVECTOR3 p1, ZSVECTOR3 p2) {
	float dx = p1.X - p2.X;
	float dy = p1.Y - p2.Y;
	float dz = p1.Z - p2.Z;

	return (float)sqrt(dx * dx + dy * dy + dz * dz);
}
