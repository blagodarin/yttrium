#include "logging.h"

#include "manager.h"

#include <Yttrium/allocator.h>
#include <Yttrium/script/manager.h>

#include "ion/dumper.h"
#include "ion/loader.h"
#include "scene.h"

namespace Yttrium
{

namespace Gui
{

ManagerImpl::ManagerImpl(const Renderer &renderer, Callbacks *callbacks, Allocator *allocator)
	: Manager(allocator)
	, _renderer(renderer)
	, _callbacks(callbacks)
	, _has_size(false)
	, _size(0)
	, _scaling(Scaling::Stretch)
	, _has_cursor(false)
{
}

ManagerImpl::~ManagerImpl()
{
	clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ManagerImpl::add_scene(Scene *scene, bool is_root)
{
	const String &scene_name = scene->name();

	if (_scenes.find(scene_name) != _scenes.end())
	{
		Y_LOG("[Gui.Manager] Scene \"" << scene_name << "\" has already been added");
		return false;
	}

	_scenes.insert(Scenes::value_type(scene_name, scene));

	if (is_root)
	{
		if (!_scene_stack.empty())
		{
			Y_LOG("[Gui.Manager] Scene \"" << scene_name << "\" \"root\" option ignored");
			// TODO: Print the existing root scene name.
		}
		else
		{
			_scene_stack.push_back(scene);
		}
	}

	return true;
}

Scene *ManagerImpl::create_scene(const StaticString &name)
{
	if (!_has_size)
	{
		Y_LOG("[Gui.Manager] Can't load scene \"" << name << "\": GUI size has not been set");
		return nullptr;
	}

	Scene *scene = Y_NEW(allocator(), Scene)(this, name, allocator());
	scene->set_size(_size);
	return scene;
}

void ManagerImpl::delete_scene(Scene *scene)
{
	Y_DELETE(allocator(), scene);
}

const ManagerImpl::FontDesc *ManagerImpl::font(const StaticString &name) const
{
	Fonts::const_iterator i = _fonts.find(String(name, ByReference()));
	return i != _fonts.end() ? &i->second : nullptr;
}

void ManagerImpl::set_font(const StaticString &name,
	const StaticString &font_source, const StaticString &texture_name)
{
	Texture2D texture = _renderer.texture_cache().load_texture_2d(texture_name);

	if (texture)
	{
		TextureFont font(font_source, allocator());

		if (font)
		{
			texture.set_filter(Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter);
			_fonts[name] = FontDesc(font, texture);
		}
	}
}

void ManagerImpl::set_scene_change_action(const StaticString &from_scene,
	const StaticString &to_scene, const StaticString &action)
{
	_scene_actions[ScenePair(from_scene, to_scene)] = action;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ManagerImpl::clear()
{
	for (Scenes::iterator i = _scenes.begin(); i != _scenes.end(); ++i)
	{
		delete_scene(i->second);
	}

	_has_size = false;
	_size = Vector2f(0);
	_fonts.clear();
	_scenes.clear();
	_scene_stack.clear();
	_scene_actions.clear();
	_has_cursor = false;
}

void ManagerImpl::dump(const StaticString &filename) const
{
	IonDumper(this).dump(filename);
}

bool ManagerImpl::has_scene(const StaticString &name) const
{
	return _scenes.find(String(name, ByReference())) != _scenes.end();
}

bool ManagerImpl::load(const StaticString &filename)
{
	clear();

	IonLoader loader(this);

	if (!loader.load(filename))
	{
		return false;
	}

	if (_scene_stack.empty())
	{
		Y_LOG("[Gui.Manager] No root scene has been added");
		clear();
		return false;
	}

	return true;
}

bool ManagerImpl::pop_scenes(size_t count)
{
	if (_scene_stack.empty())
	{
		return false;
	}

	Scene *old_scene = _scene_stack.back();

	if (count >= _scene_stack.size())
	{
		_scene_stack.clear();
		change_scene(old_scene->name(), StaticString());
		return false;
	}

	_scene_stack.resize(_scene_stack.size() - count);
	change_scene(old_scene->name(), _scene_stack.back()->name());
	return true;
}

bool ManagerImpl::push_scene(const StaticString &name)
{
	Scenes::iterator i = _scenes.find(String(name, ByReference()));

	if (i == _scenes.end())
	{
		return false;
	}

	Scene *old_scene = _scene_stack.back();

	_scene_stack.push_back(i->second);
	change_scene(old_scene->name(), name);
	return true;
}

bool ManagerImpl::process_key(Terminal *, Key key, KeyState state)
{
	if (_scene_stack.empty())
	{
		return false;
	}

	return _scene_stack.back()->process_key(key, state);
}

bool ManagerImpl::render()
{
	if (!_renderer || !_has_size || _scene_stack.empty())
	{
		return false;
	}

	SceneStack::iterator i = _scene_stack.begin() + (_scene_stack.size() - 1);

	if (_has_cursor)
	{
		(*i)->set_cursor(_cursor);
		_has_cursor = false;
	}

	while (i != _scene_stack.begin() && (*i)->is_transparent())
	{
		--i;
	}

	_renderer.set_matrix_2d(_size.x, _size.y);

	for (; i != _scene_stack.end(); ++i)
	{
		(*i)->render(&_renderer, _size);
	}

	return true;
}

void ManagerImpl::set_cursor(const Vector2f &cursor)
{
	_cursor = cursor * _size / Vector2f(_renderer.viewport_size());
	_has_cursor = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ManagerImpl::change_scene(const StaticString &old_scene, const StaticString &new_scene)
{
	Y_LOG_DEBUG("[Gui.Manager] Changing scene from \"" << old_scene << "\" to \"" << new_scene << "\"...");

	SceneActions::iterator i = _scene_actions.find(
		ScenePair(String(old_scene, ByReference()), String(new_scene, ByReference())));

	if (i != _scene_actions.end())
	{
		ScriptManager::instance()->root_context().execute(i->second);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ManagerPtr Manager::create(Renderer &renderer, Callbacks *callbacks, Allocator *allocator)
{
	return ManagerPtr(Y_NEW(allocator, ManagerImpl)(renderer, callbacks, allocator));
}

} // namespace Gui

} // namespace Yttrium
