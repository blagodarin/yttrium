#ifndef _src_gui_ion_property_loader_h_
#define _src_gui_ion_property_loader_h_

#include "../property_loader.h"

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

		void bind(std::string_view name) override;
		bool load(std::string_view, float&) const override;
		GuiActions load_actions(std::string_view name) const override;
		bool load_alignment(std::string_view name, unsigned* alignment) const override;
		bool load_color(std::string_view name, Color4f*) const override;
		void load_font(std::string_view name, std::shared_ptr<const TextureFont>*, std::shared_ptr<const Texture2D>*) const override;
		bool load_margins(std::string_view name, Margins&) const override;
		bool load_rect(std::string_view name, RectF&, bool update) const override;
		std::shared_ptr<const Sound> load_sound(std::string_view name) const override;
		bool load_state(std::string_view name, WidgetState*) const override;
		bool load_text(std::string_view name, std::string&) const override;
		bool load_texture(std::string_view name, std::shared_ptr<const Texture2D>&, Texture2D::Filter&) const override;
		bool load_translatable(std::string_view name, std::string&) const override;
		void unbind() override;

		static bool load(float&, const IonNode&);
		static GuiActions load_actions(const IonObject&);
		static GuiActions load_actions(const IonNode&);
		static bool load_alignment(unsigned* alignment, const IonNode&);
		static void load_margins(Margins&, const IonNode&);
		static void load_size(SizeF&, const IonNode&);
		static bool load_state(WidgetState*, const IonNode&);
		static bool load_text(std::string_view&, const IonNode&);
		static bool load_texture(std::shared_ptr<const Texture2D>&, Texture2D::Filter&, const IonNode&, ResourceLoader&);

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
