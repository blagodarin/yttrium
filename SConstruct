#############################################################################
#
# Yttrium SConstruct script
#
#############################################################################

#############################################################################
# Master environment
#############################################################################

import ConfigParser
import os
import os.path

EnsureSConsVersion(1, 0)

# Initialize the environment.

env = Environment(
	CPPFLAGS = [],
	CPPPATH = ['#/include'],
	ENV = { # For GCC and colorgcc.
		'HOME': os.environ['HOME'],
		'PATH': os.environ['PATH'],
		'TERM': os.environ['TERM']},
	LIBS = [],
	LINKFLAGS = [],
	TOOLS = [])

env.Decider('MD5-timestamp') # Make the builds a bit faster.

platform = env['PLATFORM']

if platform == 'posix':
	env.Tool('default') # GCC or GCC-compatible toolkit expected.
elif platform == 'win32':
	env.Tool('mingw')

# Create the initial configuration from the command line options.

AddOption('--build-mode',
	dest = 'build-mode', action = 'store', type = 'choice',
	choices = ['debug', 'development', 'release'],
	help = 'If specified, will perform the optimized release build.')

AddOption('--build-platform',
	dest = 'build-platform', action = 'store', type = 'choice',
	choices = ['posix-x11', 'win32'],
	help = 'If specified, will override the default target platform.')

build_mode = GetOption('build-mode')
build_platform = GetOption('build-platform')

# Update the configuration with the 'build.ini' settings.

build_ini = ConfigParser.RawConfigParser()
build_ini.readfp(open('build.default.ini'))
build_ini.read(['build.ini'])

if build_mode is None:
	build_mode = build_ini.get('build', 'mode')

if build_platform is None:
	build_platform = build_ini.get('build', 'platform')
	if build_platform == 'default':
		if platform == 'posix':
			build_platform = 'posix-x11'
		else:
			build_platform = platform

# Configure the environment.

ports = []

if build_platform == 'posix-x11':
	ports = ['posix', 'x11']
elif build_platform == 'win32':
	ports = ['win32']

if build_mode == 'debug':
	env.Append(CPPDEFINES = ['__Y_DEBUG'])

env.Append(CPPFLAGS = ['-Wall']) # We use GCC/MinGW.
if build_mode == 'release':
	env.Append(CPPFLAGS = ['-O3']) # We use GCC/MinGW.
else:
	env.Append(CPPFLAGS = ['-g']) # We use GCC/MinGW.

env.Prepend(LIBS = ['png', 'z', 'vorbisfile', 'vorbis', 'ogg', 'png', 'z'])
if 'posix' in ports:
	env.Append(LIBS = ['pthread', 'openal'])
if 'win32' in ports:
	env.Append(LIBS = ['OpenAL32', 'OpenGL32', 'ws2_32', 'winmm', 'gdi32'])
if 'x11' in ports:
	env.Append(LIBS = ['GL', 'Xrandr', 'X11'])

#############################################################################
# Slave environments
#############################################################################

slave_env = env.Clone(
	LIBPATH = ['#/lib'],
	LIBS = ['yttrium'])

if build_platform == 'win32':
	slave_env.Append(LINKFLAGS = ['-Wl,-subsystem,windows']) # We use GCC/MinGW.

#############################################################################
# Targets
#############################################################################

# Library.

yttrium = SConscript(dirs = 'src', exports = 'env ports')
Alias('yttrium', yttrium)
Default('yttrium')

# Tests.

tests = SConscript(dirs = 'tests', exports = {'env': slave_env})
Alias('tests', tests)
for test in tests:
	env.Alias('test', test, '@' + str(test[0]) + ' --log_level=all')
AlwaysBuild('test')

# Tools.

tools = SConscript(dirs = 'tools', exports = {'env': slave_env})
Alias('tools', tools)
Default('tools')

# Examples.

examples = SConscript(dirs = 'examples', exports = {'env': slave_env})
Alias('examples', examples)

# All targets.

Alias('all', [yttrium, tests, tools, examples])
