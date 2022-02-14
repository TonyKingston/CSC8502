#include "SceneNode.h"


SceneNode::SceneNode(Mesh* mesh, Vector4 colour) {
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;
	shader = NULL;
	material = NULL;
	animation = NULL;
	//matTextures = vector<GLuint>(0);
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
	currentFrame = 0;
	frameTime = 0.0f;
}

SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
	//delete[] matTextures;
	matTextures->clear();
	bumpTextures->clear();
}

void SceneNode::AddChild(SceneNode* s)
{
	children.push_back(s);
	s->parent = this;
}

void SceneNode::RemoveChild(int index)
{
	delete children[index];
	children[index] = NULL;
}

void SceneNode::Update(float dt)
{
	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else {
		worldTransform = transform;
	}
	if (animation) {
		frameTime -= dt;
		while (frameTime < 0.0f) {
			currentFrame = (currentFrame + 1) % animation->GetFrameCount();
			frameTime += 1.0f / animation->GetFrameRate();
		}
	}
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(dt);
	}
}

void SceneNode::Draw(const OGLRenderer& r)
{
	if (mesh) {
		if (mesh->GetSubMeshCount() > 0) {
			for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, (*matTextures)[i]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, (*bumpTextures)[i]);
				if (isInstanced) {
					mesh->DrawSubMeshInstanced(i, 10);
				}
				else {
					mesh->DrawSubMesh(i);
				}
			}
		}
		//mesh->Draw(); 
	}
}
