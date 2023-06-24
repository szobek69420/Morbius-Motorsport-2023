#include <glad/glad.h>
#include <iostream>
#include "shader.h"
#include <stb_image.h>

const char* vs_cube =
"#version 330 core\n"
"layout (location=0) in vec3 inPos;\n"
"layout (location=1) in vec2 inTexCoords;\n"
"layout (location=2) in float blockBrightness;\n"
"\n"
"out vec2 TexCoords;\n"
"out float outBrightness;\n"
"out float fogStrength;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"uniform float renderEnd;\n"
"uniform float renderEnd_water;\n"
"uniform vec3 cameraPos;\n"
"uniform int isCameraInWater;\n"
"\n"
"void main(){\n"
"\n"
"gl_Position=projection*view*model*vec4(inPos,1.0);\n"
"\n"
"outBrightness=blockBrightness;\n"
"\n"
"float distance=length(cameraPos-inPos);\n"
"\n"
"if(isCameraInWater==0){\n"
"if(distance<renderEnd-20.0) fogStrength=0.0;\n"
"else if(distance<64.0) fogStrength=1.0-((renderEnd-distance)/20.0);\n"
"else fogStrength=1.0;\n"
"}\n"
"else{\n"
"if(distance<renderEnd_water) fogStrength=distance/renderEnd_water;\n"
"else fogStrength=1.0;\n"
"}\n"
"\n"
"TexCoords=inTexCoords;\n"
"}\0";

const char* fs_cube =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"in float outBrightness;\n"
"in float fogStrength;\n"
"out vec4 FragmentColour;\n"
"uniform vec4 clearColor;\n"
"uniform vec4 waterColor;\n"
"uniform sampler2D atlas;\n"
"uniform int isCamInWater;\n"
"void main(){\n"
"if(fogStrength>0.99) discard;\n"
"FragmentColour=texture(atlas,TexCoords);\n"//vec4(0.5,0.1,0.4,1.0)
"if(FragmentColour.w<0.1) discard;\n"
"FragmentColour=vec4(vec3(FragmentColour)*outBrightness,FragmentColour.w);\n"
"if(isCamInWater!=0) FragmentColour=mix(FragmentColour,waterColor, fogStrength);\n"
"else FragmentColour=mix(FragmentColour,clearColor, fogStrength);\n"
"}\0";



const char* vs_water =
"#version 330 core\n"
"layout (location=0) in vec3 inPos;\n"
"layout (location=1) in vec2 inTexCoords;\n"
"layout (location=2) in float blockBrightness;\n"
"\n"
"out vec3 Position_gs;//geometry shadernek megy\n"
"\n"
"out vec2 TexCoords_gs;//fragment shadernek megy\n"
"out float outBrightness_gs;//fragment shadernek megy\n"
"out float fogStrength_gs;//fragment shadernek megy\n"
"\n"
"uniform float renderEnd;\n"
"uniform float renderEnd_water;\n"
"uniform vec3 cameraPos;\n"
"uniform int isCameraInWater;\n"
"\n"
"void main(){\n"
"\n"
"gl_Position=vec4(inPos,1.0);\n"
"\n"
"outBrightness_gs=blockBrightness;\n"
"\n"
"float distance=length(cameraPos-inPos);\n"
"\n"
"if(isCameraInWater==0){\n"
"if(distance<renderEnd-20.0) fogStrength_gs=0.0;\n"
"else if(distance<64.0) fogStrength_gs=1.0-((renderEnd-distance)/20.0);\n"
"else fogStrength_gs=1.0;\n"
"}\n"
"else{\n"
"if(distance<renderEnd_water) fogStrength_gs=distance/renderEnd_water;\n"
"else fogStrength_gs=1.0;\n"
"}\n"
"\n"
"TexCoords_gs=inTexCoords;\n"
"}\n"
"\0";

