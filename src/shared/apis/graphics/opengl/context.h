#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <cassert>

namespace pbr::shared::apis::graphics::opengl {
    /// The OpenGL context
    class context final {
    public:
        context(SDL_Window* window) {
            assert((window));

            this->_context = SDL_GL_CreateContext(window);
        }
        ~context() {
            SDL_GL_DeleteContext(this->_context);
            this->_context = nullptr;
        }

    private:
        /// The OpenGL context
        SDL_GLContext _context {nullptr};
    };
}
