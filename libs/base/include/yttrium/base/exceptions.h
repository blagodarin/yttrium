// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdexcept>

namespace Yt
{
	/// Thrown if the initialization of an object (e.g. Window) fails.
	/// This usually means the inability to use the corresponding object.
	class InitializationError : public std::runtime_error
	{
	public:
		explicit InitializationError(const std::string& message)
			: std::runtime_error{ message } {}
	};

	/// Thrown if a resource can't be loaded from the supplied data.
	class DataError : public std::runtime_error
	{
	public:
		explicit DataError(const std::string& message)
			: std::runtime_error{ message } {}
	};

	///
	class IonError : public DataError
	{
	public:
		///
		explicit IonError(const std::string& message)
			: DataError{ message } {}
	};

	/// Thrown when required data is missing.
	class MissingDataError : public std::runtime_error
	{
	public:
		explicit MissingDataError(const std::string& message)
			: std::runtime_error{ message } {}
	};

	/// Thrown by ResourceLoader if it is unable to find the specified resource.
	class ResourceError : public MissingDataError
	{
	public:
		explicit ResourceError(const std::string& message)
			: MissingDataError{ message } {}
	};
}