const char* gs_water =
"#version 330 core\n"
"layout (triangles) in;\n"
"layout (triangle_strip, max_vertices=3) out;\n"
"\n"
"in vec2 TexCoords_gs[];\n"
"in float outBrightness_gs[];\n"
"in float fogStrength_gs[];\n"
"\n"
"out vec3 Normal;\n"
"out vec3 Position;\n"
"out vec2 TexCoords;\n"
"out float outBrightness;\n"
"out float fogStrength;\n"
"\n"
"uniform mat4 combined_gs;\n"
"uniform float time;\n"
"\n"
"\n"
"void main(){\n"
"\n"
"//elokeszulet\n"
"vec3 modified_pos[3];\n"//a vec3 modified_pos[gl_in.length()]; nem futott le
"\n"
"for(int i=0;i<gl_in.length();i++){\n"
"modified_pos[i]=vec3(gl_in[i].gl_Position);\n"
"modified_pos[i].y+=0.075*sin(time+modified_pos[i].z)+0.075*sin(time+(modified_pos[i].z+modified_pos[i].x)*0.5);}\n"
"\n"
"Normal = normalize(cross(modified_pos[0] - modified_pos[1], modified_pos[2] - modified_pos[1]));\n"
"\n"
"//emission\n"
"for(int i=0;i<gl_in.length();i++){\n"
"Position=modified_pos[i];\n"
"TexCoords=TexCoords_gs[i];\n"
"outBrightness=outBrightness_gs[i];\n"
"fogStrength=fogStrength_gs[i];\n"
"gl_Position=combined_gs*vec4(modified_pos[i],1.0);"
"EmitVertex();}\n"
"\n"
"EndPrimitive();\n"
"}\n"
"\n"
"\0";

const char* fs_water =
"#version 330 core\n"
"in vec3 Position;\n"
"in vec3 Normal;\n"
"in vec2 TexCoords;\n"
"in float outBrightness;\n"
"in float fogStrength;\n"
"\n"
"out vec4 FragmentColour;\n"
"\n"
"uniform vec4 clearColor;\n"
"uniform vec4 waterColor;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 camPos;\n"
"uniform sampler2D atlas;\n"
"uniform sampler2D specularAtlas;\n"
"uniform int isCamInWater;\n"
"\n"
"uniform float shininess;\n"
"\n"
"void main(){\n"
"if(fogStrength>0.99) discard;\n"
"vec3 lightDir=normalize(Position-(camPos+lightPos));\n"
"vec3 specular=vec3(0.0);\n"
"if(gl_FrontFacing==false)//specular lightning\n"
"{\n"
"	//specular=pow(max(dot(reflect(-lightDir,Normal),normalize(vec3(Position-camPos))),0.0),shininess)*vec3(texture(specularAtlas,TexCoords));\n"//texture(specularAtlas,TexCoords)
"	specular=pow(max(dot(reflect(-lightDir,Normal),normalize(vec3(Position-camPos))),0.0),shininess)*vec3(1.0,1.0,1.0);\n"
"}\n"
"\n"
"FragmentColour=texture(atlas,TexCoords);\n"//vec4(0.5,0.1,0.4,1\n"
"\n"
"FragmentColour=vec4((FragmentColour.xyz+specular)*outBrightness*max(dot(-lightDir,Normal),0.3),FragmentColour.w);\n"
"if(isCamInWater!=0) FragmentColour=mix(FragmentColour,waterColor, fogStrength);\n"
"else FragmentColour=mix(FragmentColour,clearColor, fogStrength);\n"
"}\n"
"\n"
"\n"
"\0";


const char* vs_selection =
"#version 330 core\n"
"layout (location=0) in vec3 inPos;\n"
"uniform vec3 worldPos;\n"
"uniform mat4 comboed;\n"
"void main(){\n"
"gl_Position=comboed*vec4(inPos+worldPos,1.0);\n"
"}\0";

const char* fs_selection =
"#version 330 core\n"
"out vec4 FragmentColour;\n"
"void main(){\n"
"FragmentColour=vec4(1.0,0.0,0.0,1.0);\n"
"}\0";


const char* vs_ui_old =
"#version 330 core\n"
"layout (location=0) in vec2 inPos;\n"
"layout (location=1) in vec2 inTexCoords;\n"
"\n"
"out vec2 TexCoords;\n"
"void main()\n"
"{\n"
"gl_Position=vec4(inPos.x, inPos.y, 0.0, 1.0);\n"
"TexCoords=inTexCoords;\n"
"}\n"
"\0";

const char* fs_ui_old =
"#version 330 core\n"
"\n"
"in vec2 TexCoords;\n"
"\n"
"out vec4 FragmentColour;\n"
"\n"
"uniform sampler2D ui_texture;\n"
"\n"
"void main()\n"
"{\n"
"FragmentColour=texture(ui_texture,TexCoords);\n"//"FragmentColour=texture(ui_texture,TexCoords);\n"
"}\n"
"\0";

const char* vs_ui =
"#version 330 core\n"
"\n"
"layout (location=0) in vec4 inData;\n"
"\n"
"out vec2 TexCoords;\n"
"\n"
"uniform vec4 position;//x,y: position, z,w: size\n"
"uniform vec4 uvposition;\n"
"\n"
"void main()\n"
"{\n"
"TexCoords.x=inData.z*uvposition.z+uvposition.x;\n"
"TexCoords.y=inData.w*uvposition.w+uvposition.y;\n"
"\n"
"gl_Position=vec4(inData.x*position.z+position.x,inData.y*position.w+position.y,0.5,1.0);\n"
"} \n"
"\0";

const char* fs_ui =
"#version 330 core\n"
"\n"
"in vec2 TexCoords;\n"
"\n"
"out vec4 FragmentColour;\n"
"\n"
"uniform sampler2D Texture;\n"
"uniform float dankness;\n"
"\n"
"void main()\n"
"{\n"
"FragmentColour=texture(Texture, TexCoords);\n"
"if(FragmentColour.w<0.05) \n"
"	discard;\n"
"FragmentColour=vec4(dankness*FragmentColour.xyz, FragmentColour.w);\n"
"\0";

const char* vs_ui2 =
"#version 330 core\n"
"\n"
"layout (location=0) in vec4 inData;\n"
"\n"
"out vec2 TexCoords;\n"
"\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"uniform mat4 uvmodel;\n"
"uniform int isText;\n"
"\n"
"void main()\n"
"{\n"
"vec4 uv=vec4(inData.z,inData.w,1.0,1.0);\n"
"uv=uvmodel*uv;\n"
"TexCoords.x=uv.x;\n"
"TexCoords.y=uv.y;\n"
"\n"
"vec4 temp=vec4(inData.x,inData.y,0.0,1.0);\n"
"if(isText!=0) temp.y=1.0-temp.y;\n"
"gl_Position=projection*model*temp;\n"
"} \n"
"\0";

const char* fs_ui2 =
"#version 330 core\n"
"\n"
"in vec2 TexCoords;\n"
"uniform int isText2;\n"
"\n"
"out vec4 FragmentColour;\n"
"\n"
"uniform sampler2D Texture;\n"
"uniform vec4 colour;\n"
"\n"
"void main()\n"
"{\n"
"if(isText2==0){\n"
"FragmentColour=texture(Texture, TexCoords);\n"
"}\n"//isText==0
"else{\n"
"FragmentColour=vec4(1.0, 1.0, 1.0, texture(Texture, TexCoords).r);\n"
"} \n"//else
"if(FragmentColour.w<0.01) discard;\n"
"FragmentColour=vec4(colour.x*FragmentColour.x,colour.y*FragmentColour.y,colour.z*FragmentColour.z, colour.w*FragmentColour.w);\n"
"} \n"
"\0";

