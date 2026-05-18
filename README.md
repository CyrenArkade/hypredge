# hypredge
KDE's edge effects for Hyprland.

### Functions
`hl.plugin.hypredge.edge_effect(edge, dispatcher)`:  
Triggers a dispatcher when the cursor enters a region of a monitor.
Valid edges are `top`, `bottom`, `left`, `right`, `topleft`, `topright`, `bottomleft`, `bottomright`.  
Valid dispatchers include `hl.dsp...` as well as lua functions.

`hl.plugin.hypredge.move_cursor_to_edge(edge)`:  
Moves the cursor to that edge of the screen, not affecting the other axis.  
Valid edges are `top`, `bottom`, `left`, `right`.  
This will be made into a dispatcher once [this issue](https://github.com/hyprwm/Hyprland/discussions/14451) is fixed.

### Window Rule
Adds the effect `hypredge_ignore_constraints [on|off]`.  
By default, hypredge will not trigger edge effects if the mouse is constrained. This overrides that, allowing edge effects to always trigger.

### Example config
```lua
plugin {
    hypredge {
        # Prevents edge effects from activating when within this
        # distance of corners, making corner triggers easier to hit.
        corner_barrier = 100 # default

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

windowrule = match:title FINAL FANTASY XIV, hypredge:ignore_constraints on

if hl.plugin.hypredge ~= nil then

  hl.config({
    plugin = {
        hypredge = {
            corner_barrier = 100 -- default
        }
    }
  })

  -- This emulates workspace switching by screen edge, like in KDE.
  hl.plugin.hypredge.edge_effect("left", function()
      hl.dispatch(hl.dsp.focus({ workspace = "e-1" }))
      hl.plugin.hypredge.move_cursor_to_edge("right")
  end)
  hl.plugin.hypredge.edge_effect("right", function()
      hl.dispatch(hl.dsp.focus({ workspace = "e+1" }))
      hl.plugin.hypredge.move_cursor_to_edge("left")
  end)

  -- You can use any dispatchers you want!
  hl.plugin.hypredge.edge_effect("right", hl.dsp.exec_cmd("kitty"))

  -- Corners are also trigger areas!
  hl.plugin.hypredge.edge_effect("topright", hl.dsp.exec_cmd("firefox"))

  -- This lets dispatchers trigger when FFXIV (an application that constrains the mouse) is active.
  hl.window_rule({
      match = {class = "kitty"},
      hypredge_ignore_constraints = true,
  })
end
```