#include "constructor.h"

namespace pbr::shared::scene {
    /// Reads a location from the passed settings
    /// \param settings The settings to read the location from
    /// \param key The key of the location
    /// \returns The location if it was able to be read, else an empty value
    [[nodiscard]]
    static std::optional<location_data> get_location(std::optional<data::settings> settings,
                                                     std::string_view key) {
        assert((settings));

        auto location_settings = settings->get_as_settings(std::string{ key });
        if (!location_settings) {
            return {};
        }

        location_data location;

        if (auto x = settings->get_as_int("x"); x) {
            location.x = *x;
        } else {
            return {};
        }

        if (auto y = settings->get_as_int("y"); y) {
            location.y = *y;
        } else {
            return {};
        }

        location.z = settings->get_as_int("z");

        return location;
    }

    /// Adds any settings based cameras to the passed scene
    /// \param settings The settings containing the camera
    /// \param log_manager The log manager
    /// \returns The camera data if all required fields were read
    [[nodiscard]]
    static std::optional<camera_data> read_camera(std::optional<data::settings> camera_settings,
                                                  const std::shared_ptr<apis::logging::ilog_manager>& log_manager) noexcept {
        assert((camera_settings));
        assert((log_manager));

        camera_data camera;

        if (auto name = camera_settings->get("name"); name) {
            camera.name = *name;
        } else {
            log_manager->log_message("Failed to read `name` from camera.",
                                     apis::logging::log_levels::warning,
                                     "Scene");
            return {};
        }

        camera.layer = camera_settings->get_as_int("layer");

        camera.render_target_name = camera_settings->get("render_target_name");

        if (auto system = camera_settings->get("system"); system) {
            if (system == "2d") {
                camera.system = render_systems_data::_2d;
            } else if (system == "3d") {
                camera.system = render_systems_data::_3d;
            } else {
                log_manager->log_message("Invalid system when reading camera: " + *system,
                                         apis::logging::log_levels::warning,
                                         "Scene");
                return {};
            }
        } else {
            log_manager->log_message("Failed to read `system` from camera.",
                                     apis::logging::log_levels::warning,
                                     "Scene");
            return {};
        }

        if (auto position = get_location(camera_settings, "position"); position) {
            camera.position = *position;
        } else {
            log_manager->log_message("Failed to read `position` from camera.",
                                     apis::logging::log_levels::warning,
                                     "Scene");
            return {};
        }

        camera.focus = get_location(camera_settings, "focus");

        camera.up = get_location(camera_settings, "up");

        return camera;
    }

    /// Adds any settings based cameras to the passed scene
    /// \param scene The scene to add the cameras to
    /// \param settings The settings containing the cameras
    /// \param log_manager The log manager
    static void add_cameras(const std::shared_ptr<scene_base>& scene,
                            std::optional<data::settings> settings,
                            const std::shared_ptr<apis::logging::ilog_manager>& log_manager) noexcept {
        assert((scene));
        assert((settings));
        assert((log_manager));

        auto cameras = settings->get_as_settings_array("cameras");
        if (!cameras || cameras->empty()) {
            return;
        }

        for (const auto& camera : *cameras) {
            if (auto camera_data = read_camera(camera, log_manager); camera_data) {

            } else {
                log_manager->log_message("Failed to read camera settings.",
                                         apis::logging::log_levels::warning,
                                         "Scene");
            }
        }
    }

    void constructor::construct(const std::shared_ptr<scene_base>& scene,
                                const std::optional<data::settings>& settings,
                                const std::shared_ptr<apis::logging::ilog_manager>& log_manager) noexcept {
        assert((scene));
        assert((settings));
        assert((log_manager));

        add_cameras(scene, settings, log_manager);
    }
}
