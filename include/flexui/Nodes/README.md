# Nodes

This section describes how flexui structures the UI tree.

Note: The abstraction built here takes inspiration from the [blink implementation](https://github.com/chromium/chromium/tree/master/third_party/blink/renderer/core/dom) of the [DOM spec](https://dom.spec.whatwg.org/). Note that by no means this code tries to comply the DOM spec or even be close to it.

There are four node types:
* Document: root for all other nodes
* Comment: represents comments found during XML parsing
* Element: base for all other elements
* Text: text node, only contains text
* Shadow: ---

Inherithance graph for the Node class:

```
Node
├──ContainerNode
│  ├──Document
│  └──Element
│     ├──Button
│     ├──Slider
│     └──...
├──Text
└──Shadow
```