const char* vs_sky =
"#version 330 core\n"
"\n"
"layout (location=0) in vec3 inPos;\n"
"layout (location=1) in vec2 inTexCoords;\n"
"\n"
"out vec2 TexCoords;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 pv;//projection und view matrices\n"
"uniform vec4 camPos;\n"
"uniform float size;\n"
"\n"
"void main()\n"
"{ \n"
"TexCoords=inTexCoords;\n"
"gl_Position=pv*(camPos+model*vec4(inPos.x,inPos.y*size,inPos.z*size,1.0));\n"
"} \n"
"\n"
"\0";

const char* fs_sky =
"#version 330 core\n"
"\n"
"in vec2 TexCoords;\n"
"\n"
"out vec4 FragmentColour;\n"
"\n"
"uniform sampler2D skyTexture;\n"
"\n"
"void main()\n"
"{ \n"
"FragmentColour=texture(skyTexture,TexCoords);\n"
"if(FragmentColour.w<0.1) discard;\n"
"} \n"
"\0";

const char* vs_held =
"#version 330 core\n"
"\n"
"layout (location=0) in vec3 inPos;\n"
"layout (location=1) in vec2 inUV;\n"
"layout (location=2) in float inDankness;\n"
"\n"
"out vec2 TexCoords;\n"
"out float dankness;\n"
"\n"
"uniform mat4 pvm;//projection*view*model\n"
"\n"
"void main()\n"
"{ \n"
"gl_Position=pvm*vec4(inPos,1.0);\n"
"TexCoords=inUV;\n"
"dankness=inDankness;\n"
"} \n"
"\n"
"\0";

const char* fs_held =
"#version 330 core\n"
"\n"
"in vec2 TexCoords;\n"
"in float dankness;\n"
"\n"
"out vec4 FragmentColour;\n"
"\n"
"uniform sampler2D atlas;\n"
"\n"
"void main()\n"
"{ \n"
"FragmentColour=dankness*texture(atlas,TexCoords);\n"
"if(FragmentColour.w<0.02) discard;\n"
"} \n"
"\n"
"\n"
"\0";

const char* vs_model =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoords;\n"
"\n"
"out vec2 TexCoords;\n"
"out vec3 Normal;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"	 Normal=aNormal;\n"
"    TexCoords = aTexCoords;    \n"
"    gl_Position = projection* view * model * vec4(aPos, 1.0);\n"
"}\n"
"\0";

const char* fs_model =
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec2 TexCoords;\n"
"in vec3 Normal;\n"
"\n"
"uniform sampler2D texture_diffuse1;\n"
"uniform vec3 lightDir;//from (0,0,0) to the light, normalized\n"
"\n"
"void main()\n"
"{    \n"
"    FragColor = texture(texture_diffuse1, TexCoords);\n"
"	 if(FragColor.w<0.01) discard;\n"
"	 if(FragColor.x<0.98||FragColor.y>0.02||FragColor.z>0.02) FragColor=((max(dot(lightDir, normalize(Normal)),0))*0.8+0.2)*FragColor;\n"
"}\n"
"\0";


const char* vs_sprite =
"#version 330 core\n"
"layout (location=0) in vec3 inPos;\n"
"layout (location=1) in vec2 inTexCoords;\n"
"\n"
"uniform mat4 textureMatrix;\n"
"uniform mat4 modelMatrix;\n"
"uniform mat4 viewMatrix;\n"
"uniform mat4 projectionMatrix;\n"
"\n"
"out vec2 TexCoords;\n"
"\n"
"void main()\n"
"{ \n"
"gl_Position=vec4(inPos,1.0);\n"
"gl_Position=projectionMatrix*viewMatrix*modelMatrix*gl_Position;\n"
"\n"
"vec4 tempTexCoords=vec4(inTexCoords,0.0,1.0);\n"
"tempTexCoords=textureMatrix*tempTexCoords;\n"
"TexCoords=tempTexCoords.xy;\n"
"} \n"
"\n"
"\0";


const char* fs_sprite =
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec2 TexCoords;\n"
"\n"
"uniform sampler2D sprite;\n"
"\n"
"void main()\n"
"{    \n"
"    FragColor = texture(sprite, TexCoords);\n"
"}\n"
"\0";



