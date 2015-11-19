#ifndef _src_gui_gui_h_
#define _src_gui_gui_h_

#include <yttrium/gui.h>

#include <yttrium/memory/proxy_allocator.h>
#include <yttrium/object.h>
#include <yttrium/pointer.h>
#include <yttrium/script/code.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include <yttrium/texture_font.h>
#include "types.h"

namespace Yttrium
{
	class GuiIonDumper;
	class GuiScene;
	class KeyEvent;
	class RendererImpl;
	class Texture2D;
	class TextureCache;
	class WindowCallbacks;

	class GuiImpl : public Gui
	{
		friend GuiIonDumper;

	public:

		struct FontDesc
		{
			TextureFont font;
			SharedPtr<Texture2D> texture;

			FontDesc() = default;

			FontDesc(TextureFont&& font, const SharedPtr<Texture2D>& texture)
				: font(std::move(font))
				, texture(texture)
			{
			}
		};

	public:

		GuiImpl(ScriptContext&, RendererImpl&, WindowCallbacks&, Allocator& allocator);
		~GuiImpl() override;

	public:

		bool add_scene(Pointer<GuiScene>&& scene, bool is_root);

		Allocator& allocator() const { return const_cast<ProxyAllocator&>(_proxy_allocator); }

		WindowCallbacks& callbacks() const { return _callbacks; }

		Pointer<GuiScene> create_scene(const StaticString& name, bool is_transparent);

		const FontDesc* font(const StaticString& name) const;

		bool process_key_event(const KeyEvent& event);

		void render(const Point& cursor);

		ScriptContext& script_context() const { return _script_context; }

		void set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name);

		void set_scaling(Scaling scaling)
		{
			_scaling = scaling;
		}

		void set_scene_change_action(const String& from_scene, const String& to_scene, const String& action);

		void set_size(const SizeF& size)
		{
			_size = size;
			_has_size = true;
		}

		TextureCache& texture_cache() { return *_texture_cache; }

	public: // Gui

		void clear() override;
		void dump(const StaticString& filename) const override;
		bool has_scene(const StaticString& name) const override;
		bool load(const StaticString& filename) override;
		bool pop_scenes(size_t count) override;
		bool push_scene(const StaticString& name) override;

	private:

		void change_scene(const StaticString& old_scene, const StaticString& new_scene);

	private:

		ScriptContext&                    _script_context;
		RendererImpl&                     _renderer;
		WindowCallbacks&                  _callbacks;
		ProxyAllocator                    _proxy_allocator;
		Pointer<TextureCache>             _texture_cache;
		bool                              _has_size = false;
		SizeF                             _size;
		Scaling                           _scaling = Scaling::Stretch;
		StdMap<String, FontDesc>          _fonts;
		StdMap<StaticString, Pointer<GuiScene>> _scenes;
		StdVector<GuiScene*>              _scene_stack;
		StdMap<std::pair<String, String>, std::pair<String, ScriptCode>> _scene_actions;
	};
}

#endif
