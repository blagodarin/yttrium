#ifndef __GUI_MANAGER_H
#define __GUI_MANAGER_H

#include <Yttrium/gui/manager.h>
#include <Yttrium/renderer.h>
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

class Scene;

class ManagerImpl
	: public Manager
{
public:

	ManagerImpl(const Renderer &renderer, Allocator *allocator);

	virtual ~ManagerImpl() noexcept;

public:

	bool add_scene(Scene *scene, bool is_root);

	Scene *create_scene(const StaticString &name);

	void delete_scene(Scene *scene);

	void set_font(const StaticString &name, const StaticString &font_source, const StaticString &texture_name);

	inline void set_scaling(Scaling scaling);

	void set_scene_change_action(const StaticString &from_scene,
		const StaticString &to_scene, const StaticString &action);

	inline void set_size(const Vector2f &size);

public: // Manager.

	virtual void clear() noexcept;

	virtual bool has_scene(const StaticString &name) const noexcept;

	virtual bool load(const StaticString &source) noexcept;

	virtual bool pop_scenes(size_t count) noexcept;

	virtual bool push_scene(const StaticString &name) noexcept;

	virtual bool process_key(Terminal *terminal, Key key, KeyState state) noexcept;

	virtual bool render() noexcept;

	virtual void set_cursor(const Vector2f &cursor) noexcept;

private:

	void change_scene(const StaticString &old_scene, const StaticString &new_scene);

	bool load(const StaticString &name, bool internal);

	void load(const Ion::Object &parent);

private:

	struct FontDesc
	{
		TextureFont font;
		Texture2D   texture;

		FontDesc() = default;

		FontDesc(const TextureFont &font, const Texture2D &texture)
			: font(font)
			, texture(texture)
		{
		}
	};

	typedef std::map<String, FontDesc>      Fonts;
	typedef std::map<StaticString, Scene *> Scenes;
	typedef std::vector<Scene *>            SceneStack;
	typedef std::pair<String, String>       ScenePair;
	typedef std::map<ScenePair, String>     SceneActions;

private:

	Renderer     _renderer;
	bool         _has_size;
	Vector2f     _size;
	Scaling      _scaling;
	Fonts        _fonts;
	Scenes       _scenes;
	SceneStack   _scene_stack;
	SceneActions _scene_actions;
	bool         _has_cursor;
	Vector2f     _cursor;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ManagerImpl::set_scaling(Scaling scaling)
{
	_scaling = scaling;
}

void ManagerImpl::set_size(const Vector2f &size)
{
	_size = size;
	_has_size = true;
}

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_MANAGER_H
