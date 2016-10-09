#ifndef _src_gui_gui_h_
#define _src_gui_gui_h_

#include <yttrium/memory/shared_ptr.h>
#include <yttrium/renderer/texture_font.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>

#include <functional>

namespace Yttrium
{
	class Gui;
	class GuiLayer;
	class Renderer;
	class ResourceManager;
	class ScriptContext;
	class String;
	class Texture2D;
	class TextureCache;

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

		GuiPrivate(const ResourceManager&, Renderer&, ScriptContext&, Allocator&);
		~GuiPrivate();

		GuiLayer& add_layer(const StaticString& name, bool is_transparent, bool is_root);
		Allocator& allocator() const { return _allocator; }
		void clear();
		const FontDesc* font(const StaticString& name) const;
		void render_canvas(const StaticString& name, const RectF&) const;
		ScriptContext& script_context() const { return _script_context; }
		void set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name);
		TextureCache& texture_cache() { return *_texture_cache; }

	private:
		const ResourceManager& _resource_manager;
		Renderer& _renderer;
		ScriptContext& _script_context;
		Allocator& _allocator;
		const UniquePtr<TextureCache> _texture_cache;
		StdMap<String, FontDesc> _fonts;
		StdMap<StaticString, UniquePtr<GuiLayer>> _layers;
		StdVector<GuiLayer*> _layer_stack;
		StdMap<String, std::function<void(Renderer&, const RectF&)>> _canvas_handlers;

		friend Gui;
	};
}

#endif
