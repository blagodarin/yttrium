#ifndef __GUI_GUI_H
#define __GUI_GUI_H

#include <yttrium/gui.h>

#include <yttrium/pointer.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/script/code.h>
#include <yttrium/texture_font.h>
#include "types.h"

#include <map>
#include <memory>
#include <vector>

namespace Yttrium
{
	class GuiIonDumper;
	class GuiScene;
	class KeyEvent;
	class RendererImpl;
	class Texture2D;
	class TextureCache;
	class WindowCallbacks;

	class GuiImpl: public Gui
	{
		friend GuiIonDumper;

	public:

		struct FontDesc
		{
			TextureFont        font;
			Pointer<Texture2D> texture;

			FontDesc() = default;

			FontDesc(const TextureFont& font, const Pointer<Texture2D>& texture)
				: font(font)
				, texture(texture)
			{
			}
		};

	public:

		GuiImpl(RendererImpl& renderer, WindowCallbacks& callbacks, Allocator* allocator);
		~GuiImpl() override;

	public:

		bool add_scene(GuiScene* scene, bool is_root);

		WindowCallbacks& callbacks() const { return _callbacks; }

		GuiScene* create_scene(const StaticString& name);

		const FontDesc* font(const StaticString& name) const;

		Allocator* internal_allocator() const
		{
			return const_cast<ProxyAllocator*>(&_proxy_allocator);
		}

		bool process_key_event(const KeyEvent& event);

		void render(const Point& cursor);

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

		void clear() override;
		void dump(const StaticString& filename) const override;
		bool has_scene(const StaticString& name) const override;
		bool load(const StaticString& filename) override;
		bool pop_scenes(size_t count) override;
		bool push_scene(const StaticString& name) override;

	private:

		void change_scene(const StaticString& old_scene, const StaticString& new_scene);
		void delete_scene(GuiScene* scene);

	private:

		ProxyAllocator                    _proxy_allocator;
		RendererImpl&                     _renderer;
		std::unique_ptr<TextureCache>     _texture_cache;
		WindowCallbacks&                  _callbacks;
		bool                              _has_size;
		Vector2f                          _size;
		Scaling                           _scaling;
		std::map<String, FontDesc>        _fonts;
		std::map<StaticString, GuiScene*> _scenes;
		std::vector<GuiScene*>            _scene_stack;
		std::map<std::pair<String, String>, std::pair<String, ScriptCode>> _scene_actions;
	};
}

#endif // __GUI_GUI_H
