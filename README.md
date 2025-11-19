# hypredge
KDE's edge effects for Hyprland.

### Keywords
Adds commands of the form `plugin:hypredge:edge_effect = edge, dispatcher, dispatcher_args`.  
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
        corner_barrier = 100 # default

        # Respects the focused application's mouse constraints.
        respect_constraints = true # default

        # This emulates workspace switching by screen edge, like in KDE.
        edge_effect = left, workspace, e-1
        edge_effect = left, hypredge:movecursortoedge, right
        edge_effect = right, workspace, e+1
        edge_effect = right, hypredge:movecursortoedge, left

        # You can use any dispatchers you want!
        edge_effect = top, exec, kitty

        # Corners are also trigger areas!
        edge_effect = topright, exec, dolphin
    }
}
```