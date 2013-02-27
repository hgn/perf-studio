#!/usr/bin/env python3

import sys
import os
import logging
import argparse
import math
import pprint
import time
import datetime
import subprocess
import configparser
import xdg.BaseDirectory
import tempfile
import difflib


# Required packages:
#
# Fedora/RedHat
#   python3-pyxdg
#
# Debian 7.0
#   Add line in /etc/apt/sources.list:
#     deb http://ftp.debian.org/debian experimental main
#   apt-get install python3
#   apt-get -t experimental install python3-xdg


__programm__ = "perf-studio-control"
__author__   = "Hagen Paul Pfeifer"
__version__  = "0.1"
__license__  = "GPLv3"


CONFIG_HOME = os.path.join(xdg.BaseDirectory.xdg_config_home, "perf-studio")
CONFIG_CONF = os.path.join(CONFIG_HOME, "config")

CACHE_HOME   = os.path.join(xdg.BaseDirectory.xdg_cache_home, "perf-studio")


class Command:

    def __init__(self):
        pass

    def _register_logger(self, logger):
        self.logger = logger

    def initialize(self):
        pass

    def run(self):
        pass


class ProjectCmd(Command):

    def initialize(self):
        parser = argparse.ArgumentParser(description='Process some integers.')
        parser.add_argument('-c', '--create', help='Create project', action="store_true")
        parser.add_argument('-l', '--list', help='list all available projects', action="store_true")
        parser.add_argument('args', nargs=argparse.REMAINDER)
        self.args = parser.parse_args(sys.argv[2:])


    def list_projects(self):
        self.logger.warning("list all projects in %s" % (CACHE_HOME))
        for root, dirs, files in os.walk(CACHE_HOME):
            self.logger.warning("%s %s %s" % (root, dirs, files))


    def create_project_conf(self, project_path):
        conf_path = os.path.join(project_path, "conf")
        self.logger.warning("Write config file to: %s" % (conf_path))

        config = configparser.ConfigParser()
        for argument in self.args.args:
            (key, valval) = argument.split('=')
            (group, val) = key.split('.')
            if not group in config:
                config[group] = {}
            config[group]["\t%s" % (val)] = valval

        with open(conf_path, 'w') as configfile:
            config.write(configfile)

    def create_project_dir_structure(self, project_path):
        dir_path = os.path.join(project_path, "refs")
        self.logger.warning("Create REFS directory: %s" % (dir_path))
        os.mkdir(dir_path)

        dir_path = os.path.join(project_path, "db")
        self.logger.warning("Create DB   directory: %s" % (dir_path))
        os.mkdir(dir_path)


    def pick_free_id(self):
        project_dirs = list()
        for fn in os.listdir(CACHE_HOME):
            if not os.path.isdir(fn):
                next
            project_dirs.append(fn)
        project_dirs.sort()

        i = 1
        while True:
            pid = "%04d" % (i)
            if not pid in project_dirs:
                break
            i += 1
        self.logger.debug("project_dirs %s, new pid %s" % (project_dirs, pid))
        return pid


    def create_project(self):
        self.logger.warning("Create new project in %s" % (CACHE_HOME))
        pid = self.pick_free_id()
        self.logger.warning("New project ID: %s" % (pid))
        project_path = os.path.join(CACHE_HOME, pid)
        self.logger.warning("Create project path: %s" % (project_path))
        os.mkdir(project_path)
        self.create_project_conf(project_path)
        self.create_project_dir_structure(project_path)



    def run(self):
        if self.args.list:
            self.list_projects()
            return
        if self.args.create:
            self.create_project()
            return

        self.logger.error("create/list/set missing to project, see --help")



