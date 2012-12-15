################################################################################
#
# Yttrium SConstruct script
#
################################################################################

################################################################################
# Environments.
################################################################################

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
	platform = 'posix-x11'
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
		build_platform = platform

# Configure the environment.

ports = []

if build_platform == 'posix-x11':
	ports = ['posix', 'x11']
elif build_platform == 'win32':
	ports = ['windows']

if build_mode == 'debug':
	env.Append(CPPDEFINES = ['_DEBUG'])
else:
	env.Append(CPPDEFINES = ['NDEBUG'])

env.Append(CPPFLAGS = ['-std=gnu++0x', '-Wall', '-Wextra']) # We use GCC/MinGW.
if build_mode == 'release':
	env.Append(CPPFLAGS = ['-O3']) # We use GCC/MinGW.
else:
	env.Append(CPPFLAGS = ['-g']) # We use GCC/MinGW.

env.Prepend(LIBS = ['jpeg', 'ogg', 'png', 'vorbis', 'vorbisfile', 'z'])
if 'posix' in ports:
	env.Append(LIBS = ['openal', 'pthread'])
if 'windows' in ports:
	if build_platform == 'win32':
		env.Append(LIBS = ['gdi32', 'OpenAL32', 'OpenGL32', 'winmm', 'ws2_32'])
if 'x11' in ports:
	env.Append(LIBS = ['GL', 'X11', 'Xrandr'])

# Slave environment.

slave_env = env.Clone(
	LIBPATH = ['#/lib'],
	LIBS = ['yttrium'])

if build_platform == 'windows':
	slave_env.Append(LINKFLAGS = ['-Wl,-subsystem,windows']) # We use GCC/MinGW.

# Special configuration for the master environment.

env.Append(
	CPPDEFINES = [
		'__YTTRIUM'],
	CPPFLAGS = [
		'-fvisibility=hidden',
		'-fvisibility-inlines-hidden'])

################################################################################
# Targets.
################################################################################

# Library.

yttrium = SConscript(dirs = 'src', exports = 'env ports')
Alias('yttrium', yttrium)
Default('yttrium')

# Tests.

tests = SConscript(dirs = 'tests', exports = {'env': slave_env})
Alias('tests', tests)
for test in tests:
	env.Alias('test', test, '@LD_LIBRARY_PATH=lib/ ' + str(test[0]) + ' --log_level=message') # Won't go on Windows.
AlwaysBuild('test')

# Tools.

tools = SConscript(dirs = 'tools', exports = {'env': slave_env})
Alias('tools', tools)

# Examples.

examples = SConscript(dirs = 'examples', exports = {'env': slave_env})
Alias('examples', examples)

# All targets.

Alias('all', [yttrium, tests, tools, examples])
