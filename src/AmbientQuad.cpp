#include <stdio.h>
#include <iostream>
#include <vector>

#include "helpers/ProgramUtilities.h"
#include "helpers/GenerationUtilities.h"

#include "AmbientQuad.h"

AmbientQuad::AmbientQuad(){}

AmbientQuad::~AmbientQuad(){}

void AmbientQuad::init(std::map<std::string, GLuint> textureIds){
	
	// Ambient pass: needs the albedo, the normals and the AO result
	std::map<std::string, GLuint> finalTextures = { {"albedoTexture", textureIds["albedoTexture"]}, {"normalTexture", textureIds["normalTexture"]}, {"ssaoTexture", textureIds["ssaoTexture"]}};
	ScreenQuad::init(finalTextures, "ressources/shaders/gbuffer/ambient");
	_texCubeMapSmall = loadTextureCubeMap("ressources/cubemap/cubemap_diff",
										  _programId, (GLuint)_textureIds.size(),
										  "textureCubeMapSmall", true);
	
	// Setup SSAO data, get back noise texture id, add it to the gbuffer outputs.
	GLuint noiseTextureID = setupSSAO();
	std::map<std::string, GLuint> ssaoTextures = { {"depthTexture", textureIds["depthTexture"]}, {"normalTexture", textureIds["normalTexture"]}, {"noiseTexture",noiseTextureID}};
	_ssaoScreen.init(ssaoTextures, "ressources/shaders/gbuffer/ssao");
	
	
	
}

GLuint AmbientQuad::setupSSAO(){
	// Samples.
	// We need random vectors in the half sphere above z, with more samples close to the center.
	
	for(int i = 0; i < 16; ++i){
		glm::vec3 randVec = glm::vec3(Random::Float(-1.0f, 1.0f),
									  Random::Float(-1.0f, 1.0f),
									  Random::Float(0.0f, 1.0f) );
		_samples.push_back(glm::normalize(randVec));
		_samples.back() *= Random::Float(0.0f,1.0f);
		// Skew the distribution towards the center.
		float scale = i/16.0;
		scale = 0.1f+0.9f*scale*scale;
		_samples.back() *= scale;
	}
	
	// Noise texture (same size as the box blur applied after SSAO computation).
	// We need to generate two dimensional normalized offsets.
	std::vector<glm::vec3> noise;
	for(int i = 0; i < 25; ++i){
		glm::vec3 randVec = glm::vec3(Random::Float(-1.0f, 1.0f),
									  Random::Float(-1.0f, 1.0f),
									  0.0f);
		noise.push_back(glm::normalize(randVec));
	}
	
	// Send the texture to the GPU.
	GLuint textureId;
	return textureId;
}

void AmbientQuad::draw(const glm::vec2& invScreenSize, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	
	glm::mat4 invView = glm::inverse(viewMatrix);
	
	glUseProgram(_programId);
	
	GLuint invVID  = glGetUniformLocation(_programId, "inverseV");
	glUniformMatrix4fv(invVID, 1, GL_FALSE, &invView[0][0]);
	
	glActiveTexture(GL_TEXTURE0 + (unsigned int)_textureIds.size());
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texCubeMapSmall);
	
	ScreenQuad::draw(invScreenSize);
}

void AmbientQuad::drawSSAO(const glm::vec2& invScreenSize, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	
	// Projection parameter for position reconstruction.
	glm::vec4 projectionVector = glm::vec4(projectionMatrix[0][0], projectionMatrix[1][1], projectionMatrix[2][2], projectionMatrix[3][2]);
	
	glUseProgram(_ssaoScreen.program());
	
	GLuint projId = glGetUniformLocation(_ssaoScreen.program(), "projectionMatrix");
	glUniform4fv(projId, 1, &(projectionVector[0]));
	
	_ssaoScreen.draw(invScreenSize);
	
}



void AmbientQuad::clean(){
	ScreenQuad::clean();
	_ssaoScreen.clean();
}
