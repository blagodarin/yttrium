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

If (-Not (Test-Path -Path "build")) { New-Item -ItemType "directory" -Name "build" | Out-Null }
Push-Location -Path "build"

If (Test-Path -Path "prefix") { Remove-Item -Path "prefix" -Recurse -Force }
New-Item -ItemType "directory" -Name "prefix" | Out-Null
New-Item -ItemType "directory" -Name "prefix/include" | Out-Null
New-Item -ItemType "directory" -Name "prefix/libs" | Out-Null

# nasm
Y-Download "http://www.nasm.us/pub/nasm/releasebuilds/2.13.01/win64/nasm-2.13.01-win64.zip"
Y-Extract "nasm-2.13.01-win64.zip" "nasm-2.13.01"

# libjpeg-turbo
Y-Download "https://downloads.sourceforge.net/project/libjpeg-turbo/1.5.3/libjpeg-turbo-1.5.3.tar.gz"

# OpenAL
Y-Download "http://openal-soft.org/openal-binaries/openal-soft-1.18.2-bin.zip"
Y-Extract "openal-soft-1.18.2-bin.zip" "openal-soft-1.18.2-bin"
Copy-Item -Path "openal-soft-1.18.2-bin/include" -Destination "prefix" -Recurse -Force
Copy-Item -Path "openal-soft-1.18.2-bin/libs" -Destination "prefix" -Recurse -Force
Y-Download "https://openal.org/downloads/oalinst.zip"
Y-Extract "oalinst.zip" "oalinst.exe"

# OpenGL
New-Item -ItemType "directory" -Name "prefix/include/GL" | Out-Null
Y-Download-To "https://khronos.org/registry/OpenGL/api/GL/glext.h" "prefix/include/GL"
Y-Download-To "https://khronos.org/registry/OpenGL/api/GL/wglext.h" "prefix/include/GL"
New-Item -ItemType "directory" -Name "prefix/include/KHR" | Out-Null
Y-Download-To "https://khronos.org/registry/EGL/api/KHR/khrplatform.h" "prefix/include/KHR"

Pop-Location
