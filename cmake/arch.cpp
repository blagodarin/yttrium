//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#if defined(__amd64) || defined(__amd64__) || defined(__x86_64) || defined(__x86_64__) || defined(_M_AMD64) || defined(_M_X64)
#undef amd64
amd64
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
#undef x86
x86
#endif
