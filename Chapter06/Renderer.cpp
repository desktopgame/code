// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include <algorithm>
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include <GL/glew.h>

Renderer::Renderer(Game* game)
	:mGame(game)
	,mSpriteShader(nullptr)
	,mMeshShaderMap()
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	// Set OpenGL attributes
	// Use the core OpenGL profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Specify version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 6)", 100, 100,
		static_cast<int>(mScreenWidth), static_cast<int>(mScreenHeight), SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create an OpenGL context
	mContext = SDL_GL_CreateContext(mWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW.");
		return false;
	}

	// On some platforms, GLEW will emit a benign error code,
	// so clear it
	glGetError();

	// Make sure we can create/compile shaders
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	// Create quad for drawing sprites
	CreateSpriteVerts();

	return true;
}

void Renderer::Shutdown()
{
	delete mSpriteVerts;
	mSpriteShader->Unload();
	delete mSpriteShader;
	for (auto kv : mMeshShaderMap) {
		kv.second->Unload();
	}
	mMeshShaderMap.clear();
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData()
{
	// Destroy textures
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();

	// Destroy meshes
	for (auto i : mMeshes)
	{
		i.second->Unload();
		delete i.second;
	}
	mMeshes.clear();
}

void Renderer::Draw()
{
	// Set the clear color to light grey
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh components
	// Enable depth buffering/disable alpha blend
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	std::unordered_map<std::string, std::vector<MeshComponent*> > cache;
	for (auto mc : mMeshComps)
	{
		Mesh* msh = mc->GetMesh();
		std::string mshName = msh->GetShaderName();
		if (!cache.count(mshName)) {
			std::vector<MeshComponent*> v;
			v.emplace_back(mc);
			cache[mshName] = v;
		} else {
			std::vector<MeshComponent*>& v = cache.at(mshName);
			v.emplace_back(mc);
		}
	}
	for (auto kv : cache) {
		auto sh = mMeshShaderMap[kv.first];
		auto comps = kv.second;
		// Set the mesh shader active
		sh->SetActive();
		// Update view-projection matrix
		sh->SetMatrixUniform("uViewProj", mView * mProjection);
		// Update lighting uniforms
		SetLightUniforms(sh);
		for (auto comp : comps) {
			if (kv.first == "PhongMesh2") {
				sh->SetIntUniform("uMaterial.mDiffuse", 0);
				sh->SetIntUniform("uMaterial.mSpecular",0);
			}
			comp->Draw(sh);
		}
	}

	// Draw all sprite components
	// Disable depth buffering
	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// Set shader/vao as active
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

void Renderer::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Renderer::AddMeshComp(MeshComponent* mesh)
{
	mMeshComps.emplace_back(mesh);
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
	auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
	mMeshComps.erase(iter);
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Mesh* Renderer::GetMesh(const std::string & fileName)
{
	Mesh* m = nullptr;
	auto iter = mMeshes.find(fileName);
	if (iter != mMeshes.end())
	{
		m = iter->second;
	}
	else
	{
		m = new Mesh();
		if (m->Load(fileName, this))
		{
			mMeshes.emplace(fileName, m);
		}
		else
		{
			delete m;
			m = nullptr;
		}
	}
	return m;
}

bool Renderer::LoadShaders()
{
	// Create sprite shader
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}

	mSpriteShader->SetActive();
	// Set the view-projection matrix
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	// Create phong mesh shader
	Shader* mMeshShader = nullptr;
	if ((mMeshShader = LoadShader("PhongMesh", "Shaders/Phong.vert", "Shaders/Phong.frag")) == nullptr) {
		return false;
	}

	mMeshShader->SetActive();
	// Set the view-projection matrix
	mView = Matrix4::CreateLookAt(Vector3::Zero, Vector3::UnitX, Vector3::UnitZ);
	mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		mScreenWidth, mScreenHeight, 25.0f, 10000.0f);
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

	// Create phongML mesh shader
	if ((mMeshShader = LoadShader("PhongMesh2", "Shaders/PhongML.vert", "Shaders/PhongML.frag")) == nullptr) {
		return false;
	}

	mMeshShader->SetActive();
	// Set the view-projection matrix
	mView = Matrix4::CreateLookAt(Vector3::Zero, Vector3::UnitX, Vector3::UnitZ);
	mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		mScreenWidth, mScreenHeight, 25.0f, 10000.0f);
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	const char* positionFmt = "pointLights[%d].mPosition";
	const char* constantFmt = "pointLights[%d].mConstant";
	const char* linearFmt = "pointLights[%d].mLinear";
	const char* quadraticFmt = "pointLights[%d].mQuadratic";
	const char* ambientFmt = "pointLights[%d].mAmbient";
	const char* diffuseFmt = "pointLights[%d].mDiffuse";
	const char* specularFmt = "pointLights[%d].mSpecular";
	const char* materialDiffuseFmt = "uMaterial.mDiffuse";
	const char* materialSpecularFmt = "uMaterial.mSpecular";
	const char* materialShininessFmt = "uMaterial.mShininess";
	mMeshShader->SetIntUniform(materialDiffuseFmt, 0);
	mMeshShader->SetIntUniform(materialSpecularFmt, 1);
	mMeshShader->SetFloatUniform(materialShininessFmt, 32);
	const float UNIT = 200.0f;
	Vector3 posTable[4] = {
		Vector3(200.0f, -75.0f, 0.0f + UNIT),
		Vector3(200.0f, -75.0f, 0.0f - UNIT),
		Vector3(200.0f + UNIT, -75.0f, 0.0f + UNIT),
		Vector3(200.0f - UNIT, -75.0f, 0.0f - UNIT),
	};
	Vector3 colorTable[4] = {
		Vector3(1, 0, 0),
		Vector3(0, 1, 0),
		Vector3(0, 0, 1),
		Vector3(1, 0, 0),
	};
	for (int i = 0; i < 4; i++) {
		char buf[512];
		std::memset(buf, '\0', 512);

		std::sprintf(buf, positionFmt, i);
		mMeshShader->SetVectorUniform(buf, posTable[i]);

		std::sprintf(buf, constantFmt, i);
		mMeshShader->SetFloatUniform(buf, 1.0f);

		std::sprintf(buf, linearFmt, i);
		mMeshShader->SetFloatUniform(buf, 1.0f);

		std::sprintf(buf, quadraticFmt, i);
		mMeshShader->SetFloatUniform(buf, 1.0f);

		std::sprintf(buf, ambientFmt, i);
		mMeshShader->SetVectorUniform(buf, colorTable[i]);

		std::sprintf(buf, diffuseFmt, i);
		mMeshShader->SetVectorUniform(buf, colorTable[i]);

		std::sprintf(buf, specularFmt, i);
		mMeshShader->SetVectorUniform(buf, colorTable[i]);
	}

	// Create basic mesh shader
	if ((mMeshShader = LoadShader("BasicMesh", "Shaders/BasicMesh.vert", "Shaders/BasicMesh.frag")) == nullptr) {
		return false;
	}

	mMeshShader->SetActive();
	// Set the view-projection matrix
	mView = Matrix4::CreateLookAt(Vector3::Zero, Vector3::UnitX, Vector3::UnitZ);
	mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		mScreenWidth, mScreenHeight, 25.0f, 10000.0f);
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	return true;
}

Shader* Renderer::LoadShader(const std::string & name, const std::string & vertFile, const std::string & fragFile) {
	Shader* sh = new Shader();
	if (sh->Load(vertFile, fragFile)) {
		mMeshShaderMap[name] = sh;
		return sh;
	}
	sh->Unload();
	delete sh;
	return nullptr;
}

void Renderer::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
		0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
		0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
		-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// Camera position is from inverted view
	Matrix4 invView = mView;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	// Ambient light
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);
	// Directional light
	shader->SetVectorUniform("uDirLight.mDirection",
		mDirLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor",
		mDirLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor",
		mDirLight.mSpecColor);
}
