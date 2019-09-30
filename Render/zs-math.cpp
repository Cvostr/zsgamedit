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

    float invrt = (1.0f / static_cast<float>(sqrt(sqr)));
	v->X *= invrt;
	v->Y *= invrt;
	v->Z *= invrt;

}

void qNormalize(ZSQUATERNION* q){
    float m = q->X * q->X + q->Y * q->Y + q->Z * q->Z + q->W * q->W;

    if (REAL_NUM_EQ(m, 1) || REAL_NUM_EQ(m, 0))
        return;

    m = 1.f / sqrt(m);

    q->X *= m;
    q->Y *= m;
    q->Z *= m;
    q->W *= m;
}

ZSQUATERNION eulerToQuat(ZSVECTOR3 v){


    /*float p = DegToRad(pitch / 2.0f); float y = DegToRad(yaw / 2.0f); float r = DegToRad(roll / 2.0f);

    float sinp = sin(p);
    float siny = sin(y);
    float sinr = sin(r);
    float cosp = cos(p);
    float cosy = cos(y);
    float cosr = cos(r);

    float _x = sinr * cosp * cosy - cosr * sinp * siny;
    float _y = cosr * sinp * cosy + sinr * cosp * siny;
    float _z = cosr * cosp * siny - sinr * sinp * cosy;
    float _w = cosr * cosp * cosy + sinr * sinp * siny;

    ZSQUATERNION result(_x, _y, _z, _w);

    qNormalize(&result);*/
}

