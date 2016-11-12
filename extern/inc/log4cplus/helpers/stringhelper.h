#ifndef LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
#define LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_

#include <log4cplus/config.h>
#include <log4cplus/tstring.h>

#include <iterator>
#include <algorithm>

namespace log4cplus
{
	namespace helpers
	{
		log4cplus::tstring toUpper(const log4cplus::tstring& s);
        log4cplus::tstring toLower(const log4cplus::tstring& s);

        template <class _StringType , class _OutputIter>
        void tokenize(const _StringType& s, typename _StringType::value_type c, 
                      _OutputIter _result, bool collapseTokens = true) 
        {
            _StringType tmp;
            for(typename _StringType::size_type i=0; i<s.length(); ++i) {
                if(s[i] == c) {
                    *_result = tmp;
                    ++_result;
                    tmp.erase(tmp.begin(), tmp.end());
                    if(collapseTokens)
                        while(s[i+1] == c) ++i;
                }
                else
                    tmp += s[i];
            }
            if(tmp.length() > 0) *_result = tmp;
        }
                 
        template<class intType>
        inline tstring convertIntegerToString(intType value) 
        {
            if(value == 0) {
                return LOG4CPLUS_TEXT("0");
            }
            
            char buffer[21];
            char ret[21];
            unsigned int bufferPos = 0;
            unsigned int retPos = 0;

            if(value < 0) {
                ret[retPos++] = '-';
            }
            
            // convert to string in reverse order
            while(value != 0) {
                intType mod = value % 10;
                value = value / 10;
                buffer[bufferPos++] = '0' + static_cast<char>(mod);
            }
            
            // now reverse the string to get it in proper order
            while(bufferPos > 0) {
                ret[retPos++] = buffer[--bufferPos];
            }
            ret[retPos] = 0;
            
            return LOG4CPLUS_C_STR_TO_TSTRING(ret);
        }
	
		template <class _Container>
		class string_append_iterator
		{
			protected :
					
				_Container* container;
		
			public:
				
				typedef _Container          container_type;
				typedef void                value_type;
				typedef void                difference_type;
				typedef void                pointer;
				typedef void                reference;
				typedef std::forward_iterator_tag iterator_category;

			explicit string_append_iterator(_Container& x) : container(&x) {}
            
			string_append_iterator<_Container>&
			operator=( const typename _Container::value_type& v )
			{
				*container += v;
				return *this;
			}

			string_append_iterator<_Container>& operator*() { return *this; }
            string_append_iterator<_Container>& operator++() { return *this; }
            string_append_iterator<_Container>& operator++(int) { return *this; }
        }; // class string_append_iterator
    } 
}

#endif // LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
