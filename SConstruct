import os

EnsureSConsVersion(1, 0)

################################################################################
# Environment
################################################################################

#===============================================================================
# Command line options
#===============================================================================

AddOption('--no-jpeg', dest = 'no_jpeg',
	action = 'store_true', default = False,
	help = 'Exclude JPEG support')

AddOption('--no-ogg-vorbis', dest = 'no_ogg_vorbis',
	action = 'store_true', default = False,
	help = 'Exclude Ogg Vorbis support')

AddOption('--no-png', dest = 'no_png',
	action = 'store_true', default = False,
	help = 'Exclude PNG support')

AddOption('--platform', dest = 'platform',
	action = 'store', type = 'choice', choices = ['posix-x11', 'windows'],
	help = 'If specified, will override the default target platform.')

AddOption('--release', dest = 'release',
	action = 'store_true', default = False,
	help = 'If specified, will perform the optimized release build.')

AddOption('--static', dest = 'static',
	action = 'store_true', default = False,
	help = 'If specified, will build a static library.')

option_no_jpeg = GetOption('no_jpeg')
option_no_ogg_vorbis = GetOption('no_ogg_vorbis')
option_no_png = GetOption('no_png')
option_platform = GetOption('platform')
option_release = GetOption('release')
option_static = GetOption('static')

#===============================================================================
# Environment initialization
#===============================================================================

env = Environment(TOOLS = [])

host_platform = env['PLATFORM']
tool_type = None

if host_platform == 'posix':
	tool_type = 'gcc'
	env.Tool('default') # GCC or GCC-compatible toolchain expected.
	env.Append(ENV = {'HOME': os.environ['HOME'], 'TERM': os.environ['TERM']}) # Required by GCC and colorgcc.
elif host_platform == 'win32':
	tool_type = 'gcc'
	env.Tool('mingw')

#===============================================================================
# Environment configuration
#===============================================================================

CXXCOMSTR  = 'C++   $SOURCE'
LINKCOMSTR = 'LINK  $TARGET'

env['CXXCOMSTR'] = CXXCOMSTR
env['LINKCOMSTR'] = LINKCOMSTR
env['SHCXXCOMSTR'] = CXXCOMSTR
env['SHLINKCOMSTR'] = LINKCOMSTR

#-------------------------------------------------------------------------------
# Common configuration
#-------------------------------------------------------------------------------

env.Append(
	BUILD = 'tmp',
	CPPPATH = ['include'])

if option_release:
	env.Append(CPPDEFINES = ['NDEBUG'])
else:
	env.Append(CPPDEFINES = ['_DEBUG'])

env.Decider('MD5-timestamp')

env.VariantDir('$BUILD', '.', duplicate = 0)

#-------------------------------------------------------------------------------
# Tool-specific configuration
#-------------------------------------------------------------------------------

toolchains_flags = {
	'gcc': {
		'compile': ['-std=gnu++1y', '-Wall', '-Wextra'],
		'compile-debug': ['-g'],
		'compile-release': ['-O3'],
		'compile-src': ['-fvisibility=hidden', '-fvisibility-inlines-hidden'],
		'link-windows': ['-Wl,-subsystem,windows']}}

toolchain_flags = toolchains_flags[tool_type]

env.Append(CPPFLAGS = toolchain_flags['compile'])
if option_release:
	env.Append(CPPFLAGS = toolchain_flags['compile-release'])
else:
	env.Append(CPPFLAGS = toolchain_flags['compile-debug'])

################################################################################
# Targets
################################################################################

target_platform = option_platform
if target_platform is None:
	if host_platform == 'posix':
		target_platform = 'posix-x11'
	elif host_platform == 'win32':
		target_platform = 'windows'

ports = []
if target_platform == 'posix-x11':
	ports = ['posix', 'x11']
elif target_platform == 'windows':
	ports = ['windows']

LIBS = []

if not option_no_jpeg:
	LIBS += ['jpeg']
