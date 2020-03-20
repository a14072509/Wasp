#include "Skeleton.h"

bool Skeleton::Load(const char* filename) {

	Tokenizer token;
	token.Open(filename);
	token.FindToken("balljoint");

	// Parse tree
	Root = new Joint;
	token.GetToken(Root->name);
	allJoints.push_back(Root);
	Root->Load(token, allJoints);

	// Finish
	token.Close();
	return true;
}

void Skeleton::Draw(const glm::mat4& viewProjMtx, uint shader) {
	Root->Draw(viewProjMtx, shader);
}

void Skeleton::Update() {
	Root->Update();
}
