#pragma once 

#include <memory>
#include <string>
#include <functional>

#include "glm/glm.hpp"

#include "../Shader.h"
#include "../Properties/Render/ShadowsProperties.hpp"
#include "../VertexArray.hpp"
#include "../Material.hpp"
#include "../Camera.hpp"
#include "../Scene/Entity.hpp"


namespace glrenderer
{

enum class ERendererType : uint8_t
{
	FORWARD = 0,
	DEFERRED
};

enum class EViewportBufferState : uint8_t
{
	LightingTextured = 0, // Base type with lighting and textures

	Solid,
	Textured,
	Wireframe,

	// Deffered rendering
	Color,
	Spec,
	Position,
	Normal
};


/*
* Interface Renderer class with functions shared with all renderers
* 
* some functions provide a native implementation, others are to be implemented entirely in concrete renderers
*/
class IRenderer
{
public:
	using SendGlobalUniformCallback = std::function<void(const std::shared_ptr<Shader>& shader)>;

public:
	virtual ~IRenderer() {};

	// Called each frame for rendering scene
	void RenderScene(const std::shared_ptr<Camera>& camera, entt::registry& scene, const Entity& entitySelected);

public:
// Each renderer must override those functions
	virtual ERendererType GetType() const = 0;
	
	virtual void Render(entt::registry& scene, const Entity& entitySelected) = 0;
	
	virtual uint32_t GetMaximumLightCount() const = 0;

	// Called if the renderer was active during the application
	virtual void Free() = 0;

// End of mandatory override function

public:
// Generic Renderer functions
	// In the first start, we initialize renderer data
	virtual void Initialize() {}

	// Called on switch when this renderer is the new one
	virtual void Start() {}
	
	// Called on switch when this renderer is the old one
	virtual void Stop() {}

	virtual void Resize(uint32_t width, uint32_t height) {}

	virtual unsigned int GetBufferTextureId(EViewportBufferState Buffer) { return 0; }

	virtual void AddViewportBufferInList(std::vector<std::string>& buffers) {};
// End of generic Renderer functions


public:
// Render functions
	virtual void Clear();

	virtual void PreRender(const std::shared_ptr<Camera>& camera) {}

	virtual void PostRender() {}
// End of Render functions

public:

protected:
// Draw functions
	void DrawVAO(const std::shared_ptr<VertexArray>& vertexArray);
	
	void DrawLine(const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, bool selected);
// End of draw functions

};

}