if not option_no_ogg_vorbis:
	LIBS += ['ogg', 'vorbis', 'vorbisfile']
if not option_no_png:
	LIBS += ['png', 'z']

if 'posix' in ports:
	LIBS += ['openal', 'pthread']
if 'windows' in ports:
	LIBS += ['gdi32', 'OpenAL32', 'OpenGL32', 'winmm', 'ws2_32']
if 'x11' in ports:
	LIBS += ['GL', 'X11', 'Xrandr']

#===============================================================================
# Library
#===============================================================================

src_env = env.Clone()
src_env.Append(
	CPPFLAGS = toolchain_flags['compile-src'])

if option_static:
	src_env.Append(CPPDEFINES = ['__YTTRIUM_STATIC'])
else:
	src_env.Append(CPPDEFINES = ['__YTTRIUM_SHARED'])
	src_env.Append(LIBS = LIBS)

if option_no_jpeg:
	src_env.Append(CPPDEFINES = ['Y_NO_JPEG'])
if option_no_ogg_vorbis:
	src_env.Append(CPPDEFINES = ['Y_NO_OGG_VORBIS'])
if option_no_png:
	src_env.Append(CPPDEFINES = ['Y_NO_PNG'])

src_paths = [
	'.',
	'audio',
	'audio/backend',
	'audio/backend/openal',
	'base',
	'gui',
	'gui/ion',
	'gui/logic',
	'gui/widgets',
	'io',
	'io/formats',
	'ion',
	'math',
	'memory',
	'renderer',
	'renderer/gl',
	'resources',
	'script',
	'window',
	'window/bindings']

if not option_no_ogg_vorbis:
	src_paths += ['audio/io/ogg_vorbis']

if 'posix' in ports:
	src_paths += ['base/posix', 'io/posix', 'memory/posix']

if 'windows' in ports:
	src_paths += ['base/windows', 'io/windows', 'memory/windows', 'window/windows']

if 'x11' in ports:
	src_paths += ['window/x11']

src_target = 'lib/yttrium'
src_sources = [src_env.Glob('$BUILD/src/' + path + '/*.cpp') for path in src_paths]

src_sources += [
	'$BUILD/src/audio/formats/wav.cpp',
	'$BUILD/src/image/image.cpp',
	'$BUILD/src/image/formats.cpp',
	'$BUILD/src/image/formats/dds.cpp',
	'$BUILD/src/image/formats/tga.cpp']
if not option_no_jpeg:
	src_sources += ['$BUILD/src/image/formats/jpeg.cpp']
if not option_no_ogg_vorbis:
	src_sources += ['$BUILD/src/audio/formats/ogg_vorbis.cpp']
if not option_no_png:
	src_sources += ['$BUILD/src/image/formats/png.cpp']

if option_static:
	yttrium = src_env.StaticLibrary(src_target, src_sources)
else:
	yttrium = src_env.SharedLibrary(src_target, src_sources)

Depends(yttrium, Value(option_no_jpeg))
Depends(yttrium, Value(option_no_ogg_vorbis))
Depends(yttrium, Value(option_no_png))

Alias('yttrium', yttrium)
Clean('yttrium', Dir('$BUILD/src'))
Default('yttrium')

#===============================================================================
# Library clients
#===============================================================================

env.Append(
	LIBPATH = ['lib'],
	LIBS = ['yttrium'])

if 'windows' in ports:
	env.Append(LINKFLAGS = toolchain_flags['link-windows'])

if option_static:
	env.Append(CPPDEFINES = ['__YTTRIUM_STATIC'])
	env.Append(LIBS = LIBS)

#-------------------------------------------------------------------------------
# Tests
#-------------------------------------------------------------------------------

tests_env = env.Clone()
tests_env.Append(
	CPPDEFINES = ['BOOST_TEST_DYN_LINK', '__YTTRIUM_TEST'],
	CPPPATH = '#',
	LIBS = 'boost_unit_test_framework')

