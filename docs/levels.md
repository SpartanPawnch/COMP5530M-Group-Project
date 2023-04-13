# Level Files
Level files are json files with the following structure:

```json:
{
    //assets
    "audio":[
        {"uuid":"<id0>","path":"<path0>"},
        {"uuid":"<id1>","path":"<path1>"},
        ...
        ],
    "models":[
        {"uuid":"<id0>","path":"<path0>"},
        {"id":"<id1>","path":"<path1>"},
        ...
        ],
    "textures":[
        {"uuid":"<id0>","path":"<path0>"},
        {"uuid":"<id1>","path":"<path1>"},
        ...
        ],
    "scripts":[
        {"uuid":"<id0>","path":"<path0>"},
        {"uuid":"<id1>","path":"<path1>"},
        ...
        ],
    "misc":[
        {"uuid":"<id0>","path":"<path0>"},
        {"uuid":"<id1>","path":"<path1>"},
        ...
        ],

    //TODO - custom components?/object presets?
    
    //ECS contents
    "entities":[
        {
            "name":"<name0>",
            "id":"<id0>",
            "parent":"<parentIdx>"
            //TODO - innate entity subclass properties?
            "components":[
                {
                    "type":"<subclass>",
                    //shared properties
                    "uuid":"<uuid>",
                    "name":"<name>",
                    //subclasses specifics:
                    //TransformComponent
                    "position":[],//vec3
                    "rotation":[],//vec4
                    "scale":[]//vec3,
                }
            ]
        },
    ]
}
```
