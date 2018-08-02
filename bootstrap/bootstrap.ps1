Param(
  [String]$prefix,
  [Switch]$all,
  [Switch]$only,
  [Switch]$libjpeg,
  [Switch]$libogg,
  [Switch]$libpng,
  [Switch]$libvorbis,
  [Switch]$nasm,
  [Switch]$openal,
  [Switch]$opengl,
  [Switch]$zlib
)

# Reverse dependencies.
$build           = @{}
$build.libogg    = $all -Or $libogg
$build.nasm      = $all -Or $nasm
$build.openal    = $all -Or $openal
$build.opengl    = $all -Or $opengl
$build.zlib      = $all -Or $zlib
$build.libjpeg   = $all -Or $libjpeg   -Or (-Not $only -And ($build.nasm))
$build.libpng    = $all -Or $libpng    -Or (-Not $only -And ($build.nasm -Or $build.zlib))
$build.libvorbis = $all -Or $libvorbis -Or (-Not $only -And ($build.libogg))

$cmake_generator = "Visual Studio 15 2017 Win64"

Import-Module BitsTransfer

Function Y-Download([String] $url) {
  $name = $url.Substring($url.LastIndexOf('/') + 1)
  If (-Not (Test-Path -Path $name)) { Start-BitsTransfer -Source $url }
}

Function Y-Download-To([String] $url, [String] $to) {
  Push-Location -Path $to
  Y-Download $url
  Pop-Location
}

Function Y-Extract([String] $archive, [String] $dir) {
  If (Test-Path -Path $dir) { Remove-Item -Path $dir -Recurse -Force }
  Expand-Archive -Path $archive -DestinationPath "."
}

Function Y-RemoveDirectory([String] $path) {
  If (Test-Path -Path $path) {
    Remove-Item -Path $path -Recurse -Force
  }
}

Function Y-Report([String] $package) {
  Write-Host "*** Getting $($package) ***"
}

New-Item -ItemType "directory" -Name $prefix -Force | Out-Null
Push-Location -Path $prefix
$prefix = Get-Location
New-Item -ItemType "directory" -Path $prefix -Name "tmp" -Force | Out-Null
Push-Location -Path "tmp"

If ($build.libogg) {
  Y-Report "libogg"
  Y-RemoveDirectory "ogg"
  git clone https://git.xiph.org/ogg.git
  Push-Location -Path "ogg"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)"
  cmake --build . --config Release --target INSTALL -- /nologo /verbosity:minimal
  Pop-Location
}

If ($build.nasm) {
  Y-Report "nasm"
  Y-Download "https://www.nasm.us/pub/nasm/releasebuilds/2.13.03/win64/nasm-2.13.03-win64.zip"
  Y-Extract "nasm-2.13.03-win64.zip" "nasm-2.13.03"
  New-Item -ItemType "directory" -Path $prefix -Name "bin" -Force | Out-Null
  Copy-Item -Path "nasm-2.13.03/nasm.exe" -Destination "$($prefix)/bin"
  $env:ASM = "$($prefix)/bin/nasm.exe"
}

If ($build.openal) {
  Y-Report "openal"
  Y-Download "http://openal-soft.org/openal-binaries/openal-soft-1.18.2-bin.zip"
  Y-Extract "openal-soft-1.18.2-bin.zip" "openal-soft-1.18.2-bin"
  Copy-Item -Path "openal-soft-1.18.2-bin/include" -Destination "$($prefix)" -Recurse -Force
  Copy-Item -Path "openal-soft-1.18.2-bin/libs" -Destination "$($prefix)" -Recurse -Force
  Y-Download "https://openal.org/downloads/oalinst.zip"
  Y-Extract "oalinst.zip" "oalinst.exe"
  .\oalinst.exe /s
}

If ($build.opengl) {
  Y-Report "opengl"
  New-Item -ItemType "directory" -Path $prefix -Name "include" -Force | Out-Null
  New-Item -ItemType "directory" -Path "$($prefix)/include" -Name "GL" -Force | Out-Null
  Y-Download-To "https://khronos.org/registry/OpenGL/api/GL/glext.h" "$($prefix)\include\GL"
  Y-Download-To "https://khronos.org/registry/OpenGL/api/GL/wglext.h" "$($prefix)\include\GL"
  New-Item -ItemType "directory"-Path "$($prefix)/include" -Name "KHR" -Force | Out-Null
  Y-Download-To "https://khronos.org/registry/EGL/api/KHR/khrplatform.h" "$($prefix)/include/KHR"
}

If ($build.zlib) {
  Y-Report "zlib"
  Y-Download "https://zlib.net/zlib-1.2.11.tar.xz"
  Y-RemoveDirectory "zlib-1.2.11"
  cmake -E tar xJ "zlib-1.2.11.tar.xz"
  Push-Location -Path "zlib-1.2.11"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)" -DSKIP_INSTALL_FILES=ON
  cmake --build . --config Release --target INSTALL -- /nologo /verbosity:minimal
  Pop-Location
}

If ($build.libjpeg) {
  Y-Report "libjpeg"
  Y-Download "https://downloads.sourceforge.net/project/libjpeg-turbo/1.5.3/libjpeg-turbo-1.5.3.tar.gz"
  Y-RemoveDirectory "libjpeg-turbo-1.5.3"
  cmake -E tar xz "libjpeg-turbo-1.5.3.tar.gz"
  Push-Location -Path "libjpeg-turbo-1.5.3"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)" -DWITH_TURBOJPEG=OFF
  cmake --build . --config Release --target INSTALL -- /nologo /verbosity:minimal
  Pop-Location
}

If ($build.libpng) {
  Y-Report "libpng"
  Y-Download "https://downloads.sourceforge.net/project/libpng/libpng16/1.6.35/libpng-1.6.35.tar.xz"
  Y-RemoveDirectory "libpng-1.6.35"
  cmake -E tar xJ "libpng-1.6.35.tar.xz"
  Push-Location -Path "libpng-1.6.35"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)" -DPNG_TESTS=OFF -DSKIP_INSTALL_EXECUTABLES=ON -DSKIP_INSTALL_EXPORT=ON -DSKIP_INSTALL_FILES=ON -DSKIP_INSTALL_PROGRAMS=ON
  cmake --build . --config Release --target INSTALL -- /nologo /verbosity:minimal
  Pop-Location
}

If ($build.libvorbis) {
  Y-Report "libvorbis"
  Y-RemoveDirectory "vorbis"
  git clone https://git.xiph.org/vorbis.git
  Push-Location -Path "vorbis"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)" "-DOGG_ROOT=$($prefix)"
  cmake --build . --config Release --target INSTALL -- /nologo /verbosity:minimal
  Pop-Location
}

Pop-Location
Pop-Location
