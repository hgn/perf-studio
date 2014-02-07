# Measurement Classes

 nicht nur events, sondern ein vielzahl von programmen, etc. pp
 dies inkludiert das starten von programmen und das collected
 der ausgabe, das starten eines programmes und das abwarten
 bis auf das ende (dann kann eine output datei von hand geparsed werden)
 das einfache zeit messen
 Grunds=E4tzlich kann man sagen es gibt grunds=E4tzlich zwei gro=DFe=20

- third party programm starten - output parsen und standardisiert zu=20
verf=FCgung stellen
- third party programm starten - und nichts machen (module wei=DF wo date=
n=20
liegen (/tmp/foo.log) und wei=DF wie diese zu verarbeiten sind
- third party programm starten - pointer auf datei wird zur=FCckgegeben=20
und das format ist bekannt und kann weiter verarbeitet werden (parser=20
stehen zu verf=FCgung)
- kein third party programm wird gestartet aber daten werden gesammelt,=20
diese werden standadisiert zur verf=FCgung gestellt

    enum {
        CLASS=5FEXEC=5FRAW,
        CLASS=5FEXEC=5FRAW=5FSTDOUT=5FSTDERR=5FCAPTURE,
        CLASS=5FEXEC=5FTIME=5FMEASUREMENT,
        CLASS=5FEXEC=5FPERF=5FRECORD
    };

# Ein Module kann sich an mehr als einen "event" registieren, ein update() =

liefert also mehr als ein ergebniss,
# Ein Module darf aber sich aber nicht mehr als einmal mit einer class=20
anmelden. Zudem macht
# es oft keinen Sinn das ein Module mehrere Measurement Classes=20
unterst=FCtzt/einfordert. Dies liegt
# einfach daran das viele Measurement Classes einen neuen durchlauf=20
erfordern und somit einen neuen Testlauf
# erfordern.
#
# Perspective: Libperf/Libtrace Integration
#
# Later when libperf/libtrace is utilized directly by perf-studio=20
(compared to currently used fork/exec(perf) approach)
# new measurement class is added. This enabled a easy migration path to=20
add new features to perf-studio.



## Relationship Measurement Classes and Projects

If modules are started/loaded and activated (happends automatically) the
concret measurement classes are registered at the currently loaded project. If
no project is loaded modules are not registered.  If project unloaded all
measurment classes are automatically unloaded as well.

# Measurement Classes are registered to the project to provide the project =

the control of the measurement
# data. Advantages are when several modules share the same Measurement=20
Classes. For example if
# two modules required cache line miss data then the project will run one=20
measurement for both
# modules.
# Another advantage is that the project know when data is outdated (md5=20
missmatch of executable)

# If modules are deactivated (insensitive) then Measurement Classes
# are deregistered at the project. If later the Module is reenabled the=20
Measurement Classes
# are registered again.


# called by a module
# this graps over all classes by the module and register at
# a own list at=20
project_register_module(ps, module, class_list)


# called by module at deregister/unsensitive time
project=5Fderegister=5Fmodule=5Fclasses(ps, module)
{
        # iterate over list and unlink all classes
        # where owner =3D=3D module
}

# called when project is unloaded
# when project is unloaded also all modules are clossed
project=5Fderegister=5Fall=5Fclasses(ps)
{
        # simple loop over classes and unlink
}




register();



# Implement "project unload" to test further event=20
registration/deregistration.


update(struct module *m, gslit *)

