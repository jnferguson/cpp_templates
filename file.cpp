#include "file.hpp"

bool 
file_base_t::int_open(const std::string& n, std::ios_base::openmode m)
{
	if (m_file.is_open()) {
		m_file.flush();
		m_file.close();
	}

	file_state_clear_t::clear(m_file);

	m_file.open(n, m);

	if (! m_file.is_open() || m_file.fail())
		return false;

	return true;
}

bool 
file_base_t::int_read(std::vector< uint8_t >& v, std::size_t s)
{
	if (0 == s)
		return true;

	if (! m_file.is_open() || ! m_file.good())
		return false;
	
	file_state_clear_t::clear(m_file);

	v.clear();
	v.resize(s);
	std::memset(v.data(), 0, v.size());

	m_file.read(reinterpret_cast< char* >(v.data()), s);

	if (m_file.fail()) {
		v.clear();
		return false;
	}

	return true;
}

bool 
file_base_t::int_write(std::vector< uint8_t >& v)
{
	if (! m_file.is_open())
		return false;

	file_state_clear_t::clear(m_file);

	m_file.write(reinterpret_cast< char* >(v.data()), v.size());
	
	if (m_file.fail())
		return false;

	return true;
}

bool 
file_base_t::int_flush(void)
{
	if (! m_file.is_open() || ! m_file.good())
		return false;

	file_state_clear_t::clear(m_file);

	m_file.flush();

	if (m_file.fail())
		return false;

	return true;
}

bool
file_base_t::int_sync(void)
{
	if (! m_file.is_open() || ! m_file.good())
		return false;

	file_state_clear_t::clear(m_file);

	if (0 != m_file.sync())
		return false;

	return true;
}

void
file_base_t::close(void)
{
	if (true == m_file.is_open())
		m_file.close();

	return;
}

bool
file_base_t::read(std::vector< uint8_t >& v, std::size_t s)
{
	return int_read(v, s);
}

bool
file_base_t::write(std::vector< uint8_t >& v)
{
	return int_write(v);
}

bool
file_base_t::flush(void)
{
	return int_flush();
}

bool
file_base_t::sync(void)
{
	return int_sync();
}

bool 
file_base_t::seek(ssize_t pos, seek_pos_t dir)
{
	if (! m_file.is_open() || ! m_file.good())
		return false;

	file_state_clear_t::clear(m_file);

	switch (dir) {
		case seek_pos_t::pos_current:
			m_file.seekg(pos, m_file.cur);
			break;

		case seek_pos_t::pos_begin:
			m_file.seekg(pos, m_file.beg);
			break;

		case seek_pos_t::pos_end:
			m_file.seekg(pos, m_file.end);

		default:
			throw std::runtime_error("file_base_t::seek(): Invalid seek direction specified");
			break;
	}

	if (! m_file.good())
		return false;

	return true;
}

bool 
file_base_t::seek_begin(void)
{
	if (! m_file.is_open() || ! m_file.good())
		return false;

	file_state_clear_t::clear(m_file);

	m_file.seekg(0, m_file.beg);

	if (! m_file.good())
		return false;
	
	return true;
}

bool 
file_base_t::seek_end(void)
{
	if (! m_file.is_open() || ! m_file.good())
		return false;

	file_state_clear_t::clear(m_file);

	m_file.seekg(0, m_file.end);

	if (! m_file.good())
		return false;

	return true;
}

std::size_t 
file_base_t::size(void)
{
	std::size_t r(0);
	std::size_t p(0);

	if (! m_file.is_open() || ! m_file.good())
		throw std::runtime_error("file_base_t::size(): State error: " + error());

	file_state_clear_t::clear(m_file);
	p = m_file.tellg();

	if (! m_file.good())
		throw std::runtime_error("file_base_t::size(): Error retrieving file position: " + error());

	file_state_clear_t::clear(m_file);

	if (false == seek_end())
		throw std::runtime_error("file_base_t::size(): Error seeking to end of file: " + error());
	
	r = m_file.tellg();

	if (false == seek(p, seek_pos_t::pos_begin))
		throw std::runtime_error("file_base_t::size(): Error resetting file position: " + error());

	return r;
}


input_file_t::input_file_t(void) : file_base_t()
{
	return;
}

input_file_t::input_file_t(const std::string& n, file_mode_t m) : file_base_t()
{
	if (false == int_open(n, m))
		throw std::runtime_error("input_file_t::input_file_t(): Error opening file: " + error());

	return;
}

input_file_t::input_file_t(const char* n, std::ios_base::openmode m)
{
	if (false == int_open(std::string(n), m))
		throw std::runtime_error("input_file_t::input_file_t(): Error opening file: " + error());

	return;
}

input_file_t::~input_file_t(void)
{
	sync();
	flush();
	close();

	return;
}

bool
input_file_t::open(const std::string& n, file_mode_t m)
{
	return int_open(n, m);
}

bool
input_file_t::open(const char* n, file_mode_t m)
{
	return int_open(n, m);
}


output_file_t::output_file_t(void) : file_base_t()
{
	return;
}

output_file_t::output_file_t(const std::string& n, file_mode_t m)
{
	if (false == int_open(n, m))
		throw std::runtime_error("output_file_t::output_file_t(): Error opening file: " + error());
	
	return;
}

output_file_t::output_file_t(const char* n, file_mode_t m)
{
	if (false == int_open(n, m))
		throw std::runtime_error("output_file_t::output_file_t(): Error opening file: " + error());

	return;
}

output_file_t::~output_file_t(void)
{
	flush();
	close();

	return;
}

bool
output_file_t::open(const std::string& n, file_mode_t m)
{
	return int_open(n, m);
}

bool
output_file_t::open(const char* n, file_mode_t m)
{
	return int_open(n, m);
}

file_t::file_t(void) : file_base_t()
{
	return;
}

file_t::file_t(const std::string& n, file_mode_t m)
{
	if (false == int_open(n, m))
		throw std::runtime_error("file_t::file_t: Error while opening file: " + error());
	
	return;
}

file_t::file_t(const char* n, file_mode_t m)
{
	if (false == int_open(std::string(n), m))
		throw std::runtime_error("file_t::file_t: Error while opening file: " + error());

	return;
}

file_t::~file_t(void)
{
	flush();
	close();

	return;
}

bool
file_t::open(const std::string& n, file_mode_t m)
{
	return int_open(n, m);
}

bool
file_t::open(const char* n, file_mode_t m)
{
	return int_open(std::string(n), m);
}
