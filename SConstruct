#############################################################################
#
# Yttrium SConstruct script
#
#############################################################################

#############################################################################
# Environment setup
#############################################################################

import ConfigParser
import os
import os.path

from genconfig import GenConfig

EnsureSConsVersion(1, 0)

# Options

AddOption('--for',
	dest = 'for', action = 'store', type = 'choice',
	choices = ['posix', 'win32'],
	help = 'If specified, will override the default target platform.')

AddOption('--release',
	dest = 'release', action = 'store_true', default = False,
	help = 'If specified, will perform the optimized release build.')

platform_override = GetOption('for')
release = GetOption('release')

# Environment setup

env = Environment(
	CPPFLAGS = ['-Wall'], # We use GCC/MinGW.
	ENV = { # For GCC and colorgcc.
		'HOME': os.environ['HOME'],
		'PATH': os.environ['PATH'],
		'TERM': os.environ['TERM']},
	LIBS = [],
	TOOLS = [])

env.Decider('MD5-timestamp') # Make builds a bit faster.

platform = env['PLATFORM']

if platform == 'posix':
	env.Tool('default') # GCC or GCC-compatible toolkit expected.
elif platform == 'win32':
	env.Tool('mingw')

build_ini = ConfigParser.RawConfigParser()
build_ini.read(['build.ini'])

ports = []
build_platform = build_ini.get('build', 'platform')
if build_platform == 'default':
	if platform == 'posix':
		ports = ['posix', 'x11']
	elif platform == 'win32':
		ports = ['win32']
elif build_platform == 'posix-x11':
	ports = ['posix', 'x11']
elif build_platform == 'win32':
	ports = ['win32']

env.Prepend(LIBS = ['png', 'z', 'vorbisfile', 'vorbis', 'ogg', 'png', 'z'])
if 'posix' in ports:
	env.Append(LIBS = ['pthread', 'openal'])
if 'win32' in ports:
	env.Append(LIBS = ['OpenAL32', 'OpenGL32', 'ws2_32', 'winmm', 'gdi32'])
if 'x11' in ports:
	env.Append(LIBS = ['GL', 'Xrandr', 'X11'])

if release:
	env.Append(CPPFLAGS = '-O3') # We use GCC/MinGW.
else:
	env.Append(CPPFLAGS = '-g') # We use GCC/MinGW.

env.Append(
	CPPPATH = ['#/include'])

env.Append(
	CPPDEFINES = ['__Y_DEBUG'])

linkflags_gui = ' -Wl,-subsystem,windows' # We use GCC/MinGW.

# Compiler-specific configurations

def CheckAttributePacked(context):
	context.Message('Checking for __attribute__((packed))... ')
	result = context.TryCompile("""
struct Foo
{
	int bar;
}
__attribute__((packed));
""", '.cpp')
	context.Result(result)
	return result

if not GetOption('clean'):
	sconf = env.Configure(
		custom_tests = {'CheckAttributePacked': CheckAttributePacked},
		clean = True, help = False) # NOTE: Should be 'clean = False'.
	config = GenConfig('__CONFIG_HPP')

	config.define_if('__Y_POSIX', 'posix' in ports)
	config.define_if('__Y_WIN32', 'win32' in ports)
	config.define_if('__Y_X11', 'x11' in ports)

	config.new_line()

	config.define_select('Y_PACKED', sconf.CheckAttributePacked(), '__attribute__((packed))', None)

	config.save('src/config.hpp')
	env = sconf.Finish()

#############################################################################
# Targets
#############################################################################

# Library

yttrium = SConscript(dirs = 'src', exports = 'env ports')
Alias('yttrium', yttrium)
Default('yttrium')

client_env = env.Clone(
	LIBPATH = ['#/lib'],
	LIBS = ['yttrium'])

# Tests

tests = SConscript(dirs = 'tests', exports = {'env': client_env})
Alias('tests', tests)
for test in tests:
	env.Alias('test', test, '@' + str(test[0]) + ' --log_level=all')
AlwaysBuild('test')

# Tools

tools = SConscript(dirs = 'tools', exports = {'env': client_env})
Alias('tools', tools)
Default('tools')

# Examples

examples = SConscript(dirs = 'examples', exports = {'env': client_env})
Alias('examples', examples)

# All targets

Alias('all', [yttrium, tests, tools, examples])
