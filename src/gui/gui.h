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
	class ResourceLoader;
	class ScriptContext;
	class Storage;
	class String;
	class Texture2D;
	class Translation;

	class GuiPrivate
	{
	public:
		struct FontDesc
		{
			SharedPtr<TextureFont> font;
			std::shared_ptr<Texture2D> texture;

			FontDesc() = default;

			FontDesc(SharedPtr<TextureFont>&& font, std::shared_ptr<Texture2D>&& texture)
				: font(std::move(font))
				, texture(std::move(texture))
			{
			}
		};

		GuiPrivate(ResourceLoader&, ScriptContext&, Allocator&);
		~GuiPrivate();

		GuiLayer& add_layer(const StaticString& name, bool is_transparent, bool is_root);
		Allocator& allocator() const { return _allocator; }
		void clear();
		const FontDesc* font(const StaticString& name) const;
		void render_canvas(Renderer&, const StaticString& name, const RectF&) const;
		ResourceLoader& resource_loader() const { return _resource_loader; }
		ScriptContext& script_context() const { return _script_context; }
		void set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name);
		void set_translation(const StaticString& path);
		String translate(const StaticString&) const;

	private:
		ResourceLoader& _resource_loader;
		ScriptContext& _script_context;
		Allocator& _allocator;
		StdMap<String, FontDesc> _fonts{ _allocator };
		StdMap<StaticString, UniquePtr<GuiLayer>> _layers;
		StdVector<GuiLayer*> _layer_stack{ _allocator };
		StdMap<String, std::function<void(Renderer&, const RectF&)>> _canvas_handlers{ _allocator };
		std::unique_ptr<const Translation> _translation;

		friend Gui;
	};
}

#endif
