# Style

## Property

Properties 

### Data types

| Name | Note | Internal data type |
| - | - | - |
| string | Text | std::string* |
| number | A number | float |
| length | Contains a number with a unit type (`PX`, `IN`, etc) | StyleLength (float + enum) |
| color | All colors are converted to this format | Color (`uint32_t` as AARRGGBB) |
| `<enum>` | Any property that has a fixed set of values | < `uint32_t` |

All values are stored in an union inside the `StyleValue` struct.

### Supported properties

| Name | Data type |
| - | - |
| | |

For now you can read `StylePropertyID` inside [StyleProperty.hpp](StyleProperty.hpp).

### Remarks

* Custom style properties are not supported
* Property names must be lowercase (they are case-sensitive)

## StyleRule

## Definitions

* StyleSheet: 

## Stylesheets

Loading 


Only `Element` nodes are affected by styles and retain computed styles. `Text` nodes get their information from their parent element.
