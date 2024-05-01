# "using graphs"

**Skill-level:** high

**minimum version:** 2023.9.0

To use graphs on your display you have to add a graph to your yaml, only this graph can be displayed. Depending on your values you should set the loglevel of the graphs component to error. The Color has to be defined in the YAML.

## Example

```yaml
logger:
  level: INFO
  logs:
    graph: error

color:
  - id: graph_color
    red: 100%
    green: 20%
    blue: 25%

graph:
  - id: ehmtx_graph
    width: 32
    height: 8
    traces:
      - sensor: light_sensor
        line_thickness:  1
        color: graph_color  
    
ehmtxv2:
  id: rgb8x32
  display_graph: ehmtx_graph
  ...
```

The service call is:  

```c
  //void graph_screen(int lifetime , int screen_time);
    id(rgb8x32).graph_screen(10,20);
```


