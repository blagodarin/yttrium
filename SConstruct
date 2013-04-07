import os

EnsureSConsVersion(1, 0)

# Initialize the environment.

CXXCOMSTR  = 'C++   $SOURCE'
LINKCOMSTR = 'LINK  $TARGET'

env = Environment(
	CPPFLAGS = [],
	CPPPATH = ['#/include'],
	CXXCOMSTR = CXXCOMSTR,
	LIBS = [],
	LINKCOMSTR = LINKCOMSTR,
	LINKFLAGS = [],
	SHCXXCOMSTR = CXXCOMSTR,
	SHLINKCOMSTR = LINKCOMSTR,
	TOOLS = [])

env.Decider('MD5-timestamp') # Make the builds a bit faster.

platform = env['PLATFORM']

if platform == 'posix':
	platform = 'posix-x11'
	env.Tool('default') # GCC or GCC-compatible toolkit expected.
	env.Append(
		ENV = {'HOME': os.environ['HOME'], 'PATH': os.environ['PATH'], 'TERM': os.environ['TERM']}) # Required by GCC and colorgcc.
elif platform == 'win32':
	env.Tool('mingw')

# Create the initial configuration from the command line options.

AddOption('--platform', dest = 'platform',
	action = 'store', type = 'choice', choices = ['posix-x11', 'win32'],
	help = 'If specified, will override the default target platform.')

AddOption('--release', dest = 'release',
	action = 'store_true', default = False,
	help = 'If specified, will perform the optimized release build.')

build_release = GetOption('release')
build_platform = GetOption('platform')

if build_platform is None:
	build_platform = platform

# Configure the environment.

ports = []

if build_platform == 'posix-x11':
	ports = ['posix', 'x11']
elif build_platform == 'win32':
	ports = ['windows']

if build_release:
	env.Append(CPPDEFINES = ['NDEBUG'])
else:
	env.Append(CPPDEFINES = ['_DEBUG'])

env.Append(CPPFLAGS = ['-std=gnu++0x', '-Wall', '-Wextra']) # We use GCC/MinGW.
if build_release:
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
	env.Append(
		ENV = {'DISPLAY': os.environ.get('DISPLAY', ':0')}, # Required by XOpenDisplay in tests.
		LIBS = ['GL', 'X11', 'Xrandr'])

# Slave environment.

slave_env = env.Clone(
	LIBPATH = ['#/lib'],
	LIBS = ['yttrium'])

if build_platform == 'windows':
	slave_env.Append(LINKFLAGS = ['-Wl,-subsystem,windows']) # We use GCC/MinGW.

# Special configuration for the master environment.

env.Append(
	CPPDEFINES = [
		'__YTTRIUM_SHARED'],
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
