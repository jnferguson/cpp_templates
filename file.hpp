/* From memory there is a bug in here related to the file modes and my having the wrong default. I think this is the wrong 
   fstream wrapper class though? I can't seem to find the normal one though so this will do for now. I seem to recall a file
   mode related bug that causes all files to fail that ive just never pushed back upstream.
*/
#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>
#include <mutex>
#include <ios>

class file_state_clear_t
{
	private:
		file_state_clear_t(void) {}

	protected:
	public:
		~file_state_clear_t(void) { }

		static void 
		clear(std::fstream& f)
		{
			f.clear();
			errno = 0;

			return;
		}
};

typedef file_state_clear_t file_clear_t;


typedef std::fstream::openmode file_mode_t;
#define FM_INPUT std::fstream::in
#define FM_OUTPUT std::fstream::out
#define FM_READWRITE (FM_INPUT|FM_OUTPUT)
#define FM_BINARY std::fstream::binary
#define FM_BEGIN std::fstream::beg
#define FM_END std::fstream::ate
#define FM_APPEND std::fstream::app
#define FM_TRUNCATE std::fstream::trunc

#define FILE_READ_DEFAULT (static_cast< file_mode_t >((FM_INPUT | FM_BINARY | FM_BEGIN)))
#define FILE_WRITE_DEFAULT (static_cast< file_mode_t >((FM_OUTPUT | FM_BINARY | FM_END|FM_APPEND)))
#define FILE_READ_WRITE_DEFAULT (static_cast< file_mode_t >((FM_INPUT|FM_OUTPUT|FM_BINARY|FM_BEGIN|FM_TRUNCATE)))
#define FILE_WRITE_TRUNCATE (static_cast< file_mode_t >((FM_OUTPUT|FM_BINARY|FM_TRUNCATE)))
#define FILE_WRITE_APPEND FILE_WRITE_DEFAULT

typedef enum {
	pos_current = 0,
	pos_begin,
	pos_end
} seek_pos_t;

class file_base_t
{
	private:
	protected:

		std::fstream 	m_file;
		std::string		m_name;
		file_mode_t		m_mode;

		virtual bool int_open(const std::string&, file_mode_t);
		virtual bool int_read(std::vector< uint8_t >&, std::size_t);
		virtual bool int_write(std::vector< uint8_t >&);
		virtual bool int_flush(void);
		virtual bool int_sync(void);

	public:
		file_base_t(void) {}
		virtual ~file_base_t(void) {}
		
		virtual bool open(const std::string&, file_mode_t) = 0;
		virtual void close(void);

		virtual bool read(std::vector< uint8_t >&, std::size_t);
		virtual bool write(std::vector< uint8_t >&);
		
		virtual bool flush(void);
		virtual bool sync(void);

		virtual bool seek(ssize_t, seek_pos_t dir = pos_current);
		virtual bool seek_begin(void);
		virtual bool seek_end(void);
		virtual std::size_t size(void);

		virtual file_mode_t mode(void) { return m_mode; }
		virtual std::string name(void) { return m_name; }
		virtual std::string error(signed int e = errno) { return std::strerror(e); }

		virtual bool is_read(void) { return FM_INPUT == (m_mode & FM_INPUT); }
		virtual bool is_write(void) { return FM_OUTPUT == (m_mode & FM_OUTPUT); }
		virtual bool is_read_write(void) { return FM_READWRITE == (m_mode & FM_READWRITE); }
};

class file_t : public file_base_t
{
    private:
    protected:
    public:
		file_t(void);
		file_t(const std::string&, file_mode_t m = FILE_READ_WRITE_DEFAULT);
		file_t(const char*, file_mode_t m = FILE_READ_WRITE_DEFAULT);
		virtual ~file_t(void);

		virtual bool open(const std::string&, file_mode_t m = FILE_READ_WRITE_DEFAULT);
		virtual bool open(const char*, file_mode_t m = FILE_READ_WRITE_DEFAULT);
};


class input_file_t : public file_base_t
{
	private:
	protected:
	public:
		input_file_t(void);
		input_file_t(const std::string&, file_mode_t m = FILE_READ_DEFAULT);
		input_file_t(const char*, file_mode_t m = FILE_READ_DEFAULT);
		virtual ~input_file_t(void);

		virtual bool open(const std::string&, file_mode_t m = FILE_READ_DEFAULT);
		virtual bool open(const char*, file_mode_t m = FILE_READ_DEFAULT);
};

class output_file_t : public file_base_t
{
    private:
		file_t m_file;
    protected:
    public:
       	output_file_t(void);
        output_file_t(const std::string&, file_mode_t m = FILE_WRITE_DEFAULT);
        output_file_t(const char*, file_mode_t m = FILE_WRITE_DEFAULT);
        virtual ~output_file_t(void);

		virtual bool open(const std::string&, file_mode_t m = FILE_WRITE_DEFAULT);
		virtual bool open(const char*, file_mode_t m = FILE_WRITE_DEFAULT);
};
