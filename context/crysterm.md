# Crysterm Agent Usage Guide

Crysterm is a **Crystal terminal UI toolkit** for building full-screen console applications using a **screen + widget tree** model.

Use it when you want:

- full-screen terminal interfaces
- nested widgets and layouts
- keyboard and mouse interaction
- styled text, borders, shadows, and tags
- resize-aware rendering
- app-like event handling

---

## 1) Mental model

Crysterm is organized around three main ideas:

1. **`Screen`** is the top-level terminal surface
2. **`Widget`** objects are the UI building blocks
3. **Events** drive interaction and lifecycle

A typical app flow is:

1. create a `Screen`
2. create widgets and attach them to the screen or other widgets
3. register event handlers
4. call `screen.render`
5. call `screen.exec` to run the app
6. call `screen.destroy` when quitting

---

## 2) Minimal safe app pattern

This is the core pattern agents should follow:

```crystal
require "crysterm"

alias C = Crysterm

screen = C::Screen.new title: "My App"

box = C::Widget::Box.new(
  parent: screen,
  top: "center",
  left: "center",
  width: 20,
  height: 5,
  content: "{center}{bold}Hello{/bold}{/center}",
  parse_tags: true,
  style: C::Style.new(fg: "yellow", bg: "blue", border: true),
)

screen.on(C::Event::KeyPress) do |e|
  if e.char == 'q' || e.key == Tput::Key::CtrlQ
    screen.destroy
    exit
  end
end

screen.render
screen.exec
```

### Key usage rule

- Use `screen.exec` to start the interactive loop.
- Use `screen.destroy` to stop the app and clean up.

---

## 3) The main objects

## `Screen`

`Screen` is the root object.

It represents the terminal display and owns the event/render lifecycle.

Common responsibilities:

- input/output IO
- render cycle
- resize handling
- destroying the UI
- global app-level events

Example:

```crystal
screen = Crysterm::Screen.new title: "Demo"
screen.render
screen.exec
```

## `Widget`

`Widget` is the base class for all UI components.

You usually don’t instantiate `Widget` directly; instead use a concrete widget like:

- `Widget::Box`
- `Widget::Button`
- `Widget::Label`
- `Widget::Layout`
- `Widget::List`
- `Widget::TextArea`
- `Widget::TextBox`
- `Widget::ProgressBar`
- `Widget::Loading`
- `Widget::Question`
- `Widget::RadioButton`
- `Widget::CheckBox`

Widgets are attached to a parent using `parent:`.

Example:

```crystal
box = Crysterm::Widget::Box.new(
  parent: screen,
  content: "Hello",
  width: 20,
  height: 5
)
```

---

## 4) Attaching widgets

You can attach widgets to:

- the `Screen` directly
- another widget, for nested layouts

### Screen-level widget

```crystal
button = Crysterm::Widget::Button.new(
  parent: screen,
  content: "Click me",
  left: 10,
  top: 5,
  width: 20,
  height: 3
)
```

### Nested widget

```crystal
panel = Crysterm::Widget::Box.new(parent: screen, width: 50, height: 20)

label = Crysterm::Widget::Label.new(
  parent: panel,
  content: "Inside the panel",
  top: 1,
  left: 2
)
```

### Rule of thumb

- Use `parent: screen` for top-level content.
- Use `parent: other_widget` for nested UI.

---

## 5) Positioning and sizing

Crysterm uses Blessed-style flexible positioning.

You can set:

- `left`
- `top`
- `right`
- `bottom`
- `width`
- `height`

These accept multiple formats:

- integers
- percentages like `"50%"`
- centered values like `"center"`
- expressions like `"50%+-10"`

### Examples

```crystal
top: "center"
left: "center"
width: 20
height: 5
```

```crystal
width: "100%"
height: "100%-2"
```

### Important usage note

If a widget has borders or padding, you often need to subtract that space manually.

Example:

```crystal
width: "100%-2"
height: "100%-2"
```

This is common for bordered child widgets.

---

## 6) Styling widgets

Widgets use a `Style` object.

Example:

```crystal
style: Crysterm::Style.new(
  fg: "yellow",
  bg: "blue",
  border: true
)
```

Styles can control:

- foreground color
- background color
- borders
- shadows
- transparency / alpha-like effects
- state styling such as focus, hover, selected

### Common style examples

```crystal
style: C::Style.new(fg: "white", bg: "black", border: true)
style: C::Style.new(fg: "#ffcc00", bg: "#202020")
style: C::Style.new(border: C::Border.new(type: C::BorderType::Line))
```

### Tags in content strings

Crysterm supports Blessed-style tags inside text content.

Examples:

```crystal
content: "{center}{bold}Hello{/bold}{/center}"
content: "{light-blue-fg}Styled text{/light-blue-fg}"
content: "{#ff8800-fg}RGB color{/}"
```

Useful tags include:

- alignment: `{center}`, `{left}`, `{right}`
- attributes: `{bold}`, `{underline}`, `{blink}`, `{inverse}`, `{invisible}`
- colors: named colors, RGB hex, and numeric palette indices
- closing tags: `{/}` or explicit closing tags like `{/bold}`

### Parsing tags

Set:

```crystal
parse_tags: true
```

This is usually the default and should be left on if your content contains tags.

---

## 7) Rendering lifecycle

### `screen.render`

This updates internal rendered state and draws the current UI.

Use it:

- after creating widgets
- after changing widget position/content/style
- after state changes in the app

Example:

```crystal
screen.render
```

### `screen.exec`

This starts the app loop.

```crystal
screen.exec
```

Think of it as “run the UI”.

### `screen.destroy`

This tears down the screen and exits cleanly.

```crystal
screen.destroy
```

Use this when quitting from events or when you want to end the app.

---

## 8) Events and interaction

Crysterm uses event handlers.

Typical usage:

```crystal
screen.on(Crysterm::Event::KeyPress) do |e|
  # handle input
end
```

### Common event types

From the codebase, useful events include:

- `Crysterm::Event::KeyPress`
- `Crysterm::Event::Resize`
- `Crysterm::Event::Attach`
- `Crysterm::Event::Detach`
- `Crysterm::Event::Destroy`
- `Crysterm::Event::Focus`
- `Crysterm::Event::Blur`
- `Crysterm::Event::Click`
- `Crysterm::Event::Press`
- `Crysterm::Event::Submit`
- `Crysterm::Event::Cancel`
- `Crysterm::Event::Scroll`
- `Crysterm::Event::Move`
- `Crysterm::Event::Rendered`
- `Crysterm::Event::PreRender`

### KeyPress usage

The common pattern is:

```crystal
screen.on(Crysterm::Event::KeyPress) do |e|
  if e.char == 'q' || e.key == Tput::Key::CtrlQ
    screen.destroy
    exit
  end
end
```

### Useful properties on `KeyPress`

```crystal
e.char
e.key
e.sequence
e.accepted?
e.accept
e.ignore
```

### Resize usage

```crystal
screen.on(Crysterm::Event::Resize) do |e|
  screen.render
end
```

---

## 9) Common widgets and how to use them

## `Widget::Box`

Useful for:

- containers
- framed panels
- text blocks
- layout regions

```crystal
box = C::Widget::Box.new(
  parent: screen,
  top: 2,
  left: 2,
  width: 30,
  height: 10,
  content: "Panel",
  style: C::Style.new(border: true)
)
```

## `Widget::Button`

Useful for clickable actions.

```crystal
button = C::Widget::Button.new(
  parent: screen,
  content: "Click me",
  width: 20,
  height: 3,
  style: C::Style.new(border: true)
)
```

## `Widget::Layout`

Useful for arranging children automatically.

There are grid and inline/masonry-like layouts.

```crystal
layout = C::Widget::Layout.new(
  parent: screen,
  top: 0,
  left: 0,
  width: "100%",
  height: "100%",
  layout: C::LayoutType::Grid
)
```

### Important note

`Layout` generally needs explicit width and height.

## `Widget::TextArea` and `Widget::TextBox`

Useful for input.

- `TextBox` is for one-line input
- `TextArea` is for multiline input

## `Widget::ProgressBar`

Useful for progress indicators.

```crystal
progress = C::Widget::ProgressBar.new(
  parent: screen,
  width: 40,
  height: 3,
  filled: 50,
  style: C::Style.new(border: true)
)
```

## `Widget::List` / `Widget::ListTable`

Useful for menus and lists of items.

## `Widget::Loading`

Useful for spinner/loading UIs.

## `Widget::Question`

Useful for yes/no or prompt-style interactions.

---

## 10) Focus and input behavior

Widgets can be focused:

```crystal
widget.focus
```

Useful when building interactive forms or menus.

Some widgets support:

- `focus_on_click`
- keyboard input
- scroll behavior
- mouse interactions

For input-heavy UIs, a common flow is:

1. create the input widget
2. call `focus`
3. handle submit/cancel/key events
4. render changes

---

## 11) Layout and alignment patterns

### Centered widget

```crystal
box = C::Widget::Box.new(
  parent: screen,
  top: "center",
  left: "center",
  width: 20,
  height: 5,
  content: "Centered"
)
```

### Full-screen layout

```crystal
layout = C::Widget::Layout.new(
  parent: screen,
  top: 0,
  left: 0,
  width: "100%",
  height: "100%"
)
```

