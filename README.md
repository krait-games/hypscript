# HypScript Programming Language

HypScript is a programming language that is designed to be easy to embed in a C++ application. It is statically typed, and has a syntax that is similar to JavaScript. HypScript was originally designed to be used in the [Hyperion](https://github.com/krait-games/hyperion-engine) game engine.

Here's an example of a simple HypScript class:

```
class TestScript
{
    scene: Scene = null;
    entity: uint = ~0u;

    func on_added(scene: Scene, entity: uint) {
        self.scene = scene;
        self.entity = entity;
    }

    func on_removed() {
        self.scene = null;
        self.entity = ~0u;
    }

    func on_updated(delta: float) {
        self.scene.set_entity_position(self.entity, self.scene.get_entity_position(self.entity) + vec3(0.0, 1.0, 0.0) * delta);
    }
}
```

## Embedding in C++

### Global Functions

Bindings can be created by subclassing the `ScriptBindingsBase` base class. We've provided a few implementations that are core to the language under the src/script/bindings directory. Here's an example of how to add a global function to the be usable from scripts:

```cpp
// Include hypscript headers
#include <hypscript/script/ScriptApi.hpp>
#include <hypscript/script/ScriptBindingDef.generated.hpp>

#include <stdio.h>

// Usually used when binding a C++ class,
// but here we'll just stub it out
class HelloPrinterClassStub { };

static struct HelloPrinterBindings : ScriptBindingsBase
{
    HelloPrinterBindings()
        : ScriptBindingsBase(TypeID::ForType<HelloPrinterClassStub>())
    {
    }

    virtual void Generate(scriptapi2::Context &context) override
    {   
        context.Global("say_hello", "function< void >", CxxFn<
            void,
            []() -> void
            {
                printf("Hello, world!\n");
            }
        >);
    }
} s_hello_printer_bindings; 
```

### Classes

Classes can also be bound by subclassing the `ScriptBindingsBase` base class. Here's an example of how to bind a class to the scripting environment:

```cpp

// Same includes as before ... 

static struct SceneScriptBindings : ScriptBindingsBase
{
    SceneScriptBindings()
        : ScriptBindingsBase(TypeID::ForType<Scene>())
    {
    }

    virtual void Generate(scriptapi2::Context &context) override
    {
        context.Class<Scene>("Scene")
            // your constructor, called when a new instance of the class is created
            .Method("$construct", "function< Scene >", CxxCtor< Scene >)
            .Method("get_id", "function< uint, Scene >", CxxMemberFn< UInt32, Scene, &Scene::GetID > >)
            .Build();

    }

} s_scene_script_bindings { };
```

You can now use your `Scene` class in your HypScript code:

```
var scene: Scene = Scene();     // calls your Scene::Scene constructor
var id: uint = scene.get_id();  // calls your Scene::GetID member function

Logger.log(Logger.INFO, "The scene's id is {0}", id);
```