#ifndef _src_gui_ion_property_loader_h_
#define _src_gui_ion_property_loader_h_

#include "../property_loader.h"

#include <yttrium/renderer/texture.h>

#include <string>

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

		void set_default_font_name(const std::string*);

		void bind(const StaticString& name) override;
		bool load(const StaticString&, float&) const override;
		GuiActions load_actions(const StaticString& name) const override;
		bool load_alignment(const StaticString& name, unsigned* alignment) const override;
		bool load_color(const StaticString& name, Vector4* color) const override;
		void load_font(const StaticString& name, ResourcePtr<const TextureFont>*, ResourcePtr<const Texture2D>*) const override;
		bool load_margins(const StaticString& name, Margins&) const override;
		bool load_rect(const StaticString& name, RectF&, bool update) const override;
		ResourcePtr<const Sound> load_sound(const StaticString& name) const override;
		bool load_state(const StaticString& name, WidgetState*) const override;
		bool load_text(const StaticString& name, String&) const override;
		bool load_texture(const StaticString& name, ResourcePtr<const Texture2D>&, Texture2D::Filter&) const override;
		bool load_translatable(const StaticString& name, String&) const override;
		void unbind() override;

		static bool load(float&, const IonNode&);
		static GuiActions load_actions(const IonObject&);
		static GuiActions load_actions(const IonNode&);
		static bool load_alignment(unsigned* alignment, const IonNode&);
		static void load_margins(Margins&, const IonNode&);
		static void load_size(SizeF&, const IonNode&);
		static bool load_state(WidgetState*, const IonNode&);
		static bool load_text(const StaticString**, const IonNode&);
		static bool load_texture(ResourcePtr<const Texture2D>&, Texture2D::Filter&, const IonNode&, ResourceLoader&);

	private:
		const IonObject* _object;
		const IonObject* _class;
		const GuiPrivate& _gui;
		const IonObject* _bound_object;
		const IonObject* _bound_class;
		const std::string* _default_font_name = nullptr;
	};
}

#endif
