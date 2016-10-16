#ifndef _src_gui_gui_h_
#define _src_gui_gui_h_

#include <yttrium/memory/shared_ptr.h>
#include <yttrium/renderer/texture_font.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>

#include <functional>
#include <memory>

namespace Yttrium
{
	class Gui;
	class GuiLayer;
	class Renderer;
	class ScriptContext;
	class Storage;
	class String;
	class Texture2D;
	class TextureCache;
	class Translation;

	class GuiPrivate
	{
	public:
		struct FontDesc
		{
			SharedPtr<TextureFont> font;
			SharedPtr<Texture2D> texture;

			FontDesc() = default;

			FontDesc(UniquePtr<TextureFont>&& font, SharedPtr<Texture2D>&& texture)
				: font(std::move(font))
				, texture(std::move(texture))
			{
			}
		};

		GuiPrivate(const Storage&, Renderer&, ScriptContext&, Allocator&);
		~GuiPrivate();

		GuiLayer& add_layer(const StaticString& name, bool is_transparent, bool is_root);
		Allocator& allocator() const { return _allocator; }
		void clear();
		const FontDesc* font(const StaticString& name) const;
		void render_canvas(const StaticString& name, const RectF&) const;
		const Storage& storage() const { return _storage; }
		ScriptContext& script_context() const { return _script_context; }
		void set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name);
		void set_translation(const StaticString& path);
		TextureCache& texture_cache() { return *_texture_cache; }
		String translate(const StaticString&) const;

	private:
		const Storage& _storage;
		Renderer& _renderer;
		ScriptContext& _script_context;
		Allocator& _allocator;
		const UniquePtr<TextureCache> _texture_cache;
		StdMap<String, FontDesc> _fonts{ _allocator };
		StdMap<StaticString, UniquePtr<GuiLayer>> _layers;
		StdVector<GuiLayer*> _layer_stack{ _allocator };
		StdMap<String, std::function<void(Renderer&, const RectF&)>> _canvas_handlers{ _allocator };
		std::unique_ptr<const Translation> _translation;

		friend Gui;
	};
}

#endif
