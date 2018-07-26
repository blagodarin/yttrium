Param(
  [String]$prefix,
  [Switch]$all,
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
$build = @{}
$build.libjpeg = $libjpeg -or $all
$build.libogg = $libogg -or $all -or $libvorbis
$build.libpng = $libpng -or $all
$build.libvorbis = $libvorbis -or $all
$build.nasm = $nasm -or $all -or $libjpeg -or $libpng
$build.openal = $openal -or $all
$build.opengl = $opengl -or $all
$build.zlib = $zlib -or $all -or $libpng

$cmake_generator = "Visual Studio 15 2017 Win64"

Import-Module BitsTransfer

$7z = "$((Get-ItemProperty -Path HKLM:SOFTWARE\7-Zip -Name Path).Path)7z.exe"

Function Y-Directory([String] $path) {
  If (-Not (Test-Path -Path $path)) {
    New-Item -ItemType "directory" -Name "$($prefix)/bin" -Force | Out-Null
  }
}

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

New-Item -ItemType "directory" -Name $prefix -Force | Out-Null
Push-Location -Path $prefix
$prefix = Get-Location
New-Item -ItemType "directory" -Name "tmp" -Force | Out-Null
Push-Location -Path "tmp"

If ($build.nasm) {
  Y-Download "http://www.nasm.us/pub/nasm/releasebuilds/2.13.01/win64/nasm-2.13.01-win64.zip"
  Y-Extract "nasm-2.13.01-win64.zip" "nasm-2.13.01"
  Y-Directory "$($prefix)/bin"
  Copy-Item -Path "nasm-2.13.01/nasm.exe" -Destination "$($prefix)/bin"
  $env:ASM = "$($prefix)/bin/nasm.exe"
}

If ($build.libjpeg) {
  Y-Download "https://downloads.sourceforge.net/project/libjpeg-turbo/1.5.3/libjpeg-turbo-1.5.3.tar.gz"
  & $7z x -y "libjpeg-turbo-1.5.3.tar.gz" | Out-Null
  Remove-Item -Path "libjpeg-turbo-1.5.3" -Recurse -Force
  & $7z x "libjpeg-turbo-1.5.3.tar" | Out-Null
  Push-Location -Path "libjpeg-turbo-1.5.3"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)" -DWITH_TURBOJPEG=OFF
  cmake --build . --config Release --target INSTALL
  Pop-Location
}

If ($build.openal) {
  Y-Download "http://openal-soft.org/openal-binaries/openal-soft-1.18.2-bin.zip"
  Y-Extract "openal-soft-1.18.2-bin.zip" "openal-soft-1.18.2-bin"
  Copy-Item -Path "openal-soft-1.18.2-bin/include" -Destination "$($prefix)" -Recurse -Force
  Copy-Item -Path "openal-soft-1.18.2-bin/libs" -Destination "$($prefix)" -Recurse -Force
  Y-Download "https://openal.org/downloads/oalinst.zip"
  Y-Extract "oalinst.zip" "oalinst.exe"
  .\oalinst.exe /s
}

If ($build.opengl) {
  New-Item -ItemType "directory" -Name "$($prefix)/include/GL" | Out-Null
  Y-Download-To "https://khronos.org/registry/OpenGL/api/GL/glext.h" "$($prefix)/include/GL"
  Y-Download-To "https://khronos.org/registry/OpenGL/api/GL/wglext.h" "$($prefix)/include/GL"
  New-Item -ItemType "directory" -Name "$($prefix)/include/KHR" | Out-Null
  Y-Download-To "https://khronos.org/registry/EGL/api/KHR/khrplatform.h" "$($prefix)/include/KHR"
}

If ($build.zlib) {
  Y-Download "https://zlib.net/zlib-1.2.11.tar.xz"
  & $7z x -y "zlib-1.2.11.tar.xz" | Out-Null
  Remove-Item -Path "zlib-1.2.11" -Recurse -Force
  & $7z x "zlib-1.2.11.tar" | Out-Null
  Push-Location -Path "zlib-1.2.11"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)" -DSKIP_INSTALL_FILES=ON
  cmake --build . --config Release --target INSTALL
  Pop-Location
}

If ($build.libpng) {
  Y-Download "https://downloads.sourceforge.net/project/libpng/libpng16/1.6.35/libpng-1.6.35.tar.xz"
  & $7z x -y "libpng-1.6.35.tar.xz" | Out-Null
  Remove-Item -Path "libpng-1.6.35" -Recurse -Force
  & $7z x "libpng-1.6.35.tar" | Out-Null
  Push-Location -Path "libpng-1.6.35"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)" -DPNG_TESTS=OFF -DSKIP_INSTALL_EXECUTABLES=ON -DSKIP_INSTALL_EXPORT=ON -DSKIP_INSTALL_FILES=ON -DSKIP_INSTALL_PROGRAMS=ON
  cmake --build . --config Release --target INSTALL
  Pop-Location
}

If ($build.libogg) {
  Remove-Item -Path "ogg" -Recurse -Force
  git clone https://git.xiph.org/ogg.git
  Push-Location -Path "ogg"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)"
  cmake --build . --config Release --target INSTALL
  Pop-Location
}

If ($build.libvorbis) {
  Remove-Item -Path "vorbis" -Recurse -Force
  git clone https://git.xiph.org/vorbis.git
  Push-Location -Path "vorbis"
  cmake -G $cmake_generator . "-DCMAKE_INSTALL_PREFIX=$($prefix)" "-DOGG_ROOT=$($prefix)"
  cmake --build . --config Release --target INSTALL
  Pop-Location
}

Pop-Location
Pop-Location
