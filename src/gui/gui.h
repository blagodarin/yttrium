#ifndef __GUI_GUI_H
#define __GUI_GUI_H

#include <yttrium/gui.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/renderer.h>
#include <yttrium/renderer/texture_cache.h>
#include <yttrium/script/code.h>
#include <yttrium/texture_font.h>

#include "types.h"

#include <map>
#include <vector>

namespace Yttrium
{

namespace Ion
{
	class Object;
}

class GuiIonDumper;
class GuiScene;

class GuiImpl: public Gui
{
	friend GuiIonDumper;

public:

	struct FontDesc
	{
		TextureFont  font;
		Texture2DPtr texture;

		FontDesc() {}

		FontDesc(const TextureFont& font, const Texture2DPtr& texture)
			: font(font)
			, texture(texture)
		{
		}
	};

public:

	GuiImpl(const Renderer& renderer, Callbacks& callbacks, Allocator* allocator);
	~GuiImpl() noexcept override;

public:

	bool add_scene(GuiScene* scene, bool is_root);

	Callbacks& callbacks() const { return _callbacks; }

	GuiScene* create_scene(const StaticString& name);

	const FontDesc* font(const StaticString& name) const;

	Allocator* internal_allocator() const
	{
		return const_cast<ProxyAllocator*>(&_proxy_allocator);
	}

	void set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name);

	void set_scaling(Scaling scaling)
	{
		_scaling = scaling;
	}

	void set_scene_change_action(const String& from_scene, const String& to_scene, const String& action);

	void set_size(const Vector2f& size)
	{
		_size = size;
		_has_size = true;
	}

	TextureCache* texture_cache()
	{
		return _texture_cache.get();
	}

public: // Gui

	void clear() noexcept override;
	void dump(const StaticString& filename) const noexcept override;
	bool has_scene(const StaticString& name) const noexcept override;
	bool load(const StaticString& filename) noexcept override;
	bool pop_scenes(size_t count) noexcept override;
	bool push_scene(const StaticString& name) noexcept override;
	bool process_key(const KeyEvent& event) noexcept override;
	bool render() noexcept override;
	void set_cursor(const Vector2f& cursor) noexcept override;

private:

	void change_scene(const StaticString& old_scene, const StaticString& new_scene);
	void delete_scene(GuiScene* scene);

private:

	ProxyAllocator                    _proxy_allocator;
	Renderer                          _renderer;
	TextureCachePtr                   _texture_cache;
	Callbacks&                        _callbacks;
	bool                              _has_size;
	Vector2f                          _size;
	Scaling                           _scaling;
	std::map<String, FontDesc>        _fonts;
	std::map<StaticString, GuiScene*> _scenes;
	std::vector<GuiScene*>            _scene_stack;
	std::map<std::pair<String, String>, std::pair<String, ScriptCode>> _scene_actions;
	bool                              _has_cursor;
	Vector2f                          _cursor;
};

} // namespace Yttrium

#endif // __GUI_GUI_H
