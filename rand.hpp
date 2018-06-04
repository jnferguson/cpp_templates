#pragma once
#include <cstdint>
#include <random>

class rand_t
{
	private:
		std::random_device                              m_dev;
		std::mt19937                                    m_rnd;
		std::uniform_int_distribution< uint64_t >       m_qword;
    
	protected:
	public:
	  rand_t(void) : m_rnd(m_dev()) { }
		~rand_t(void) { }
	
		uint8_t
		byte(void)
		{
			return static_cast< uint8_t >( m_qword(m_rnd) & UINT8_MAX );
		}

		uint16_t
		word(void)
		{
			return static_cast< uint16_t >( m_qword(m_rnd) & UINT16_MAX );
		}

    uint32_t
	  dword(void)
	  {
		  return static_cast< uint32_t >( m_qword(m_rnd) & UINT32_MAX);
	  }

	  uint64_t
	  qword(void)
	  {
		  return m_qword(m_rnd);
	  }
};