tests = tests_env.Program('bin/tests', env.Glob('$BUILD/tests/*.cpp'))
Alias('tests', tests)
Clean('tests', Dir('$BUILD/tests'))

test_env = Environment(TOOLS = [])
if host_platform == 'posix':
	test_env.Append(ENV = {'LD_LIBRARY_PATH': 'lib'})
if 'x11' in ports:
	test_env.Append(ENV = {'DISPLAY': os.environ['DISPLAY']}) # Required by XOpenDisplay at runtime.
	test_env.Append(ENV = {'HOME': os.environ['HOME']}) # Required for X authentication to succeed.
test_env.Alias('test', tests, str(tests[0]) + ' --log_level=message')
AlwaysBuild('test')

#-------------------------------------------------------------------------------
# Benchmarks
#-------------------------------------------------------------------------------

def YttriumBenchmark(env, name):
	build_path = '$BUILD/benchmarks/' + name
	target = env.Program('bin/benchmark-' + name, env.Glob(build_path + '/*.cpp'))
	env.Clean(target, env.Dir(build_path))
	Alias('benchmarks', target)
	return target

YttriumBenchmark(env, 'buffer')

Clean('benchmarks', Dir('$BUILD/benchmarks'))

#-------------------------------------------------------------------------------
# Tools
#-------------------------------------------------------------------------------

def YttriumTool(env, name):
	build_path = '$BUILD/tools/' + name
	target = env.Program('bin/' + name, env.Glob(build_path + '/*.cpp'))
	env.Clean(target, env.Dir(build_path))
	Alias('tools', target)
	return target

YttriumTool(env, 'generate-sounds')
YttriumTool(env, 'generate-test-images')
ypq = YttriumTool(env, 'ypq')
ytr = YttriumTool(env, 'ytr')

Clean('tools', Dir('$BUILD/tools'))

#-------------------------------------------------------------------------------
# Utility functions
#-------------------------------------------------------------------------------

def YttriumTranslation(env, translation, source_dir):
	ytr_env = Environment(TOOLS = [])
	if host_platform == 'posix':
		ytr_env.Append(ENV = {'LD_LIBRARY_PATH': 'lib'})
	target = ytr_env.Command(translation, env.Glob('$BUILD/' + source_dir + '/*.ion'), str(ytr[0]) + ' $TARGET $SOURCES')
	Depends(target, [ytr, yttrium]) # Changing i18n code in dynamically-linked yttrium may leave ytr unchanged.
	Precious(target)
	NoClean(target)
	return target

def YttriumPackage(env, package, index):
	ytr_env = Environment(TOOLS = [])
	if host_platform == 'posix':
		ytr_env.Append(ENV = {'LD_LIBRARY_PATH': 'lib'})
	target = ytr_env.Command(package, index, str(ypq[0]) + ' $TARGET $SOURCE')
	Depends(target, [ypq, yttrium])
	AlwaysBuild(target)
	return target

#-------------------------------------------------------------------------------
# Examples
#-------------------------------------------------------------------------------

def YttriumExample(env, name):
	build_path = '$BUILD/examples/' + name
	target = env.Program('bin/' + name, env.Glob(build_path + '/*.cpp'))
	env.Clean(target, env.Dir(build_path))
	Alias('examples', target)
	return target

YttriumExample(env, '3d')

tetrium = YttriumExample(env, 'tetrium')
tetrium_ytr = YttriumTranslation(env, 'examples/tetrium/data/i18n/en.ion', 'examples/tetrium/data/gui')
tetrium_ypq = YttriumPackage(env, 'tetrium.ypq', 'examples/tetrium/data/index.txt')
Depends(tetrium_ypq, tetrium_ytr)
Depends(tetrium, tetrium_ypq)

Clean('examples', Dir('$BUILD/examples'))

#===============================================================================
# All targets
#===============================================================================

Alias('all', ['yttrium', 'tests', 'benchmarks', 'tools', 'examples'])
Clean('all', Dir(['bin', 'lib', '$BUILD']))
