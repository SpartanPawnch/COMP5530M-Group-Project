# Level Files
Level files are json files with the following structure:

```json:
{
    //assets
    "audio":[
        {"id":"<id0>","path":"<path0>"},
        {"id":"<id1>","path":"<path1>"},
        ...
        ],
    "models":[
        {"id":"<id0>","path":"<path0>"},
        {"id":"<id1>","path":"<path1>"},
        ...
        ],
    "textures":[
        {"id":"<id0>","path":"<path0>"},
        {"id":"<id1>","path":"<path1>"},
        ...
        ],
    "scripts":[
        {"id":"<id0>","path":"<path0>"},
        {"id":"<id1>","path":"<path1>"},
        ...
        ],
    "misc":[
        {"id":"<id0>","path":"<path0>"},
        {"id":"<id1>","path":"<path1>"},
        ...
        ],

    //TODO - custom components?/object presets?
    
    //ECS contents
    "entities":[
        {
            "name":"<name0>",
            "id":"<id0>",
            //TODO - innate object properties
            "components":[
                {
                //TODO component structure
                }
            ]
        },
    ]
}
```