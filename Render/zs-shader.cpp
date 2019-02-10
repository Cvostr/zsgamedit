#include "headers/zs-shader.h"
#include "../World/headers/World.h"
#include "../World/headers/obj_properties.h"
#include <GL/glew.h>

#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

int cur_shader_gl_id = -1;

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

    setGLuniformInt("tDiffuse", 10);
    setGLuniformInt("tNormal", 11);
    setGLuniformInt("tPos", 12);

	setGLuniformInt("shadow0", 20);
	setGLuniformInt("shadow1", 21);

    this->isCreated = true; //Shader created & compiled now
	return true;

}

void ZSPIRE::Shader::Destroy() {
	glDeleteProgram(this->SHADER_ID);
    this->isCreated = false;
}

void ZSPIRE::Shader::Use() {
    if(cur_shader_gl_id == this->SHADER_ID) return;

	glUseProgram(this->SHADER_ID);
    cur_shader_gl_id = this->SHADER_ID;
}

void ZSPIRE::Shader::setGLuniformMat4x4(const char* uniform_str, ZSMATRIX4x4 value) {
	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniformMatrix4fv(uniform_id, 1, GL_FALSE, &value.m[0][0]);
}

void ZSPIRE::Shader::setTransform(ZSMATRIX4x4 transform){
	setGLuniformMat4x4("object_transform", transform);
}

void ZSPIRE::Shader::setCamera(Camera* cam){
    setGLuniformMat4x4("cam_projection", cam->getProjMatrix());
    setGLuniformMat4x4("cam_view", cam->getViewMatrix());
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

void ZSPIRE::Shader::setGLuniformVec4(const char* uniform_str, ZSVECTOR4 value){

    unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
    glUniform4f(uniform_id, value.X, value.Y, value.Z, value.W);

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

void ZSPIRE::Shader::sendLight(unsigned int index, void* _light){
    LightsourceProperty* light = static_cast<LightsourceProperty*>(_light);
    if (light->light_type > 0) {
        std::string id_s = std::to_string(index);

        std::string type;
        type = "lights[" + id_s + "].type";

        std::string pos;
        pos = "lights[" + id_s + "].pos";

        std::string color;
        color = "lights[" + id_s + "].color";

        std::string dir;
        dir = "lights[" + id_s + "].dir";

        std::string range;
        range = "lights[" + id_s + "].range";

        std::string intensity;
        intensity = "lights[" + id_s + "].intensity";

        //std::string spot_angle;
        //spot_angle = "lights[" + std::to_string(index) + "].spot_angle";

        //std::string spot_oangle;
        //spot_oangle = "lights[" + std::to_string(index) + "].spot_out_angle";

        setGLuniformInt(type.c_str(), (int)light->light_type);
        setGLuniformVec3(pos.c_str(), light->transform->_last_translation);
        setGLuniformVec3(dir.c_str(), light->direction);
        setGLuniformFloat(range.c_str(), light->range);
        setGLuniformFloat(intensity.c_str(), light->intensity);
        //setGLuniformFloat(spot_angle.c_str(), light->spot_angle_rad);
        //setGLuniformFloat(spot_oangle.c_str(), light->outrad);
        setGLuniformColor(color.c_str(), light->color);
    }
}

void ZSPIRE::Shader::unsetLight(unsigned int index){
    std::string type;
    type = "lights[" + std::to_string(index) + "].type";

    setGLuniformInt(type.c_str(), 0);
}
