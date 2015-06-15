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

#===============================================================================
# Utility functions
#===============================================================================

def BuildSources(env, subdir, entries):
	targets = []
	for entry in entries:
		build_path = '$BUILD/' + subdir + '/' + entry
		target = env.Program('bin/' + entry, env.Glob(build_path + '/*.cpp'))
		env.Clean(target, env.Dir(build_path))
		targets += [target]
	return targets

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
	'audio',
	'audio/backend',
	'audio/backend/openal',
	'audio/io',
	'base',
	'gui',
	'gui/ion',
	'gui/logic',
	'gui/widgets',
	'image',
	'ion',
	'math',
	'memory',
	'package',
	'renderer',
	'renderer/gl',
	'script',
	'window',
	'window/bindings']

if not option_no_jpeg:
	src_paths += ['image/jpeg']
if not option_no_ogg_vorbis:
	src_paths += ['audio/io/ogg_vorbis']
if not option_no_png:
	src_paths += ['image/png']

if 'posix' in ports:
	src_paths += [
		'base/posix']

if 'windows' in ports:
	src_paths += [
		'base/windows',
		'window/windows']

if 'x11' in ports:
	src_paths += [
		'window/x11']

src_target = 'lib/yttrium'
src_sources = [src_env.Glob('$BUILD/src/' + path + '/*.cpp') for path in src_paths]

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
# Tools
#-------------------------------------------------------------------------------

Alias('tools', BuildSources(env, 'tools', [
	'generate-sounds',
	'generate-test-images',
	'ypq']))
Clean('tools', Dir('$BUILD/tools'))

#-------------------------------------------------------------------------------
# Examples
#-------------------------------------------------------------------------------

Alias('examples', BuildSources(env, 'examples', [
	'rts',
	'tetrium']))
Clean('examples', Dir('$BUILD/examples'))

#===============================================================================
# All targets
#===============================================================================

Alias('all', ['yttrium', 'tests', 'tools', 'examples'])
Clean('all', Dir(['bin', 'lib', '$BUILD']))
