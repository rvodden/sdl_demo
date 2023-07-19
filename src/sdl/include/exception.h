#ifndef __SDL_EXCEPTION_H__
#define __SDL_EXCEPTION_H__

#include <stdexcept>
#include <string>

namespace sdl {

class Exception : public std::runtime_error {
  public:
    /**!
     * @brief Construct an exception
     * 
     * @param[cause] The cause of the error, usually the SDL function name which returned < 0
    */
    explicit Exception(std::string cause);

    //! @brief copy constructor;
    Exception(const Exception& other) = default;
    //! @brief destructor;  
    ~Exception() noexcept {};

  private:
    std::string _cause;
    std::string _error;
};

}

#endif