GLuint createShader(enum shadertype shader)
{
	unsigned int program=0;

	unsigned int vs = 0, gs = 0, fs = 0;
	bool containsGS = false;

	int  success;
	char infoLog[512];



	program = glCreateProgram();

	
	vs = glCreateShader(GL_VERTEX_SHADER);
	gs = glCreateShader(GL_GEOMETRY_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	//sources
	switch (shader)
	{
	case SHADER_CUBE:
		std::cout << "SHADER_CUBE\n";
		glShaderSource(vs, 1, &vs_cube, NULL);
		glShaderSource(fs, 1, &fs_cube, NULL);
		break;

	case SHADER_WATER:
		std::cout << "SHADER_WATER\n";
		containsGS = true;
		glShaderSource(vs, 1, &vs_water, NULL);
		glShaderSource(gs, 1, &gs_water, NULL);
		glShaderSource(fs, 1, &fs_water, NULL);
		break;

	case SHADER_SELECTION:
		std::cout << "SHADER_SELECTION\n";
		glShaderSource(vs, 1, &vs_selection, NULL);
		glShaderSource(fs, 1, &fs_selection, NULL);
		break;

	case SHADER_UI_OLD:
		std::cout << "SHADER_UI_OLD\n";
		glShaderSource(vs, 1, &vs_ui_old, NULL);
		glShaderSource(fs, 1, &fs_ui_old, NULL);
		break;

	case SHADER_UI:
		std::cout << "SHADER_UI\n";
		glShaderSource(vs, 1, &vs_ui, NULL);
		glShaderSource(fs, 1, &fs_ui, NULL);
		break;

	case SHADER_UI_IMAGE:
		std::cout << "SHADER_UI_IMAGE\n";
		glShaderSource(vs, 1, &vs_ui2, NULL);
		glShaderSource(fs, 1, &fs_ui2, NULL);
		break;

	case SHADER_UI_TEXT:
		std::cout << "SHADER_UI_TEXT\n";
		glShaderSource(vs, 1, &vs_ui2, NULL);
		glShaderSource(fs, 1, &fs_ui2, NULL);
		break;

	case SHADER_SKY:
		std::cout << "SHADER_SKY\n";
		glShaderSource(vs, 1, &vs_sky, NULL);
		glShaderSource(fs, 1, &fs_sky, NULL);
		break;

	case SHADER_HELD_ITEM:
		std::cout << "SHADER_HELD_ITEM\n";
		glShaderSource(vs, 1, &vs_held, NULL);
		glShaderSource(fs, 1, &fs_held, NULL);
		break;

	case SHADER_MODEL:
		std::cout << "SHADER_MODEL\n";
		glShaderSource(vs, 1, &vs_model, NULL);
		glShaderSource(fs, 1, &fs_model, NULL);
		break;

	case SHADER_SPRITE:
		std::cout << "SHADER_SPRITE\n";
		glShaderSource(vs, 1, &vs_sprite, NULL);
		glShaderSource(fs, 1, &fs_sprite, NULL);
		break;
	}

	//assembly
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;}

	if (containsGS)
	{
		glCompileShader(gs);

		glGetShaderiv(gs, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(gs, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;}
	}

	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;}


	glAttachShader(program, vs);
	if (containsGS)
		glAttachShader(program, gs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINKING::FUCK\n" << infoLog << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	std::cout << "Shader compilation successful" << std::endl;

	return program;
}

void destroyShader(unsigned int shader)
{
	glDeleteProgram(shader);
}

//image
GLuint TextureImporter::CreateTexture(const char* textureName, bool RGBA)
{
	unsigned int texture = 0;

	//glActiveTexture(GL_TEXTURE15);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load(textureName, &width, &height, &nrChannels, 0);
	if (data)
	{
		if(RGBA)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 0;
	}
	stbi_image_free(data);

	return texture;
}

void TextureImporter::DestroyTexture(unsigned int texture)
{
	glDeleteTextures(1, &texture);
}