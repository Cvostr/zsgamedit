#include "headers/zs-shader.h"

#include <GL/glew.h>

#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//#include "headers/zs-math.h"

void ZSPIRE::Shader::Init() {
	this->SHADER_ID = glCreateProgram();
}

void GLcheckCompileErrors(unsigned int shader, const char* type, const char* filepath = nullptr)
{

	GLint success;
	GLchar infoLog[1024];
	if (strcmp(type, "PROGRAM"))
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);

			if (filepath != nullptr)
				printf("%s :\n", filepath);

			std::cout << type << " Shader compilation error! " << infoLog;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);

			std::cout << "Shader program compilation error! " << infoLog;
		}
	}
}

bool readShaderFile(const char* path, char* result) {

	std::string res_data;

	std::ifstream stream;

	stream.exceptions(std::ifstream::badbit);

	try
	{
		stream.open(path);// Open file
		std::stringstream css;
		css << stream.rdbuf();

		stream.close();//close file stream
		res_data = css.str();
		strcpy(result, res_data.c_str());
	}
	catch (std::ifstream::failure e)
	{
		return false;
	}
	return true;
}

bool ZSPIRE::Shader::compileFromFile(const char* VSpath, const char* FSpath){
	char vsp[64];
	char fsp[64];

	strcpy(vsp, VSpath);
	strcpy(fsp, FSpath);


	std::cout << "OGL: Compiling shader " << vsp << " " << fsp << std::endl;

	Init();

	int VS = glCreateShader(GL_VERTEX_SHADER);
	int FS = glCreateShader(GL_FRAGMENT_SHADER);

	GLchar vs_data[4096];
	GLchar fs_data[4096];

	const GLchar* vs = &vs_data[0];
	const GLchar* fs = &fs_data[0];

	readShaderFile(vsp, &vs_data[0]);
	readShaderFile(fsp, &fs_data[0]);

	glShaderSource(VS, 1, &vs, NULL); //Setting shader code text on vs
	glShaderSource(FS, 1, &fs, NULL); //Setting shader code text on fs

	glCompileShader(VS); //Compile VS shader code
	GLcheckCompileErrors(VS, "VERTEX", VSpath); //Check vertex errors
	glCompileShader(FS); //Compile FS shader code
	GLcheckCompileErrors(FS, "FRAGMENT", FSpath); //Check fragment compile errors

	glAttachShader(this->SHADER_ID, VS);
	glAttachShader(this->SHADER_ID, FS);

	glLinkProgram(this->SHADER_ID);
	GLcheckCompileErrors(SHADER_ID, "PROGRAM");
	//Clear shaders, we don't need them anymore
	glDeleteShader(VS);
	glDeleteShader(FS);

	Use();
	setGLuniformInt("diffuse", 0);
	setGLuniformInt("normal_map", 1);

	setGLuniformInt("sprite_map", 0);
	setGLuniformInt("glyph_map", 1);

	setGLuniformInt("t_diffuse_0", 0);
	setGLuniformInt("t_diffuse_1", 1);
	setGLuniformInt("t_diffuse_2", 2);

	setGLuniformInt("shadow0", 20);
	setGLuniformInt("shadow1", 21);

	return true;

}

void ZSPIRE::Shader::Destroy() {
	glDeleteProgram(this->SHADER_ID);
}

void ZSPIRE::Shader::Use() {
	glUseProgram(this->SHADER_ID);
}

void ZSPIRE::Shader::setGLuniformMat4x4(const char* uniform_str, ZSMATRIX4x4 value) {
	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniformMatrix4fv(uniform_id, 1, GL_FALSE, &value.m[0][0]);
}

void ZSPIRE::Shader::setTransform(ZSMATRIX4x4 transform){
	setGLuniformMat4x4("object_transform", transform);
}

void ZSPIRE::Shader::setGLuniformColor(const char* uniform_str, ZSRGBCOLOR value) {

	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniform3f(uniform_id, value.gl_r, value.gl_g, value.gl_b);

}
void ZSPIRE::Shader::setGLuniformFloat(const char* uniform_str, float value) {

	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniform1f(uniform_id, value);

}

void ZSPIRE::Shader::setGLuniformVec3(const char* uniform_str, ZSVECTOR3 value){

	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniform3f(uniform_id, value.X, value.Y, value.Z);

}

void ZSPIRE::Shader::setGLuniformInt(const char* uniform_str, int value) {

	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniform1i(uniform_id, value);

}

void ZSPIRE::Shader::setHasDiffuseTextureProperty(bool hasDiffuseMap){
	this->setGLuniformInt("hasDiffuseMap", (int)hasDiffuseMap);
}

void ZSPIRE::Shader::setHasNormalTextureProperty(bool hasNormalMap){
	this->setGLuniformInt("hasNormalMap", (int)hasNormalMap);
}

void ZSPIRE::Shader::setTextureCountProperty(int tX, int tY) {
	this->setGLuniformInt("textures_x", tX);
	this->setGLuniformInt("textures_y", tY);
}
