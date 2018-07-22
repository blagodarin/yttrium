Param(
  [String]$Prefix,
  [Switch]$All,
  [Switch]$WithLibjpeg,
  [Switch]$WithNasm,
  [Switch]$WithOpenAL,
  [Switch]$WithOpenGL
)

Import-Module BitsTransfer

$7z = "$((Get-ItemProperty -Path HKLM:SOFTWARE\7-Zip -Name Path).Path)7z.exe"

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

New-Item -ItemType "directory" -Name $Prefix -Force | Out-Null
Push-Location -Path $Prefix
$Prefix = Get-Location
New-Item -ItemType "directory" -Name "tmp" -Force | Out-Null
Push-Location -Path "tmp"

If ($All -Or $WithNasm) {
  Y-Download "http://www.nasm.us/pub/nasm/releasebuilds/2.13.01/win64/nasm-2.13.01-win64.zip"
  Y-Extract "nasm-2.13.01-win64.zip" "nasm-2.13.01"
  New-Item -ItemType "directory" -Name "$($Prefix)/bin" -Force | Out-Null
  Copy-Item -Path "nasm-2.13.01/nasm.exe" -Destination "$($Prefix)/bin"
}

If ($All -Or $WithLibjpeg) {
  Y-Download "https://downloads.sourceforge.net/project/libjpeg-turbo/1.5.3/libjpeg-turbo-1.5.3.tar.gz"
  & $7z x -y "libjpeg-turbo-1.5.3.tar.gz" | Out-Null
  Remove-Item -Path "libjpeg-turbo-1.5.3" -Recurse -Force
  & $7z x "libjpeg-turbo-1.5.3.tar" | Out-Null
  Push-Location -Path "libjpeg-turbo-1.5.3"
  cmake -G "Visual Studio 15 2017 Win64" . "-DCMAKE_INSTALL_PREFIX=$($Prefix)" -DWITH_TURBOJPEG=OFF
  cmake --build . --config Release --target INSTALL
  Pop-Location
}

If ($All -Or $WithOpenAL) {
  Y-Download "http://openal-soft.org/openal-binaries/openal-soft-1.18.2-bin.zip"
  Y-Extract "openal-soft-1.18.2-bin.zip" "openal-soft-1.18.2-bin"
  Copy-Item -Path "openal-soft-1.18.2-bin/include" -Destination "$($Prefix)" -Recurse -Force
  Copy-Item -Path "openal-soft-1.18.2-bin/libs" -Destination "$($Prefix)" -Recurse -Force
  Y-Download "https://openal.org/downloads/oalinst.zip"
  Y-Extract "oalinst.zip" "oalinst.exe"
  .\oalinst.exe /s
}

If ($All -Or $WithOpenGL) {
  New-Item -ItemType "directory" -Name "$($Prefix)/include/GL" | Out-Null
  Y-Download-To "https://khronos.org/registry/OpenGL/api/GL/glext.h" "$($Prefix)/include/GL"
  Y-Download-To "https://khronos.org/registry/OpenGL/api/GL/wglext.h" "$($Prefix)/include/GL"
  New-Item -ItemType "directory" -Name "$($Prefix)/include/KHR" | Out-Null
  Y-Download-To "https://khronos.org/registry/EGL/api/KHR/khrplatform.h" "$($Prefix)/include/KHR"
}

Pop-Location
Pop-Location
