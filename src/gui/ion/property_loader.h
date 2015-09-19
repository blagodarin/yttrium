#ifndef _src_gui_ion_property_loader_h_
#define _src_gui_ion_property_loader_h_

#include "../property_loader.h"

#include <yttrium/texture.h>

namespace Yttrium
{
	class GuiImpl;
	class IonNode;
	class IonObject;
	class TextureCache;

	class GuiIonPropertyLoader : public GuiPropertyLoader
	{
	public:

		GuiIonPropertyLoader(const IonObject* object, const IonObject* class_, GuiImpl& gui);

		void bind(const StaticString& name) override;
		bool load_alignment(const StaticString& name, unsigned* alignment) const override;
		bool load_color(const StaticString& name, Vector4* color) const override;
		bool load_font(const StaticString& name, TextureFont* font, SharedPtr<Texture2D>* texture) const override;
		bool load_margins(const StaticString& name, Margins* margins) const override;
		bool load_position(const StaticString& name, Vector2* position) const override;
		bool load_rect(const StaticString& name, Rect* rect, bool update) const override;
		bool load_scaling(const StaticString& name, Scaling* scaling) const override;
		bool load_size(const StaticString& name, Vector2* size) const override;
		SharedPtr<Sound> load_sound(const StaticString& name) const override;
		bool load_state(const StaticString& name, WidgetState* state) const override;
		bool load_text(const StaticString& name, String* text) const override;
		bool load_texture(const StaticString& name, SharedPtr<Texture2D>* texture) const override;
		bool load_translatable(const StaticString& name, String* text) const override;
		void unbind() override;

		static bool load_alignment(unsigned* alignment, const IonNode& node);
		static bool load_margins(Margins* margins, const IonNode& node);
		static bool load_scaling(Scaling* scaling, const IonNode& node);
		static bool load_size(Vector2* size, const IonNode& node);
		static SharedPtr<Sound> load_sound(const IonNode& node);
		static bool load_state(WidgetState* state, const IonNode& node);
		static bool load_text(const StaticString** text, const IonObject& object, const StaticString& name);
		static bool load_texture(SharedPtr<Texture2D>* texture, const IonNode& node, TextureCache& texture_cache, Texture2D::Filter default_filter);

	private:

		const IonObject* _object;
		const IonObject* _class;
		const GuiImpl&   _gui;
		TextureCache&    _texture_cache;
		const IonObject* _bound_object;
		const IonObject* _bound_class;
	};
}

#endif
