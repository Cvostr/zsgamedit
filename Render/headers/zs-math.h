#ifndef ZSMATH
#define ZSMATH


#define ZS_PI 3.14159265f
#define REAL_NUM_EQ(x, y) (fabs((x) - (y)) < 0.000001)


	typedef struct ZSVECTOR2 {
		float X;
		float Y;

        ZSVECTOR2() {
            X = 0;
            Y = 0;
        }
		ZSVECTOR2(float nx, float ny) {
				X = nx;
				Y = ny;
		}

	}ZSVECTOR2;

	typedef struct ZSVECTOR3 {
		float X;
		float Y;
		float Z;

        ZSVECTOR3(){
            X = 0;
            Y = 0;
            Z = 0;
        }

		ZSVECTOR3(float nx, float ny, float nz) {
			X = nx;
			Y = ny;
			Z = nz;
		}

		ZSVECTOR3(float v) {
			X = v;
			Y = v;
			Z = v;
		}

	}ZSVECTOR3;


	typedef struct ZSVECTOR4 {

        float X;
        float Y;
        float Z;
        float W;

		ZSVECTOR4() {
            X = 0;
            Y = 0;
            Z = 0;
            W = 0;
		}

		ZSVECTOR4(float x, float y, float z, float w) {
			this->X = x;
			this->Y = y;
			this->Z = z;
			this->W = w;
		}

	}ZSVECTOR4;

	typedef struct ZSVERTEX {

		ZSVECTOR3 pos;

		ZSVECTOR2 uv;

		ZSVECTOR3 normal;

		ZSVECTOR3 tangent;

		ZSVECTOR3 bitangent;

		ZSVERTEX() {}
		ZSVERTEX(ZSVECTOR3 npos, ZSVECTOR2 nuv) : pos(npos.X, npos.Y, npos.Z), uv(nuv.X, nuv.Y) {}
		ZSVERTEX(ZSVECTOR3 npos, ZSVECTOR2 nuv, ZSVECTOR3 nnormal) : pos(npos.X, npos.Y, npos.Z), uv(nuv.X, nuv.Y), normal(nnormal.X, nnormal.Y, nnormal.Z) {}
		ZSVERTEX(ZSVECTOR3 npos, ZSVECTOR2 nuv, ZSVECTOR3 nnormal, ZSVECTOR3 ntangent, ZSVECTOR3 nbitangent) 
			: pos(npos.X, npos.Y, npos.Z), 
			uv(nuv.X, nuv.Y), 
			normal(nnormal.X, nnormal.Y, nnormal.Z),
			tangent(ntangent.X, ntangent.Y, ntangent.Z),
			bitangent(nbitangent.X, nbitangent.Y, nbitangent.Z){}

	}ZSVERTEX;

	typedef struct ZSMATRIX4x4 {
		float m[4][4];

		ZSMATRIX4x4(ZSVECTOR4 v1, ZSVECTOR4 v2, ZSVECTOR4 v3, ZSVECTOR4 v4) {
			m[0][0] = v1.X; m[0][1] = v1.Y; m[0][2] = v1.Z; m[0][3] = v1.W;
			m[1][0] = v2.X; m[1][1] = v2.Y; m[1][2] = v2.Z; m[1][3] = v2.W;
			m[2][0] = v3.X; m[2][1] = v3.Y; m[2][2] = v3.Z; m[2][3] = v3.W;
			m[3][0] = v4.X; m[3][1] = v4.Y; m[3][2] = v4.Z; m[3][3] = v4.W;


		}
		ZSMATRIX4x4() {
			for (unsigned int i = 0; i < 4; i++) {
				for (unsigned int j = 0; j < 4; j++) {
					m[i][j] = 0;
				}
			}

		}
	}ZSMATRIX4x4;


	typedef class ZSRGBCOLOR {
	public:
		int r;
		int g;
		int b;
		int a;

		float gl_r;
		float gl_g;
		float gl_b;
		float gl_a;

		void updateGL();

		ZSRGBCOLOR(int r, int g, int b, int a) {
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;

			this->updateGL();
		}
		ZSRGBCOLOR(int r, int g, int b) {
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = 255;

			this->updateGL();
		}

		ZSRGBCOLOR() {
			this->r = 255;
			this->g = 255;
			this->b = 255;
			this->a = 255;

			this->updateGL();
		}

	}ZSRGBCOLOR;
    //Converts degrees into radians
    float DegToRad(float degrees);
    //Get vector with identity lenght
	void vNormalize(ZSVECTOR3* v);
	ZSVECTOR3 vCross(ZSVECTOR3 v1, ZSVECTOR3 v2);
	float vDot(ZSVECTOR3 v1, ZSVECTOR3 v2);
    bool isDistanceFits(ZSVECTOR3 pos1, ZSVECTOR3 pos2, float max_dist);

	ZSMATRIX4x4 getIdentity();
	ZSMATRIX4x4 transpose(ZSMATRIX4x4 mat);
	ZSMATRIX4x4 matrixMM(ZSMATRIX4x4 l, ZSMATRIX4x4 r);
	ZSMATRIX4x4 getPerspective(float fovy, float aspect, float zNear, float zFar);
	ZSMATRIX4x4 getOrthogonal(float left, float right, float bottom, float top);
	ZSMATRIX4x4 getOrthogonal(float left, float right, float bottom, float top, float zNear, float zFar);
	ZSMATRIX4x4 matrixLookAt(ZSVECTOR3 eye, ZSVECTOR3 center, ZSVECTOR3 up);
	
	ZSMATRIX4x4 getScaleMat(float scaleX, float scaleY, float scaleZ);
    ZSMATRIX4x4 getScaleMat(ZSVECTOR3 scale);
	ZSMATRIX4x4 getTranslationMat(float trX, float trY, float trZ);
    ZSMATRIX4x4 getTranslationMat(ZSVECTOR3 translation);
	ZSMATRIX4x4 getRotationXMat(float thetaN);
	ZSMATRIX4x4 getRotationYMat(float thetaN);
	ZSMATRIX4x4 getRotationZMat(float thetaN);
    ZSMATRIX4x4 getRotationMat(float thetaX, float thetaY, float thetaZ);
    ZSMATRIX4x4 getRotationMat(ZSVECTOR3 rotation);
    ZSMATRIX4x4 getRotationMat(ZSVECTOR3 rotation, ZSVECTOR3 center);
    //ZSMATRIX4x4 parent


	float getDistance(ZSVECTOR3 p1, ZSVECTOR3 p2);
    float length(ZSVECTOR3 vec); //Calculates module length of vector
	ZSVECTOR3 _getDirection(float pitch, float yaw, float roll);

	inline ZSMATRIX4x4 operator*(const ZSMATRIX4x4& l, const ZSMATRIX4x4& r)
	{
		return matrixMM(l, r);

	}

	inline ZSVECTOR3 operator-(const ZSVECTOR3& l, const ZSVECTOR3& r)
	{
		ZSVECTOR3 Ret(l.X - r.X, l.Y - r.Y, l.Z - r.Z);

		return Ret;
	}

	inline ZSVECTOR3 operator+(const ZSVECTOR3& l, const ZSVECTOR3& r)
	{
		ZSVECTOR3 Ret(l.X + r.X, l.Y + r.Y, l.Z + r.Z);

		return Ret;
	}

	inline ZSVECTOR3 operator*(const ZSVECTOR3& l, const float& d)
	{
		ZSVECTOR3 Ret(l.X * d, l.Y * d, l.Z * d);

		return Ret;
	}
	inline ZSVECTOR3 operator/(const ZSVECTOR3& l, const float& d)
	{
		ZSVECTOR3 Ret(l.X / d, l.Y / d, l.Z / d);

		return Ret;
	}
    inline ZSVECTOR3 operator*(const ZSVECTOR3& l, const ZSVECTOR3& r)
    {
        ZSVECTOR3 Ret(l.X * r.X, l.Y * r.Y, l.Z * r.Z);

        return Ret;
    }
    inline ZSVECTOR3 operator/(const ZSVECTOR3& l, const ZSVECTOR3& r)
    {
        ZSVECTOR3 Ret(l.X / r.X, l.Y / r.Y, l.Z / r.Z);

        return Ret;
    }
    inline bool operator==(const ZSVECTOR3& l, const ZSVECTOR3& r)
    {
        bool result = false;
        if(l.X == r.X && l.Y == r.Y && l.Z == r.Z) result = true;

        return result;
    }
    inline bool operator!=(const ZSVECTOR3& l, const ZSVECTOR3& r)
    {
        bool result = false;
        if(l.X != r.X || l.Y != r.Y|| l.Z != r.Z) result = true;

        return result;
    }

#endif
