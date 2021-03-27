// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#if defined(__amd64) || defined(__amd64__) || defined(__x86_64) || defined(__x86_64__) || defined(_M_AMD64) || defined(_M_X64)
#	undef amd64
amd64
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
#	undef x86
x86
#endif
