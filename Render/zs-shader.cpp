#include "headers/zs-shader.h"
#include "../World/headers/World.h"
#include "../World/headers/obj_properties.h"
#include <GL/glew.h>

#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define SHADER_STR_LEN 16384

ZSPIRE::Shader::Shader(){
    isCreated = false; //Not created by default
}

void ZSPIRE::Shader::Init() {
    this->SHADER_ID = glCreateProgram(); //Call OGL function to create new shader
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
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

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
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);

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

    std::cout << "OGL: Compiling shader " << VSpath << " " << FSpath << std::endl;

	Init();

    unsigned int VS = glCreateShader(GL_VERTEX_SHADER);
    unsigned int FS = glCreateShader(GL_FRAGMENT_SHADER);

    GLchar vs_data[SHADER_STR_LEN];
    GLchar fs_data[SHADER_STR_LEN];

	const GLchar* vs = &vs_data[0];
	const GLchar* fs = &fs_data[0];

    readShaderFile(VSpath, &vs_data[0]);
    readShaderFile(FSpath, &fs_data[0]);

    glShaderSource(VS, 1, &vs, nullptr); //Setting shader code text on vs
    glShaderSource(FS, 1, &fs, nullptr); //Setting shader code text on fs

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

    this->isCreated = true; //Shader created & compiled now
	return true;

}

bool ZSPIRE::Shader::compileFromStr(const char* _VS, const char* _FS){
    std::cout << "OGL: Compiling shader " << std::endl;

    Init();

    unsigned int VS = glCreateShader(GL_VERTEX_SHADER);
    unsigned int FS = glCreateShader(GL_FRAGMENT_SHADER);

    GLchar vs_data[SHADER_STR_LEN];
    GLchar fs_data[SHADER_STR_LEN];

    strcpy(vs_data, _VS);
    strcpy(fs_data, _FS);

    const GLchar* vs = &vs_data[0];
    const GLchar* fs = &fs_data[0];


    glShaderSource(VS, 1, &vs, nullptr); //Setting shader code text on vs
    glShaderSource(FS, 1, &fs, nullptr); //Setting shader code text on fs

    glCompileShader(VS); //Compile VS shader code
    GLcheckCompileErrors(VS, "VERTEX", "VSpath"); //Check vertex errors
    glCompileShader(FS); //Compile FS shader code
    GLcheckCompileErrors(FS, "FRAGMENT", "FSpath"); //Check fragment compile errors

    glAttachShader(this->SHADER_ID, VS);
    glAttachShader(this->SHADER_ID, FS);

    glLinkProgram(this->SHADER_ID);
    GLcheckCompileErrors(SHADER_ID, "PROGRAM");
    //Clear shaders, we don't need them anymore
    glDeleteShader(VS);
    glDeleteShader(FS);

    this->isCreated = true; //Shader created & compiled now
    return true;
}

void ZSPIRE::Shader::Destroy() {
	glDeleteProgram(this->SHADER_ID);
    this->isCreated = false;
}

void ZSPIRE::Shader::Use() {
    //if(cur_shader_gl_id == this->SHADER_ID) return;

	glUseProgram(this->SHADER_ID);
    //cur_shader_gl_id = this->SHADER_ID;
}

unsigned int ZSPIRE::Shader::getUniformBufferIndex(const char* UB_NAME){
    return glGetUniformBlockIndex(this->SHADER_ID, UB_NAME);
}

void ZSPIRE::Shader::setUniformBufferBinding(unsigned int UB_INDEX, unsigned int binding){
    glUniformBlockBinding(this->SHADER_ID, UB_INDEX, binding);
}

void ZSPIRE::Shader::setGLuniformColor(const char* uniform_str, ZSRGBCOLOR value) {
    int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
    glUniform3f(uniform_id, value.gl_r, value.gl_g, value.gl_b);
}

void ZSPIRE::Shader::setGLuniformVec4(const char* uniform_str, ZSVECTOR4 value){

    int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
    glUniform4f(uniform_id, value.X, value.Y, value.Z, value.W);

}

