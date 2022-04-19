#include "opengl_dependencies.h"
#include "opengl_errors.h"

#include <SDL_opengl.h>
#include <sstream>

namespace pbr::shared::apis::graphics::opengl {
    std::optional<std::string> check_opengl_error_internal(const char* file_name, uint32_t line_number) {
        GLenum error = GL_NO_ERROR;
        std::stringstream error_message;

        while ((error = glGetError()) != GL_NO_ERROR) {
            std::string error_string;

            switch (error) {
                case GL_INVALID_ENUM: {
                    error_string = "GL_INVALID_ENUM";
                    break;
                }
                case GL_INVALID_VALUE: {
                    error_string = "GL_INVALID_VALUE";
                    break;
                }
                case GL_INVALID_OPERATION: {
                    error_string = "GL_INVALID_OPERATION";
                    break;
                }
                case GL_INVALID_FRAMEBUFFER_OPERATION: {
                    error_string = "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                }
                case GL_OUT_OF_MEMORY: {
                    error_string = "GL_OUT_OF_MEMORY";
                    break;
                }
#if !defined(PLATFORM_IOS)
                case GL_STACK_UNDERFLOW: {
                    error_string = "GL_STACK_UNDERFLOW";
                    break;
                }
                case GL_STACK_OVERFLOW: {
                    error_string = "GL_STACK_OVERFLOW";
                    break;
                }
#endif
                default: {
                    error_string = "Unknown error";
                    break;
                }
            }

            error_message << "**********" << std::endl;
            error_message << "OpenGL Error" << std::endl;
            error_message << "File: " << file_name << ":" << line_number << std::endl;
            error_message << "Number: " << static_cast<uint32_t>(error) << std::endl;
            error_message << "String: " << error_string << std::endl;
            error_message << "**********" << std::endl;
        }

        if (error_message.tellp() > 0) {
            return error_message.str();
        } else {
            return {};
        }
    }
}