### Nested content with borders

```crystal
panel = C::Widget::Box.new(
  parent: screen,
  width: "100%-2",
  height: "100%-2",
  style: C::Style.new(border: true)
)
```

### Practical rule

When using borders, account for the border thickness in width/height calculations.

---

## 12) Text tags and content formatting

Crysterm content strings can embed formatting tags.

### Example

```crystal
content: "{center}Hello {bold}world{/bold}!{/center}"
```

### Useful patterns

- `{center}` to center text
- `{bold}` for emphasis
- `{light-blue-fg}` for named foreground colors
- `{#ff00aa-fg}` for RGB colors
- `{/}` to close all open tags

### When to use tags

Use tags when the text itself is the simplest way to express formatting.

Use `Style` when styling should apply to the whole widget.

---

## 13) Screen-level usage pattern

A practical app often looks like this:

```crystal
require "crysterm"

alias C = Crysterm

screen = C::Screen.new title: "Dashboard"

layout = C::Widget::Layout.new(
  parent: screen,
  top: 0,
  left: 0,
  width: "100%",
  height: "100%",
  layout: C::LayoutType::Grid
)

header = C::Widget::Box.new(
  parent: layout,
  width: "100%",
  height: 3,
  content: "{center}{bold}Dashboard{/bold}{/center}",
  parse_tags: true,
  style: C::Style.new(border: true)
)

status = C::Widget::ProgressBar.new(
  parent: layout,
  width: "100%",
  height: 3,
  filled: 35,
  style: C::Style.new(border: true)
)

screen.on(C::Event::KeyPress) do |e|
  if e.char == 'q' || e.key == Tput::Key::CtrlQ
    screen.destroy
    exit
  end
end

screen.render
screen.exec
```

---

## 14) Common agent mistakes

Avoid these:

### 1. Forgetting to call `screen.exec`

Creating widgets is not enough; `exec` starts the app loop.

### 2. Forgetting `screen.destroy`

Use it to exit cleanly.

### 3. Not attaching widgets to a parent

A widget needs a `parent:` to become part of the UI tree.

### 4. Ignoring borders when sizing child widgets

Use `"100%-2"` or similar when needed.

### 5. Treating `render` as optional

If widget state changes, call `render`.

### 6. Not handling `Resize`

Many layouts need a redraw after screen size changes.

### 7. Assuming `Screen` is a widget

It is not; it is the top-level surface.

---

## 15) What to trust when unsure

If usage is unclear, trust sources in this order:

1. `examples/`
2. `README.md`
3. `src/`
4. `documentation/`

For practical usage, the best examples are:

- `examples/hello.cr`
- `examples/hello2.cr`
- `examples/tech-demo.cr`

---

## 16) Quick recipes

### Hello box

```crystal
box = C::Widget::Box.new(
  parent: screen,
  top: "center",
  left: "center",
  width: 20,
  height: 5,
  content: "{center}Hello{/center}",
  style: C::Style.new(border: true)
)
```

### Quit on q or Ctrl-Q

```crystal
screen.on(C::Event::KeyPress) do |e|
  if e.char == 'q' || e.key == Tput::Key::CtrlQ
    screen.destroy
    exit
  end
end
```

### Resize redraw

```crystal
screen.on(C::Event::Resize) do |_|
  screen.render
end
```

### Centered layout

```crystal
layout = C::Widget::Layout.new(
  parent: screen,
  top: "center",
  left: "center",
  width: 40,
  height: 10
)
```

### Text with tags

```crystal
content: "{center}{bold}{light-green-fg}Success{/light-green-fg}{/bold}{/center}"
```

---

## Sources

- [README.md](https://github.com/crystallabs/crysterm/blob/master/README.md)
- [src/crysterm.cr](https://github.com/crystallabs/crysterm/blob/master/src/crysterm.cr)
- [src/screen.cr](https://github.com/crystallabs/crysterm/blob/master/src/screen.cr)
- [src/widget.cr](https://github.com/crystallabs/crysterm/blob/master/src/widget.cr)
- [src/event.cr](https://github.com/crystallabs/crysterm/blob/master/src/event.cr)
- [documentation/README.md](https://github.com/crystallabs/crysterm/blob/master/documentation/README.md)
- [documentation/positioning.md](https://github.com/crystallabs/crysterm/blob/master/documentation/positioning.md)
- [examples/hello.cr](https://github.com/crystallabs/crysterm/blob/master/examples/hello.cr)
- [examples/tech-demo.cr](https://github.com/crystallabs/crysterm/blob/master/examples/tech-demo.cr)

If you want, I can also turn this into a **shorter agent quick-start** version or a **repo-specific “how to build a Crysterm app” playbook**.
