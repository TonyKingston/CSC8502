#include "SceneNode.h"


SceneNode::SceneNode(Mesh* mesh, Vector4 colour) {
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;
	shader = NULL;
	material = NULL;
	//matTextures = vector<GLuint>(0);
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
}

SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
	//delete[] matTextures;
}

void SceneNode::AddChild(SceneNode* s)
{
	children.push_back(s);
	s->parent = this;
}

void SceneNode::RemoveChild(int index)
{
	delete children[index];
}

void SceneNode::Update(float dt)
{
	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else {
		worldTransform = transform;
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
				mesh->DrawSubMesh(i);
			}
		}
		//mesh->Draw(); 
	}
}
