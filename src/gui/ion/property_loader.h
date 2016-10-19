#ifndef _src_gui_ion_property_loader_h_
#define _src_gui_ion_property_loader_h_

#include "../property_loader.h"

#include <yttrium/renderer/texture.h>

namespace Yttrium
{
	class GuiPrivate;
	class IonNode;
	class IonObject;
	class ResourceLoader;

	class GuiIonPropertyLoader : public GuiPropertyLoader
	{
	public:
		GuiIonPropertyLoader(const IonObject* object, const IonObject* class_, GuiPrivate&);

		void set_default_font_name(const String*);

		void bind(const StaticString& name) override;
		bool load(const StaticString&, float&) const override;
		bool load_alignment(const StaticString& name, unsigned* alignment) const override;
		bool load_color(const StaticString& name, Vector4* color) const override;
		bool load_font(const StaticString& name, SharedPtr<TextureFont>* font, std::shared_ptr<Texture2D>* texture) const override;
		bool load_margins(const StaticString& name, Margins* margins) const override;
		bool load_rect(const StaticString& name, RectF& rect, bool update) const override;
		std::shared_ptr<const Sound> load_sound(const StaticString& name) const override;
		bool load_state(const StaticString& name, WidgetState* state) const override;
		bool load_text(const StaticString& name, String* text) const override;
		bool load_texture(const StaticString& name, std::shared_ptr<Texture2D>* texture) const override;
		bool load_translatable(const StaticString& name, String* text) const override;
		void unbind() override;

		static bool load(float&, const IonNode&);
		static bool load_alignment(unsigned* alignment, const IonNode& node);
		static bool load_margins(Margins* margins, const IonNode& node);
		static bool load_size(SizeF& size, const IonNode& node);
		static bool load_state(WidgetState* state, const IonNode& node);
		static bool load_text(const StaticString** text, const IonNode& node);
		static bool load_texture(std::shared_ptr<Texture2D>* texture, const IonNode& node, ResourceLoader&, Texture2D::Filter default_filter);

	private:
		const IonObject* _object;
		const IonObject* _class;
		const GuiPrivate& _gui;
		const IonObject* _bound_object;
		const IonObject* _bound_class;
		const String* _default_font_name = nullptr;
	};
}

#endif
