#include "logging.h"
#include "gui.h"

#include <yttrium/allocator.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/script/context.h>

#include "../memory/allocatable.h"
#include "ion/dumper.h"
#include "ion/loader.h"
#include "scene.h"

namespace Yttrium
{

GuiImpl::GuiImpl(const Renderer& renderer, Callbacks& callbacks, Allocator* allocator)
	: Gui(allocator)
	, _proxy_allocator("gui", allocator)
	, _renderer(renderer)
	, _texture_cache(TextureCache::create(_renderer))
	, _callbacks(callbacks)
	, _has_size(false)
	, _size(0)
	, _scaling(Scaling::Stretch)
	, _has_cursor(false)
{
}

GuiImpl::~GuiImpl()
{
	clear();
}

bool GuiImpl::add_scene(GuiScene* scene, bool is_root)
{
	const String& scene_name = scene->name();

	if (_scenes.find(scene_name) != _scenes.end())
	{
		Y_LOG("[Gui] Scene \"" << scene_name << "\" has already been added");
		return false;
	}

	_scenes.emplace(scene_name, scene);

	if (is_root)
	{
		if (!_scene_stack.empty())
		{
			Y_LOG("[Gui] Scene \"" << scene_name << "\" \"root\" option ignored");
			// TODO: Print the existing root scene name.
		}
		else
		{
			_scene_stack.push_back(scene);
		}
	}

	return true;
}

GuiScene* GuiImpl::create_scene(const StaticString& name)
{
	if (!_has_size)
	{
		Y_LOG("[Gui] Can't load scene \"" << name << "\": GUI size has not been set");
		return nullptr;
	}

	Allocatable<GuiScene> scene(&_proxy_allocator);
	scene.reset(*this, name);
	scene->set_size(_size);
	return scene.release();
}

const GuiImpl::FontDesc* GuiImpl::font(const StaticString& name) const
{
	auto i = _fonts.find(String(name, ByReference()));
	return i != _fonts.end() ? &i->second : nullptr;
}

void GuiImpl::set_font(const StaticString& name,
	const StaticString& font_source, const StaticString& texture_name)
{
	Texture2DPtr texture = _texture_cache->load_texture_2d(texture_name, true);
	if (texture.is_null())
		return;

	// NOTE: If one of the manager's fonts is set in the renderer, we have no ways of removing it from there
	// when the manager is being cleant up, so we must use the renderer's allocator here.

	TextureFont font(font_source, _renderer.allocator());

	if (font)
	{
		texture->set_filter(Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter);
		_fonts[String(name, &_proxy_allocator)] = FontDesc(font, texture);
	}
}

void GuiImpl::set_scene_change_action(const String& from_scene, const String& to_scene, const String& action)
{
	_scene_actions.emplace(ScenePair(from_scene, to_scene), action);
}

void GuiImpl::clear()
{
	for (const auto& scene: _scenes)
		delete_scene(scene.second);

	_has_size = false;
	_size = Vector2f(0);
	_fonts.clear();
	_scenes.clear();
	_scene_stack.clear();
	_scene_actions.clear();
	_has_cursor = false;
}

void GuiImpl::dump(const StaticString& filename) const
{
	GuiIonDumper(*this).dump(filename);
}

bool GuiImpl::has_scene(const StaticString& name) const
{
	return _scenes.find(String(name, ByReference())) != _scenes.end();
}

bool GuiImpl::load(const StaticString& filename)
{
	clear();

	GuiIonLoader loader(this);
	if (!loader.load(filename))
		return false;

	if (_scene_stack.empty())
	{
		Y_LOG("[Gui] No root scene has been added");
		clear();
		return false;
	}

	return true;
}

bool GuiImpl::pop_scenes(size_t count)
{
	if (_scene_stack.empty())
		return false;

	const GuiScene* old_scene = _scene_stack.back();

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

bool GuiImpl::push_scene(const StaticString& name)
{
	auto i = _scenes.find(String(name, ByReference()));
	if (i == _scenes.end())
		return false;

	const GuiScene* old_scene = _scene_stack.back();
	_scene_stack.push_back(i->second);
	change_scene(old_scene->name(), name);
	return true;
}

bool GuiImpl::process_key(const KeyEvent& event)
{
	return _scene_stack.empty()
		? false
		: _scene_stack.back()->process_key(event);
}

bool GuiImpl::render()
{
	if (!_renderer || !_has_size || _scene_stack.empty())
		return false;

	auto i = _scene_stack.begin() + (_scene_stack.size() - 1);

	if (_has_cursor)
	{
		(*i)->set_cursor(_cursor);
		_has_cursor = false;
	}

	while (i != _scene_stack.begin() && (*i)->is_transparent())
		--i;

	_renderer.set_matrix_2d(_size.x, _size.y);

	for (; i != _scene_stack.end(); ++i)
		(*i)->render(_renderer, _size);

	return true;
}

void GuiImpl::set_cursor(const Vector2f& cursor)
{
	_cursor = cursor * _size / Vector2f(_renderer.viewport_size());
	_has_cursor = true;
}

void GuiImpl::change_scene(const StaticString& old_scene, const StaticString& new_scene)
{
	Y_LOG_DEBUG("[Gui] Changing scene from \"" << old_scene << "\" to \"" << new_scene << "\"...");

	auto i = _scene_actions.find(ScenePair(String(old_scene, ByReference()), String(new_scene, ByReference())));
	if (i != _scene_actions.end())
		ScriptContext::global().execute(i->second);
}

void GuiImpl::delete_scene(GuiScene* scene)
{
	Y_DELETE(&_proxy_allocator, scene);
}

GuiPtr Gui::create(Renderer& renderer, Callbacks& callbacks, Allocator* allocator)
{
	return GuiPtr(Y_NEW(allocator, GuiImpl)(renderer, callbacks, allocator));
}

} // namespace Yttrium
