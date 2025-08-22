#ifndef SDL_EXCEPTION_H
#define SDL_EXCEPTION_H

#include <stdexcept>
#include <string>

#include "sdlpp_export.h"

namespace sdlpp {

class SDLPP_EXPORT Exception : public std::runtime_error {
 public:
  /**!
   * @brief Construct an exception
   *
   * @param[cause] The cause of the error, usually the SDL function name
   * which returned < 0
   */
  explicit Exception(const std::string& cause);

  Exception(const Exception& other) = default;
  auto operator=(const Exception& other) -> Exception& = default;
  Exception(Exception&& other) = default;
  auto operator=(Exception&& other) -> Exception& = default;
  ~Exception() override = default;

 private:
  std::string _cause;
  std::string _error;
};

}  // namespace sdlpp

#endif  // SDL_EXCEPTION_H
