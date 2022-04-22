#include "opengl_dependencies.h"
#include "shared/apis/windowing/application_window.h"
#include "graphics_manager.h"
#include "opengl_errors.h"
#include "shared/apis/graphics/color.h"

#include <sstream>

namespace pbr::shared::apis::graphics::opengl {
    /// Sets the api version
    void set_api_version() {
#if defined(USE_OPENGL_ES)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
    }

    /// Sets the clear color
    /// \param color The color to clear to
    void set_clear_color(color color) {
        glClearColor(color.r / 255.0f,
                     color.g / 255.0f,
                     color.b / 255.0f,
                     color.a / 255.0f);
    }

    bool graphics_manager::load_api(const std::filesystem::path&) noexcept {
        this->_log_manager->log_message("Loading the OpenGL graphics API...",
                                        logging::log_levels::info,
                                        "Graphics");

        if (SDL_VideoInit(nullptr) < 0) {
            this->_log_manager->log_message("Failed to init SDL with error:",
                                            logging::log_levels::error,
                                            "Graphics");
            this->_log_manager->log_message(SDL_GetError(),
                                            logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        this->_log_manager->log_message("Loaded the graphics API.",
                                        logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    bool graphics_manager::initialize() noexcept {
        this->_log_manager->log_message("Initializing the graphics manager...",
                                        logging::log_levels::info,
                                        "Graphics");

        auto application_window = std::dynamic_pointer_cast<windowing::application_window>(
            this->_window_manager->get_main_application_window());
        assert((application_window));

        set_api_version();

        this->_context = std::make_shared<context>(application_window->get_native_handle());

        if (!this->setup_glew()) {
            this->_log_manager->log_message("Failed to initialize GLEW.",
                                            logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        if (!this->enable_vsync(false)) {
            this->_log_manager->log_message("Failed to disable vsync.",
                                            logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        set_clear_color(colors::black);

        this->_shader_manager = std::make_shared<shader_manager>(this->_data_manager,
                                                                 this->_log_manager,
                                                                 graphics_manager::shader_list_path);

        this->setup_compositor();

        this->sync_resolutions();

        this->_log_manager->log_message("Initialized the graphics manager.",
                                        logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    bool graphics_manager::refresh_resources() noexcept {
        return true;
    }

    bool graphics_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the graphics API...",
                                        logging::log_levels::info,
                                        "Graphics");

        this->_context.reset();

        SDL_VideoQuit();

        this->_log_manager->log_message("Shut down the graphics API.",
                                        logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    void graphics_manager::submit_renderable_entities(renderable_entities renderable_entities) noexcept {
        // OpenGL is single threaded, so no need for any mutexes here
        this->_renderable_entities = renderable_entities;
    }

    void graphics_manager::submit_frame_for_render() noexcept {
        // submit render targets to compositor

        this->_compositor->render({});

        auto application_window = this->_window_manager->get_main_application_window();
        application_window->update_display();
    }

    bool graphics_manager::setup_glew() const noexcept {
        glewExperimental = GL_TRUE;

        GLenum glewError = glewInit();
        if (glewError != GLEW_OK) {
            std::stringstream errorString;
            errorString << glewGetErrorString(glewError);
            this->_log_manager->log_message("Failed to load GLEW with the following error: " +
                                            errorString.str(),
                                            logging::log_levels::error,
                                            "Graphics");

            CHECK_OPENGL_ERROR(this->_log_manager);
            return false;
        }

        return true;
    }

    bool graphics_manager::enable_vsync(bool enable) const noexcept {
        if (SDL_GL_SetSwapInterval(enable ? 0 : 1) != 0) {
            CHECK_OPENGL_ERROR(this->_log_manager);
            return false;
        }

        return true;
    }

    void graphics_manager::setup_compositor() noexcept {
        this->_screen_render_target = std::make_shared<render_targets::screen>();

        this->_compositor = std::make_shared<compositor>(this->_screen_render_target,
                                                         this->_shader_manager);
    }

    void graphics_manager::sync_resolutions() noexcept {
        auto application_window = std::dynamic_pointer_cast<windowing::application_window>(
            this->_window_manager->get_main_application_window());
        assert((application_window));

        auto size = application_window->get_size();

        this->_screen_render_target->resize(size.width, size.height);
    }
}
