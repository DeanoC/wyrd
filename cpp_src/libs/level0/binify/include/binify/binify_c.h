#ifndef BINIFY_BINIFY_C_API_H
#define BINIFY_BINIFY_C_API_H

// a c style interface but using C++ objects
#if defined(__cplusplus)
#include <string>
bool BINIFY_StringToOStream( std::string const& txt_, std::ostream *out_, std::string& log_ );
#endif

struct BINIFY_HANDLE {} ;

extern "C" BINIFY_HANDLE const * BINIFY_Alloc();
extern "C" int BINIFY_Parse( BINIFY_HANDLE const * handle_, char const * const in_);
extern "C" size_t BINIFY_BinarySize( BINIFY_HANDLE const * handle_ );
extern "C" uint8_t const *BINIFY_BinaryData( BINIFY_HANDLE const * handle_ );
extern "C" void BINIFY_Free( BINIFY_HANDLE const * handle_ );

#endif //BINIFY_BINIFY_C_API_H
