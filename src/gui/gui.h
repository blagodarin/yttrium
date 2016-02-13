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

namespace Yttrium
{
	class GuiLayer;
	class KeyEvent;
	class RendererImpl;
	class Texture2D;
	class TextureCache;
	class WindowCallbacks;

	class GuiImpl : public Gui
	{
	public:
		GuiImpl(ScriptContext&, RendererImpl&, WindowCallbacks&, Allocator& allocator);
		~GuiImpl() override;

		void clear() override;
		bool has_layer(const StaticString& name) const override;
		bool load(const StaticString& filename) override;
		bool pop_layers(size_t count) override;
		bool push_layer(const StaticString& name) override;

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

		GuiLayer& add_layer(const StaticString& name, bool is_transparent, bool is_root);

		Allocator& allocator() const { return const_cast<ProxyAllocator&>(_proxy_allocator); }

		WindowCallbacks& callbacks() const { return _callbacks; }

		const FontDesc* font(const StaticString& name) const;

		bool process_key_event(const KeyEvent& event);

		void render(const PointF& cursor) const;

		ScriptContext& script_context() const { return _script_context; }

		void set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name);

		TextureCache& texture_cache() { return *_texture_cache; }

	private:
		ScriptContext&                    _script_context;
		RendererImpl&                     _renderer;
		WindowCallbacks&                  _callbacks;
		ProxyAllocator                    _proxy_allocator;
		Pointer<TextureCache>             _texture_cache;
		StdMap<String, FontDesc>          _fonts;
		StdMap<StaticString, Pointer<GuiLayer>> _layers;
		StdVector<GuiLayer*>              _layer_stack;
	};
}

#endif
