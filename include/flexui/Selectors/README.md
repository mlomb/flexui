# Selectors

Selectors let you target `Element` nodes in the tree structure.

The selectors used by flexui are very similar to CSS selectors, but remember that flexui does not (and have no intention to) adhere with the [CSS spec](https://www.w3.org/TR/selectors-3/).

This is an overview of how Selectors are implemented ([Selector.hpp](Selector.hpp)):

A `Selector` is a list of `SelectorPart`s. Each `SelectorPart` consists of:
1. A `SelectorIdentifier` which holds the name of the identifier (a string) and its type (`ID`, `TAG` or `CLASS`).
2. A `SelectorRelationship` which determinates the relationship with the previous `SelectorPart` in the list.  
Supported relationships are:
    | SelectorRelationship | CSS symbol | Description |
    | - | - | - |
    | `NONE` | _(no space)_ | same target (ie. tag<b>.class</b> ) |
    | `DESCENDANT` | _(space)_ | any element in the subtree |
    | `CHILD` | > | only inmmediate child elements |
    | `ADJACENT_SIBLING` | + | matches the second element only if it immediately follows the first |
    | `GENERAL_SIBLING` | ~ | all iterations of the second element that are followed by the first |
3. A `SelectorPseudoState` that stores pseudo-states as a bitfield so they can be combined (ie. button:active:hover).  
Pseudo-states are predefined and only the following are supported:
    | SelectorPseudoState | CSS syntax |
    | - | - |
    | `NONE` | N/A |
    | `HOVER` | :hover |
    | `DISABLED` | :disabled |
    | `CHECKED` | :checked |
    | `ACTIVE` | :active |
    | `FOCUS` | :focus |


Remarks:
* No support for attribute selectors.
* No support for arbitrary pseudo-classes or pseudo-elements.
* No support for complex pseudo-classes selectors like `nth-child`.

### Example

Let's consider the following selector:

```css
#container .buttons > button.fancy + test:hover
```

The `Selector` generated will be a list of the following `SelectorPart`s:

```
identifer="container" (ID)  prev_rel=NONE             pseudos=00000000
identifer="buttons" (CLASS) prev_rel=DESCENDANT       pseudos=00000000
identifer="button" (TAG)    prev_rel=CHILD            pseudos=00000000
identifer="fancy" (CLASS)   prev_rel=NONE             pseudos=00000000
identifer="test" (TAG)      prev_rel=ADJACENT_SIBLING pseudos=00000001
```

## Selector Matching

`Element::matchesSelector`

`ContainerNode::querySelector` or `ContainerNode::querySelectorAll`

How matching works:

