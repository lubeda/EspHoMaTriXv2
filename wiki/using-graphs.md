# "using graphs"

**Skill-level:** high

**minimum version:** 2023.10.0

To use graphs on your display you have to add a graph to your yaml, only this graph can be displayed

```yaml
logger:
  level: DEBUG
  logs:
    graph: error

graph:
  - id: ehmtx_graph
    sensor: light_sensor
    duration: 15min
    border: false
    line_thickness:  1
    width: 32
    height: 8

ehmtxv2:
  id: rgb8x32
  display_graph: ehmtx_graph
  ...
```

The service call is:  

```c
    id(rgb8x32).graph_screen(10,20,200,130,170);
```


