#include "opengl_dependencies.h"
#include "shared/apis/windowing/application_window.h"
#include "graphics_manager.h"
#include "opengl_errors.h"
#include "shared/apis/graphics/color.h"
#include "mesh_shapes.h"

#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

using namespace std::string_literals;

namespace pbr::shared::apis::graphics::opengl {
    /// Sets the api version
    void set_api_version() noexcept {
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
    void set_clear_color(color color) noexcept {
        glClearColor(color.r / 255.0f,
                     color.g / 255.0f,
                     color.b / 255.0f,
                     color.a / 255.0f);
    }

    /// Sets the stencil buffer size
    void setup_stencil_size() noexcept {
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    }

    /// Sets up blending
    void setup_blending() noexcept {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /// Sets up depth testing
    void setup_depth_testing() noexcept {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }

    bool graphics_manager::load_api(const std::filesystem::path&) noexcept {
        this->_log_manager->log_message("Loading the OpenGL graphics API...",
                                        logging::log_levels::info,
                                        "Graphics");

        if (SDL_VideoInit(nullptr) < 0) {
            this->_log_manager->log_message("Failed to init SDL with error: "s + SDL_GetError(),
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

        setup_stencil_size();

        setup_blending();

        setup_depth_testing();

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

    std::shared_ptr<render_targets::texture> graphics_manager::render_target(float x, float y, float z, float w, float h) {
        auto texture_target = std::make_shared<render_targets::texture>(1024, 768, this->_log_manager);

        std::shared_ptr<shader_program> shader;
        if (auto program = shader_program::create(this->_log_manager, this->_shader_manager,
                                                  { "color_vertex", "color_fragment" }); !program) {
            this->_log_manager->log_message("Failed to create shader program.",
                                            logging::log_levels::error,
                                            "Graphics");
            return {};
        } else {
            shader = *program;
        }

        auto mesh = create_rectangle(x, y, z, w, h, this->_log_manager);
        assert((mesh));

        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) 1024 / (float)768, 0.1f, 100.0f);

        glm::mat4 View = glm::lookAt(
            glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
            glm::vec3(0,0,0), // and looks at the origin
            glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

        texture_target->bind();
        texture_target->clear();
        shader->use();
        if (!shader->set_uniform("inMVP", mvp)) {}
        mesh->render();
        shader->clear_use();
        texture_target->unbind();

        return texture_target;
    }

    void graphics_manager::submit_frame_for_render() noexcept {
        // submit render targets to compositor

        auto texture_target1 = render_target(0.0f, 0.0f, -3.0f, 0.5f, 0.5f);
        auto texture_target2 = render_target(0.5f, 0.5f, -2.0f, 0.5f, 0.5f);
        auto texture_target3 = render_target(0.5f, 0.5f, -1.0f, 0.5f, 0.5f);

        this->_compositor->render({ texture_target1, texture_target2, texture_target3 });

        auto application_window = this->_window_manager->get_main_application_window();
        application_window->update_display();

        CHECK_OPENGL_ERROR_NO_RETURN(this->_log_manager);
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
        this->_screen_render_target = std::make_shared<render_targets::screen>(1024, 768);

        this->_compositor = std::make_shared<compositor>(this->_screen_render_target,
                                                         this->_shader_manager,
                                                         this->_log_manager);
    }

    void graphics_manager::sync_resolutions() noexcept {
        auto application_window = std::dynamic_pointer_cast<windowing::application_window>(
            this->_window_manager->get_main_application_window());
        assert((application_window));

        auto size = application_window->get_size();

        this->_screen_render_target->resize(size.width, size.height);
    }
}
