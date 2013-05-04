#ifndef __GUI_MANAGER_H
#define __GUI_MANAGER_H

#include <Yttrium/gui/manager.h>
#include <Yttrium/proxy_allocator.h>
#include <Yttrium/renderer.h>
#include <Yttrium/renderer/texture_cache.h>
#include <Yttrium/texture_font.h>

#include "types.h"

#include <map>
#include <vector>

namespace Yttrium
{

namespace Ion
{

class Object;

} // namespace Ion

namespace Gui
{

class IonDumper;
class Scene;

class ManagerImpl
	: public Manager
{
	friend IonDumper;

public:

	struct FontDesc
	{
		TextureFont font;
		Texture2D   texture;

		FontDesc() {}

		FontDesc(const TextureFont &font, const Texture2D &texture)
			: font(font)
			, texture(texture)
		{
		}
	};

public:

	ManagerImpl(const Renderer &renderer, Callbacks *callbacks, Allocator *allocator);

	~ManagerImpl() noexcept override;

public:

	bool add_scene(Scene *scene, bool is_root);

	inline Callbacks *callbacks() const;

	Scene *create_scene(const StaticString &name);

	void delete_scene(Scene *scene);

	const FontDesc *font(const StaticString &name) const;

	inline Allocator *internal_allocator() const;

	inline Renderer *renderer();

	void set_font(const StaticString &name, const StaticString &font_source, const StaticString &texture_name);

	inline void set_scaling(Scaling scaling);

	void set_scene_change_action(const String &from_scene, const String &to_scene, const String &action);

	inline void set_size(const Vector2f &size);

	inline TextureCache *texture_cache();

public: // Manager.

	void clear() noexcept override;

	void dump(const StaticString &filename) const noexcept override;

	bool has_scene(const StaticString &name) const noexcept override;

	bool load(const StaticString &filename) noexcept override;

	bool pop_scenes(size_t count) noexcept override;

	bool push_scene(const StaticString &name) noexcept override;

	bool process_key(Terminal *terminal, Key key, KeyState state) noexcept override;

	bool render() noexcept override;

	void set_cursor(const Vector2f &cursor) noexcept override;

private:

	void change_scene(const StaticString &old_scene, const StaticString &new_scene);

	bool load(const StaticString &name, bool internal);

	void load(const Ion::Object &parent);

private:

	typedef std::map<String, FontDesc>      Fonts;
	typedef std::map<StaticString, Scene *> Scenes;
	typedef std::vector<Scene *>            SceneStack;
	typedef std::pair<String, String>       ScenePair;
	typedef std::map<ScenePair, String>     SceneActions;

private:

	ProxyAllocator   _proxy_allocator;
	Renderer         _renderer;
	TextureCachePtr  _texture_cache;
	Callbacks       *_callbacks;
	bool             _has_size;
	Vector2f         _size;
	Scaling          _scaling;
	Fonts            _fonts;
	Scenes           _scenes;
	SceneStack       _scene_stack;
	SceneActions     _scene_actions;
	bool             _has_cursor;
	Vector2f         _cursor;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Allocator *ManagerImpl::internal_allocator() const
{
	return const_cast<ProxyAllocator *>(&_proxy_allocator);
}

Manager::Callbacks *ManagerImpl::callbacks() const
{
	return _callbacks;
}

Renderer *ManagerImpl::renderer()
{
	return &_renderer;
}

void ManagerImpl::set_scaling(Scaling scaling)
{
	_scaling = scaling;
}

void ManagerImpl::set_size(const Vector2f &size)
{
	_size = size;
	_has_size = true;
}

TextureCache *ManagerImpl::texture_cache()
{
	return _texture_cache.pointer();
}

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_MANAGER_H