ZSVECTOR3 quatToEuler(ZSQUATERNION q){

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

bool isDistanceFits(ZSVECTOR3 pos1, ZSVECTOR3 pos2, float max_dist){
    float dist = getDistance(pos1, pos2);
    return dist <= max_dist ? true : false;
}

ZSVECTOR3 lerp(ZSVECTOR3 v1, ZSVECTOR3 v2, float factor){
    ZSVECTOR3 result = v1 * (1.f - factor) + v2 * factor;
    return result;
}
ZSQUATERNION slerp(ZSQUATERNION q1, ZSQUATERNION q2, float factor){
    ZSQUATERNION result;
    //Normalize quaternions
    qNormalize(&q1);
    qNormalize(&q2);

    float dot_product = result.X * result.X + result.Y * result.Y + result.Z * result.Z + result.W * result.W;
    float one_minus_blend = 1.0f - factor;

    if (dot_product < 0.0f)
    {
        result.X = q1.X * one_minus_blend + factor * -q2.X;
        result.Y = q1.Y * one_minus_blend + factor * -q2.Y;
        result.Z = q1.Z * one_minus_blend + factor * -q2.Z;
        result.W = q1.W * one_minus_blend + factor * -q2.W;
     }
     else
     {
        result.X = q1.X * one_minus_blend + factor * q2.X;
        result.Y = q1.Y * one_minus_blend + factor * q2.Y;
        result.Z = q1.Z * one_minus_blend + factor * q2.Z;
        result.W = q1.W * one_minus_blend + factor * q2.W;
    }

    qNormalize(&result);

    return result;
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

ZSMATRIX4x4 matrixSum(ZSMATRIX4x4 l, ZSMATRIX4x4 r) {
    ZSMATRIX4x4 Ret;
    for (unsigned int i = 0; i < 4; i++) {
        for (unsigned int b = 0; b < 4; b++) {
            Ret.m[i][b] = l.m[i][b] + r.m[i][b];
        }

    }
    return Ret;
}


ZSMATRIX4x4 getPerspective(float fovy, float aspect, float zNear, float zFar) {
	ZSMATRIX4x4 result;

    double range = tan(static_cast<double>(ZS_PI * (fovy / (2.0f * 180.f)))) * static_cast<double>(zNear);
    double left = -range * static_cast<double>(aspect);
    double right = range * static_cast<double>(aspect);
	double bottom = -range;
	double top = range;

    result.m[0][0] = (2.0f * zNear) / static_cast<float>(right - left);
    result.m[1][1] = (2.0f * zNear) / static_cast<float>(top - bottom);
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

float determinant(ZSMATRIX4x4 mat){
    //[0][0] [0][1] [0][2] [0][3]
    //[1][0] [1][1] [1][2] [1][3]
    //[2][0] [2][1] [2][2] [2][3]
    //[3][0] [3][1] [3][2] [3][3]

    float d1 = mat.m[0][0] * determinant(mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[2][1], mat.m[2][2], mat.m[2][3], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
    float d2 = mat.m[0][1] * determinant(mat.m[1][0], mat.m[1][2], mat.m[1][3], mat.m[2][0], mat.m[2][2], mat.m[2][3], mat.m[3][0], mat.m[3][2], mat.m[3][3]);
    float d3 = mat.m[0][2] * determinant(mat.m[1][0], mat.m[1][1], mat.m[1][3], mat.m[2][0], mat.m[2][1], mat.m[2][3], mat.m[3][0], mat.m[3][1], mat.m[3][3]);
    float d4 = mat.m[0][3] * determinant(mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[3][0], mat.m[3][1], mat.m[3][2]);

    return d1 - d2 + d3 - d4;

}
float determinant(float a, float b, float c, float d, float e, float f, float g, float h, float i){
    //a b c
    //d e f
    //g h i
    return (a * e * i) + (d * c * h) + (g * b * f) - (g * e * c) - (d * b * i) - (h * f * a);
}

ZSMATRIX4x4 invert(ZSMATRIX4x4 mat){
    float _determinant = determinant(mat);
    _determinant = 1.f / _determinant;

    float A11 = +determinant(mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[2][1], mat.m[2][2], mat.m[2][3], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
    float A12 = -determinant(mat.m[1][0], mat.m[1][2], mat.m[1][3], mat.m[2][0], mat.m[2][2], mat.m[2][3], mat.m[3][0], mat.m[3][2], mat.m[3][3]);
    float A13 = +determinant(mat.m[1][0], mat.m[1][1], mat.m[1][3], mat.m[2][0], mat.m[2][1], mat.m[2][3], mat.m[3][0], mat.m[3][1], mat.m[3][3]);
    float A14 = -determinant(mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[3][0], mat.m[3][1], mat.m[3][2]);

    float A21 = -determinant(mat.m[0][1], mat.m[0][2], mat.m[0][3], mat.m[2][1], mat.m[2][2], mat.m[2][3], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
    float A22 = +determinant(mat.m[0][0], mat.m[0][2], mat.m[0][3], mat.m[2][0], mat.m[2][2], mat.m[2][3], mat.m[3][0], mat.m[3][2], mat.m[3][3]);
    float A23 = -determinant(mat.m[0][0], mat.m[0][1], mat.m[0][3], mat.m[2][0], mat.m[2][1], mat.m[2][3], mat.m[3][0], mat.m[3][1], mat.m[3][3]);
    float A24 = +determinant(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[3][0], mat.m[3][1], mat.m[3][2]);

    float A31 = +determinant(mat.m[0][1], mat.m[0][2], mat.m[0][3], mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
    float A32 = -determinant(mat.m[0][0], mat.m[0][2], mat.m[0][3], mat.m[1][0], mat.m[1][2], mat.m[1][3], mat.m[3][0], mat.m[3][2], mat.m[3][3]);
    float A33 = +determinant(mat.m[0][0], mat.m[0][1], mat.m[0][3], mat.m[1][0], mat.m[1][1], mat.m[1][3], mat.m[3][0], mat.m[3][1], mat.m[3][3]);
    float A34 = -determinant(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[3][0], mat.m[3][1], mat.m[3][2]);

    float A41 = -determinant(mat.m[0][1], mat.m[0][2], mat.m[0][3], mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[2][1], mat.m[2][2], mat.m[2][3]);
    float A42 = +determinant(mat.m[0][0], mat.m[0][2], mat.m[0][3], mat.m[1][0], mat.m[1][2], mat.m[1][3], mat.m[2][0], mat.m[2][2], mat.m[2][3]);
    float A43 = -determinant(mat.m[0][0], mat.m[0][1], mat.m[0][3], mat.m[1][0], mat.m[1][1], mat.m[1][3], mat.m[2][0], mat.m[2][1], mat.m[2][3]);
    float A44 = +determinant(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2]);

    ZSMATRIX4x4 result = ZSMATRIX4x4(ZSVECTOR4(A11, A21, A31, A41), ZSVECTOR4(A12, A22, A32, A42), ZSVECTOR4(A13, A23, A33, A43),
                                     ZSVECTOR4(A14, A24, A34, A44));
    result = result * _determinant;
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

ZSMATRIX4x4 removeTranslationFromViewMat(ZSMATRIX4x4 viewMat){
    ZSMATRIX4x4 result = viewMat;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;
    result.m[0][3] = 0;
    result.m[1][3] = 0;
    result.m[2][3] = 0;

    return result;
}
ZSMATRIX4x4 removeRotationFromTransformMat(ZSMATRIX4x4 transform, ZSMATRIX4x4 view){
    ZSMATRIX4x4 result = transform;

    result.m[0][0] = view.m[0][0];
    result.m[0][1] = view.m[1][0];
    result.m[0][2] = view.m[2][0];

    result.m[1][0] = view.m[0][1];
    result.m[1][1] = view.m[1][1];
    result.m[1][2] = view.m[2][1];

    result.m[2][0] = view.m[0][2];
    result.m[2][1] = view.m[1][2];
    result.m[2][2] = view.m[2][2];

    return result;
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
    float theta = DegToRad(thetaN);
	ZSMATRIX4x4 mat;

	mat.m[0][0] = 1; mat.m[0][1] = 0;               mat.m[0][2] = 0;                mat.m[0][3] = 0;
    mat.m[1][0] = 0; mat.m[1][1] = static_cast<float>(cos(theta));      mat.m[1][2] = static_cast<float>(sin(theta)) * -1;  mat.m[1][3] = 0;
    mat.m[2][0] = 0; mat.m[2][1] = static_cast<float>(sin(theta));      mat.m[2][2] = static_cast<float>(cos(theta));       mat.m[2][3] = 0;
	mat.m[3][0] = 0; mat.m[3][1] = 0;               mat.m[3][2] = 0;                mat.m[3][3] = 1;

	return mat;
}
ZSMATRIX4x4 getRotationYMat(float thetaN) {
    float theta = DegToRad(thetaN);
	ZSMATRIX4x4 mat;

    mat.m[0][0] = static_cast<float>(cos(theta));       mat.m[0][1] = 0;            mat.m[0][2] = static_cast<float>(sin(theta));   mat.m[0][3] = 0;
	mat.m[1][0] = 0;                mat.m[1][1] = 1;            mat.m[1][2] = 0;            mat.m[1][3] = 0;
    mat.m[2][0] = static_cast<float>(sin(theta)) * -1;  mat.m[2][1] = 0;            mat.m[2][2] = static_cast<float>(cos(theta));   mat.m[2][3] = 0;
	mat.m[3][0] = 0;                mat.m[3][1] = 0;            mat.m[3][2] = 0;            mat.m[3][3] = 1;

	return mat;
}
ZSMATRIX4x4 getRotationZMat(float thetaN) {
    float theta = DegToRad(thetaN);
	ZSMATRIX4x4 mat;

    mat.m[0][0] = static_cast<float>(cos(theta));       mat.m[0][1] = static_cast<float>(sin(theta)) * -1;  mat.m[0][2] = 0;   mat.m[0][3] = 0;
    mat.m[1][0] = static_cast<float>(sin(theta));       mat.m[1][1] = static_cast<float>(cos(theta));       mat.m[1][2] = 0;   mat.m[1][3] = 0;
	mat.m[2][0] = 0;                mat.m[2][1] = 0;                mat.m[2][2] = 1;   mat.m[2][3] = 0;
	mat.m[3][0] = 0;                mat.m[3][1] = 0;                mat.m[3][2] = 0;   mat.m[3][3] = 1;

	return mat;
}

ZSMATRIX4x4 getRotationMat(float thetaX, float thetaY, float thetaZ){
    //Calculate matrices
    ZSMATRIX4x4 xmat = getRotationXMat(thetaX);
    ZSMATRIX4x4 ymat = getRotationYMat(thetaY);
    ZSMATRIX4x4 zmat = getRotationZMat(thetaZ);

    ZSMATRIX4x4 result = getIdentity(); //Define identity
    //Sequantily multiply it by xmat,ymat,zmat
    result = result * xmat;
    result = result * ymat;
    result = result * zmat;

    return result;
}

ZSMATRIX4x4 getRotationMat(ZSVECTOR3 rotation){
    return getRotationMat(rotation.X, rotation.Y, rotation.Z);
}

ZSMATRIX4x4 getRotationMat(ZSVECTOR3 rotation, ZSVECTOR3 center){
    ZSMATRIX4x4 result = getTranslationMat(center);
    result = result * getRotationMat(rotation);
    result = result * getTranslationMat(center * -1);
    return result;
}

ZSMATRIX4x4 getRotationMat(ZSQUATERNION quat){
    float x2 = quat.X * quat.X;
    float y2 = quat.Y * quat.Y;
    float z2 = quat.Z * quat.Z;
    float xy = quat.X * quat.Y;
    float xz = quat.X * quat.Z;
    float yz = quat.Y * quat.Z;
    float wx = quat.W * quat.X;
    float wy = quat.W * quat.Y;
    float wz = quat.W * quat.Z;

    // This calculation would be a lot more complicated for non-unit length quaternions // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by // OpenGL
    ZSMATRIX4x4 rot = ZSMATRIX4x4( ZSVECTOR4(1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f), ZSVECTOR4(2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f)
                                   , ZSVECTOR4(2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f),
                                   ZSVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
    return rot;
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
    this->gl_r = static_cast<float>(r) / 255;
    this->gl_g = static_cast<float>(g) / 255;
    this->gl_b = static_cast<float>(b) / 255;
    this->gl_a = static_cast<float>(a) / 255;
}

ZSVECTOR3 _getDirection(float pitch, float yaw, float roll) {
	ZSVECTOR3 q;

    float cy = cos(DegToRad(yaw * 0.5f));
    float sy = sin(DegToRad(yaw * 0.5f));
    float cr = cos(DegToRad(roll * 0.5f));
    float sr = sin(DegToRad(roll * 0.5f));
    float cp = cos(DegToRad(pitch * 0.5f));
    float sp = sin(DegToRad(pitch * 0.5f));


    q.Z = (cy * sr * cp - sy * cr * sp);
    q.X = (cy * cr * sp + sy * sr * cp);
    q.Y = (sy * cr * cp - cy * sr * sp);


	return q;
}

float getDistance(ZSVECTOR3 p1, ZSVECTOR3 p2) {
	float dx = p1.X - p2.X;
	float dy = p1.Y - p2.Y;
	float dz = p1.Z - p2.Z;

    return static_cast<float>(sqrt(dx * dx + dy * dy + dz * dz));
}

float getLength(ZSVECTOR3 vec){
    return static_cast<float>(sqrt(vec.X * vec.X + vec.Y * vec.Y + vec.Z * vec.Z));
}
