# hypredge
KDE's edge effects for Hyprland.

### Keywords
Adds commands of the form `plugin:hypredge:edge-effect = edge, dispatcher, dispatcher_args`.  
Valid edges are `top`, `bottom`, `left`, `right`, `topleft`, `topright`, `bottomleft`, `bottomright`.

### Dispatchers
Adds the dispatcher `hypredge:movecursortoedge`.  
Valid edges are `top`, `bottom`, `left`, `right`.

### Example config
```
plugin {
    hypredge {
        # Prevents edge effects from activating when within this
        # distance of corners, making corner triggers easier to hit.
        # default: 100
        corner_barrier = 100

        # This emulates workspace switching by screen edge, like in KDE.
        edge-effect = left, workspace, e-1
        edge-effect = left, hypredge:movecursortoedge, right
        edge-effect = right, workspace, e+1
        edge-effect = right, hypredge:movecursortoedge, left

        # You can use any dispatchers you want!
        edge-effect = top, exec, kitty

        # Corners are also trigger areas!
        edge-effect = topright, exec, dolphin
    }
}
```