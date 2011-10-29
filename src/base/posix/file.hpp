#ifndef __BASE_POSIX_FILE_HPP
#define __BASE_POSIX_FILE_HPP

#include <Yttrium/file.hpp>

namespace Yttrium
{

class FileReaderImpl: public FileReader
{
	friend class FileReader;

public:

	FileReaderImpl() throw();

	bool open_file(const StaticString &name) throw();

public: // FileReader

	virtual ~FileReaderImpl() throw();

	virtual FileReaderPtr dup(Allocator *allocator);

	virtual size_t read(void *buffer, size_t size) throw();

	virtual UOffset offset() throw();

	virtual bool seek(Offset offset, Whence whence) throw();

	virtual UOffset size() throw();

private:

	FileReaderImpl(int descriptor) throw();

	FileReaderImpl(int descriptor, UOffset offset, UOffset size) throw();

private:

	int     _descriptor;
	UOffset _offset;
	UOffset _size;
};

class FileWriterImpl: public FileWriter
{
	friend class FileWriter;

public:

	FileWriterImpl() throw();

	bool open_file(const StaticString &name) throw();

public: // FileWriter

	virtual ~FileWriterImpl() throw();

	virtual FileWriterPtr dup(Allocator *allocator);

	virtual size_t write(const void *buffer, size_t size) throw();

	virtual bool flush() throw();

	virtual UOffset offset() throw();

	virtual bool seek(Offset offset, Whence whence) throw();

	virtual UOffset size() throw();

	virtual bool resize(UOffset size) throw();

	virtual bool truncate() throw();

private:

	FileWriterImpl(int descriptor) throw();

	FileWriterImpl(int descriptor, UOffset offset) throw();

private:

	int     _descriptor;
	UOffset _offset;
};

} // namespace Yttrium

#endif // __BASE_POSIX_FILES_HPP
