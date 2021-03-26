# flexui

Welcome to the flexui source code!

Overview of the folder structure:

* `flexui`
  * `Elements`: basic components
  * `Nodes`: structure
  * `Style`: styling

## Passes

1. Style pass: Compute all styles, make sure every `ComputeStyle` is up to date
2. Layout pass: performs the layout calculations (runs the Yoga engine) and updates the rects in the LayoutObjects
3. Render pass: Generate the final indices and verticies

## AAA
