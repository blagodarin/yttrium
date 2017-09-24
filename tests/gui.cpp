#include <yttrium/exceptions.h>
#include <yttrium/gui/gui.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/storage/storage.h>
#include "test_utils.h"

#include <boost/test/unit_test.hpp>

using Yttrium::Gui;
using Yttrium::GuiDataError;
using Yttrium::ResourceLoader;
using Yttrium::ScriptContext;
using Yttrium::Storage;

BOOST_AUTO_TEST_CASE(test_gui)
{
	Storage storage{Storage::UseFileSystem::Before};
	storage.attach_buffer("gui.ion", ::make_buffer("include \"included.ion\""));
	storage.attach_buffer("included.ion", ::make_buffer(R"(
		on_key "esc" { quit }
		root screen "root"
		{
			center "10" "10"
			{
				button
				{
					position "4" "4" "2" "2"
					text `Push`
					on_click { enter "empty" }
				}
			}
			left "10"
			{
				label
				{
					position "1" "1" "5" "1"
					text `Hello world!`
				}
			}
			right "10"
			{
				canvas "right"
				{
					position "1" "1" "2" "8"
				}
			}
			stretch
			{
				image
				{
					color "1" "0" "0"
				}
			}
		}
		transparent screen "empty"
		{
			on_key "esc" { return }
		}
	)"));

	ResourceLoader resource_loader{storage};
	ScriptContext script_context;
	BOOST_CHECK_NO_THROW(Gui(resource_loader, script_context, "gui.ion"));
}

BOOST_AUTO_TEST_CASE(test_gui_data_error)
{
	Storage storage{Storage::UseFileSystem::Before};
	storage.attach_buffer("gui.ion", ::make_buffer("unknown"));

	ResourceLoader resource_loader{storage};
	ScriptContext script_context;
	BOOST_CHECK_THROW(Gui(resource_loader, script_context, "gui.ion"), GuiDataError);
}
