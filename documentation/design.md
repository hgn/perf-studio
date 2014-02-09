# Measurement Classes

Perf-Studio modules requires measurement data. Normally this is perf recorded
data, but it can be other data as well (oprofile(1), time(1)). Modules do not
directly start measurement applications like perf(1) itself, rather they
register the required data set at the perf-studio core. Perf-studio core will
inform each activated module if new measurement data is available
(```module->update()``` is called).

Perf-Studio core API is flexible enough to handle different kind of measurements:

- measurement program executed by fork/exec. Output is recorded and provided for the module.
  E.g. ```/usr/bin/time df 1>/dev/null```
- measurement program executed by fork/exec and nothing is recorded. The module
	knows how to find the output (e.g module parse tracefile from oprofile directly)
- measurement program is executed and a path to the output file is returned. This is the
  current implemented mode for perf(1), by providing a path to perf.data if the data is
	recorded.
- Beside this perf-studio provides build-in functionality which is provided directly.
  For example simple time measurement is done this way.

The following code illustrate all currently supported measurement classes:

```
enum {
    CLASS_EXEC_RAW,
    CLASS_EXEC_RAW_STDOUT_STDERR_CAPTURE,
    CLASS_EXEC_TIME_MEASUREMENT,
    CLASS_EXEC_PERF_RECORD
};
```

Each module register there required data and some measurement classes requires
more detailed data. The ```CLASS_EXEC_PERF_RECORD``` class requires exact
knowledge of recorded events (-e flags).

Each module can register multiple measurement classes - but not the same class
multiple times!

## Interpret Measurement Data

Depending on the measurement class the analyzed data must be more or less
complex processed. ```CLASS_EXEC_TIME_MEASUREMENT``` data is provided by a
```struct``` in update and nothing must be done. perf recorded data provides
just a path argument pointing to the raw ```perf.data``` file. This sounds a
little bit awkward but provides the most flexible API and possibilities.
Furthermore, perf-studio core provides helper functionality to parse and format
perf recorded data. In further versions perf-core may itself provide more
prepared data. New measurement classes are introduced to provide a migration
strategy and do not break the API.


## Registering versus Direct Measurement

The dominating component is the perf-core (especially the project). Advantage
is that perf-studio knows what recorded measurement data belongs to what
analyzed binary. If a binary is new compiled then perf-studio knows this and
subsequently knows that the measurement data is outdated.

Another advantage of this architecture is a measurement can be provided for two
or more modules if they share the required data. Measurement data can be reused
by different modules.


## Perspective: Libperf/Libtrace Integration

Later when libperf/libtrace is utilized directly by perf-studio (compared to
currently used fork/exec(perf) approach) a new measurement class is added. This
enabled a easy migration path to add new features to perf-studio.



## Relationship Measurement Classes and Projects

If modules are started/loaded and activated (happends automatically) the
concret measurement classes are registered at the currently loaded project. If
no project is loaded modules are not registered.  If project unloaded all
measurment classes are automatically unloaded as well.

Measurement Classes are registered to the project to provide the project the
control of the measurement data. Advantages are when several modules share the
same Measurement Classes. For example if two modules required cache line miss
data then the project will run one measurement for both modules.  Another
advantage is that the project know when data is outdated (md5 missmatch of
executable)

![Module and Project Interaction](https://dl.dropboxusercontent.com/s/d0ghb805klc4quk/project-module-register.png)

If modules are deactivated (insensitive) then Measurement Classes are
deregistered at the project. If later the Module is reenabled the Measurement
Classes are registered again.


called by a module this graps over all classes by the module and register at a
own list at

```
project_register_module(ps, module, class_list)
```

called by module at deregister/unsensitive time

```
project_deregister_module_classes(ps, module)
{
    # iterate over list and unlink all classes
    # where owner module
}
```

Called when project is unloaded when project is unloaded also all modules are
clossed


```
project_deregister_all_classes(ps)
{
    # simple loop over classes and unlink
}
```




    register();



Implement "project unload" to test further event
registration/deregistration.


    update(struct module *m, gslit *)

