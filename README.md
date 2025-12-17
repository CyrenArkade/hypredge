# hypredge
KDE's edge effects for Hyprland.

### Keywords
Adds commands of the form `plugin:hypredge:edge_effect = edge, dispatcher, dispatcher_args`.  
Triggers a dispatcher when the cursor enters a region of a monitor.
Valid edges are `top`, `bottom`, `left`, `right`, `topleft`, `topright`, `bottomleft`, `bottomright`.

### Dispatchers
Adds the dispatcher `hypredge:movecursortoedge [top|bottom|left|right]`.  
Moves cursor to that edge of the screen, not affecting the other axis.

### Window Rule
Adds the effect `hypredge:ignore_constraints [on|off]`.  
By default, hypredge will not trigger edge effects if the mouse is constrained. This overrides that, allowing edge effects to always trigger.

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

# This lets dispatchers trigger when FFXIV is active.
windowrule = match:title FINAL FANTASY XIV, hypredge:ignore_constraints on
```