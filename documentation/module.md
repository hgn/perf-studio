# Perf Studio Modules

## Introduction

## Writing Modules


## Module Callbacks

There is a DIFFERENCE between `activate/deactivate` and `enable/disable`. Where
the former functions are used to bring up the MODULE itself like creating
widgets, creating fundemantal objects, is the later used to temporary DISABLE
and reenable the MODULE. Enable will call activate will call enable, so that at
the beginning no additional button must be pushed to work properly.

If a project is activated - `project->mc_unit` must be false

If a MODULE is activated, the MODULE call `project_register_mc_store()` to
inform the project (and thus perf-studio) that (possible) new EVENTS should be
analyzed.

A modules enable() is called when: a) the MODULE is loaded, b) was disabled and
is now enabled again.

If a project is unloaded every connected MODULE (through mc_store) is informed
that the project is unloaded in near future. The module MUST release is
mc_store from the project via `project_register_mc_store()`

A module call `project_register_mc_store()` at actication time to register the
mc_store for the MODULE.

If a MODULE is disabled (`MODULE->disable()`) the MODULE must call
`project_unregister_mc_store()` if the MODULE has REGISTERED a mc_store at the
current project.

If a MODULE is already enabled (enabled == TRUE) and a new project is activated
the MODULE should be informed and should register at the project. The MODULE
functions are `MODULE->project_loaded()/MODULE->project_unloading()` callbacks
are called.
