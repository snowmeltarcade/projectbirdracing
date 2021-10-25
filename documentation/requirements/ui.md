# UI

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

UIs can be built by UI definition files, scripts and the game code itself. Where possible, UI definition files and scripts should be used as this provides the most flexibility.

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