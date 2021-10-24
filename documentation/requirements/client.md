# Client

- [Client](#client)
  - [Graphics](#graphics)
    - [2D Graphics](#2d-graphics)
    - [3D Graphics](#3d-graphics)
  - [Window](#window)
  - [Audio](#audio)
  - [Input](#input)
  - [Networking](#networking)
  - [Scene](#scene)
  - [World](#world)
  - [Physics](#physics)
    - [2D Physics](#2d-physics)
    - [3D Physics](#3d-physics)
  - [UI](#ui)
  - [ECS](#ecs)
  - [Scripting](#scripting)

The player uses the client to connect to the server and play the game.

The client has these components - graphics, window, audio, input, networking, scene, world, physics, UI (User Interface), ECS (Entity Component System) and scripting.

Where possible, each component should work in its own thread and work independently of other components.

## Graphics

Both 2D and 3D rendering must be supported. Rendering should take place in its own thread. Data to be rendered should be pushed into this component and not read from other components.

### 2D Graphics

Rendering is done in screen space. Animated sprites, tile maps, fonts, particles and special effects are required.

### 3D Graphics

Ability to render both in screen space and world space is required. Height maps, static models, skeletal animation, particles, special effects, billboards, fluid, animated textures and reflections should be supported.

## Window

Ability to create game window required. Full screen and windowed mode in multiple resolutions should be supported.

## Audio

Both stereo and 3D audio must be supported. MIDI and multi-channel sound tracks must be able to be played.

Audio should be handled in its own thread.

## Input

Keyboard, mouse, touch screen and game pad support is required. The input source should be abstracted into actions, which is what the game system will act on.

Input can be handled in the same thread as the windowing system.

## Networking

TCP and UDP must be supported. Network packets that must be received by the server should be sent via TCP. Packets that are not required should be sent over UDP.

UDP packets will be sent with a timestamp. This will help the server discard packets that are no longer up-to-date.

## Scene

Manages the current scene the player is in. A scene may or may not contain a world. The required scenes are:

* Loading
  * Handles the loading of worlds, various components and displays the loading UIs
* World
  * Contains the current running world and the UIs

## World

Receives all data from the server, so acts as reflection of the world on the server.

Avatars in the world take their input by means of actions. These actions come from either the server, or in the case of the player's avatar, from the player through the input system. The player's avatar data will be sent to the server.

The world is represented as a 2D grid of tiles. This grid of tiles is kept up to date on the client by the server sending any updates over the required network stream. Static models are represented by IDs and a tile index, where they are rendered by the client.

Avatar locations are sent from the server as 2D locations. 3D height coordinates are handled by the client side 3D physics system.

As the client will always receive historic NPC (non-player character) avatar locations, they will have their positions interpolated to predict their next position. This is done by having their current action as well as their most up-to-date location sent from the server. The client world will then use that action to predict the next location. When a new, accurate, location is received, the client will use that location with the new action for the prediction. This will result in NPCs having a near-accurate location and action representation on the client compared with the server. As all game logic is performed on the server, where accurate NPC locations are kept, the client not having 100% accurate NPC location details will not affect game play.

## Physics

2D and 3D physics are required.

### 2D Physics

Bounding box and circle collision, translation, rotation, dilation, reflection are required, along with acceleration and gravity. This will be used by the UI system and 2D particles.

### 3D Physics

Sphere, AABB (axis aligned bounding box), translation rotation, dilation, reflection, ray casting, acceleration and gravity are required.

Avatar locations are stored in 2D, even in the 3D world. The 2D X-coordinate represents the 3D X-coordinate. The 2D Y-coordinate represents the 3D Z-coordinate. The 3D Y-coordinate will be computed by the 3D physics system based on the height of the floor.

## UI

The ability to render windows and UI components is required. User events must also be supported. The UI will be rendered in screen space.

The root UI element is the canvas. All UI elements are rendered in proportion to its parent element. Sizes and locations can be specified as both a percentage of its parent or a fixed pixel size.

A special UI element called a cursor is to be supported. When a mouse like device is used, the cursor will be displayed. When a touch screen device is used, the cursor is not displayed. On a touch screen down event, the cursor's position will be set to the touch's location. UI events are based off of the cursor.

Other than the canvas, all UI elements have the following events:

* `on_focus_in`
  * When the cursor enters the element's bounds
* `on_focus_out`
  * When the cursor exits the element's bounds
* `on_click_down`
  * When the cursor is clicked down (mouse button down, tap in progress etc...)
* `on_click_up`
  * When the cursor is clicked up (mouse button up, tap finished etc...)

UI events will call a script function that is executed when the event is executed.

All UI elements have the following common properties:

* `location`
  * The location of the element in regards to its parent element. Can be either a percentage or a fixed pixel value.
* `size`
  * The size of the element in regards to its parent element. Can be either a percentage or a fixed pixel value.
* `color`
  * The color of the element. Each UI element may handle this value in a different way. 

When a UI element is in focus, it is to have a highlighted border.

Each UI element can have a corresponding CSS class. This class can set basic attributes of elements. See [here](ui_css.md) for more information on UI CSS styling.

The following UI components are required:

* Canvas
  * The root UI element. It has no renderable or intractable function. Its only purpose is to hold child UI elements.
* Container
  * Contains a set of child elements and automatically sets their locations and sizes. Elements will take the smallest possible size. If elements are outside of the bounds of the parent container, a scroll bar will be displayed and the elements can be scrolled. The following arrangements are to be supported:
    * Vertical list
      * Fits elements in a vertical list with the width of child elements set to 100%. The height of elements can be specified as a percentage of the parent container or as a fixed pixel size.
    * Horizontal list
      * Fits elements in a horizontal list with the height of child elements set to 100%. The width of elements can be specified as a percentage of the parent container or as a fixed pixel size.
    * Grid
      * Fits elements into a small container of a set size. The number of small containers is specified by a number of columns and rows. The container size can be set as a percentage of the parent container or as a fixed pixel size.
* Image
  * Displays an image of a set size. The `color` attribute tints the image with that color. The following fit settings can be chosen:
    * Fit
      * The image size is set to the same size as the image control
    * Neutral
      * The image is displayed at its natural size. The image is cropped if it is bigger than the control. If the image is smaller, the remaining space is transparent.
* Button
  * A user clickable element. Text will be overlayed on the images. The `color` attribute sets the color of the text. Supports images for the following states:
    * Neutral
    * Click down
    * Click up
* Textbox
  * Allows user to enter a single line of text. A password mode renders a `*` for each character instead of the inputted character. The `color` attribute sets the color of the text to that color.

## ECS

All entities in the game are managed by the ECS (entity component system). The client ECS acts on the data it sees. Entities that require their data updated from the server are updated by an ECS process.

The following components are required:

* `location_2d`
  * Contains the 2D location of this entity in screen space
* `location_3d`
  * contains the 3D location of this entity in the world
* `sprite`
  * Contains the data needed to render a 2D sprite. Requires a `location_2d` component.
* `mesh`
  * Contains the data needed to render a 3D mesh. If a `location_3d` component is attached, this mesh is rendered in world space. If a `location_2d` location is attached, the mesh is rendered in screen space.
* `material`
  * Contains the needed material data to render this entity, such as shader and textures.
* `rigid_body_2d`
  * Contains the needed data to handle 2D collisions, such as bounding box and circle collisions.
* `rigid_body_3d`
  * Contains the needed data to handle 3D collisions, such as sphere and AABB collisions.
* `velocity_2d`
  * Contains the needed data to impart movement to the attached `location_2d` component.
* `velocity_3d`
  * Contains the needed data to impart movement to the attached `location_3d` component.

## Scripting
Game logic and related scripting is handled on the server. Scripting will be used primarily by the UI system to handle UI events.

Any scripting on the client must not be able to affect game play data on the server.