class ConfigCmd(Command):

    def initialize(self):
        parser = argparse.ArgumentParser(description='Process some integers.')
        parser.add_argument('-n', '--name', help='username', type=str, dest='username')
        parser.add_argument('-v', '--verbose', help='verbose output', action="store_true")
        self.args = parser.parse_args(sys.argv[2:])


    def create_config(self):
        self.config = configparser.ConfigParser()

        common = dict()
        common['username']       = self.args.username
        common['perf-exec-path'] = '/usr/src/linux/tools/perf/perf' 
        self.config['common'] = common

        projects = dict()
        projects['max-perf-data-per_project'] = '1GiB'
        self.config['projects'] = projects


    def diff(self):
        if not os.path.exists(CONFIG_CONF):
            return

        self.logger.warning("configuration file already exists, will analyse difference")

        foo = tempfile.TemporaryFile()
        foo_path = tempfile.mktemp()
        with open(foo_path, 'w') as foo:
            self.config.write(foo)

        foo = open(foo_path, 'r')
        foolines = foo.readlines()

        bar = open(CONFIG_CONF, 'r')
        barlines = bar.readlines()

        m = difflib.SequenceMatcher(None, barlines, foolines)
        difference = 100.0 - (m.ratio() * 100.0)
        self.logger.warning("change ratio: %.2f%%" % (difference))
        if difference == 0.0:
            self.logger.warning("files identical")

        d = difflib.Differ()
        diff = difflib.unified_diff(barlines, foolines)
        for i in diff:
            self.logger.warning("%s" % (i.rstrip()))

        foo.close()
        os.remove(foo_path)

        bar.close()


    def write_config(self):
        if not os.path.isdir(CONFIG_HOME):
            self.logger.warning("create perf-studio configuration directory %s" % (CONFIG_HOME))
            os.mkdir(CONFIG_HOME)

        self.diff()

        self.logger.warning("write configuration file \"%s\"" % (CONFIG_CONF))
        for section in self.config.sections():
            self.logger.warning(" [%s]" % (section))
            for option in self.config.options(section):
                self.logger.warning("\t%s = %s" % (option, self.config.get(section, option)))

        with open(CONFIG_CONF, 'w') as configfile:
            self.config.write(configfile)


    def run(self):
        self.create_config()
        self.write_config()



class PerfStudioControl:

    modes = {
            "config":   [ "ConfigCmd",  "Create use globale configure file" ],
            "project":  [ "ProjectCmd", "Create a new perf-studio project" ]
            }

    def __init__(self):
        ch = logging.StreamHandler()
        formatter = logging.Formatter("%(message)s")
        ch.setFormatter(formatter)
        self.logger = logging.getLogger()
        self.logger.setLevel(logging.WARNING)
        self.logger.addHandler(ch)

    def print_version(self):
        sys.stdout.write("%s\n" % (__version__))

    def print_usage(self):
        sys.stderr.write("Usage: perf-studio-ctrl [-h | --help]" +
                         " [--version]" +
                         " <command> [<command-args>]\n")

    def print_modules(self):
        for i in PerfStudioControl.modes.keys():
            sys.stderr.write("   %-15s - %s\n" % (i, PerfStudioControl.modes[i][1]))

    def parse_global_otions(self):
        if len(sys.argv) <= 1:
            self.print_usage()
            sys.stderr.write("Available commands:\n")
            self.print_modules()
            return None

        submodule = sys.argv[1].lower()

        if submodule == "--version":
            self.print_version()
            return None

        if submodule == "-h" or submodule == "--help":
            self.print_usage()
            sys.stderr.write("Available commands:\n")
            self.print_modules()
            return None

        if submodule not in PerfStudioControl.modes:
            self.print_usage()
            sys.stderr.write("Module \"%s\" not known, available modules are:\n" %
                             (submodule))
            self.print_modules()
            return None

        classname = PerfStudioControl.modes[submodule][0]
        return classname

    def run(self):
        classtring = self.parse_global_otions()
        if not classtring:
            return 1

        classinstance = globals()[classtring]()
        classinstance._register_logger(self.logger)
        classinstance.initialize()
        classinstance.run()
        return 0


if __name__ == "__main__":
    try:
        psc = PerfStudioControl()
        sys.exit(psc.run())
    except KeyboardInterrupt:
        sys.stderr.write("SIGINT received, exiting\n")
