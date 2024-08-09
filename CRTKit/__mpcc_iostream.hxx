/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

/// @brief CRT exit, with exit code (!!! exits all threads. !!!)
/// @param code 
/// @return 
extern "C" int __exit(int code);

/// @brief Standard C++ namespace
namespace std
{
    template <typename Encoding>  
	class ofstream;

    template <typename Encoding>  
    class ostream
    {
    public:
        ostream() = default;
        virtual ~ostream() = default;

        virtual ostream<Encoding>& operator<<(const Encoding* input_streamable) = 0;
        virtual ostream<Encoding>& operator<<(Encoding* input_streamable) = 0;
    };
    
	extern ofstream<char> cout;
};