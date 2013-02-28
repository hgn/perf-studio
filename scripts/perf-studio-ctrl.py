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
import pwd
import collections


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
__email__    = "hagen@jauu.net"


CONFIG_HOME = os.path.join(xdg.BaseDirectory.xdg_config_home, "perf-studio")
CONFIG_CONF = os.path.join(CONFIG_HOME, "config")

CACHE_HOME   = os.path.join(xdg.BaseDirectory.xdg_cache_home, "perf-studio")


class StdinReader:

    @staticmethod
    def yes_no(outf, prompt, default='no'):
        v = ('[y/N]', False) if default == 'no' else ('[Y/n]', True)
        outf('{} {}'.format(prompt, v[0]))
        for line in sys.stdin:
            try:
                line = line.rstrip()
                if 'y' == line: return True
                elif 'n' == line: return False
                outf('{} {}'.format(prompt, v[0]))
            except:
                pass


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
        self.parser = argparse.ArgumentParser(description='Create/modify/show perf-studio projects')
        self.parser.add_argument('-v', '--verbose', help='Verbose output', action="store_true")
        self.parser.add_argument('-c', '--create', help='Create project', action="store_true")
        self.parser.add_argument('-l', '--list', help='List all available projects', action="store_true")
        self.parser.add_argument('-s', '--set', help='Set projects options', action="store_true")
        self.parser.add_argument('args', nargs=argparse.REMAINDER)
        self.args = self.parser.parse_args(sys.argv[2:])
        self.logger.setLevel(logging.DEBUG) if self.args.verbose else None

        if not self.args.list and not self.args.create:
            self.parser.print_help()
            self.logger.error("")
            self.logger.error("create/list/set option missing")
            sys.exit(1)


    def list_projects(self):
        self.logger.info("Project path: {0}".format(CACHE_HOME))
        self.logger.warning("Registered projects:")
        try:
            for fn in os.listdir(CACHE_HOME):
                if not os.path.isdir(fn):
                    next
                self.logger.warning("Project ID {0}".format(fn))
        except OSError:
            self.logger.warning("No project available!")


    def create_project_conf(self, project_path):
        conf_path = os.path.join(project_path, "conf")
        self.logger.warning("Write config file to: {}".format(conf_path))

        config = configparser.ConfigParser()
        for argument in self.args.args:
            (key, valval) = argument.split('=')
            (group, val) = key.split('.')
            if not group in config:
                config[group] = {}
            config[group]["  {}".format(val)] = valval

        with open(conf_path, 'w') as configfile:
            config.write(configfile)

    def create_project_dir_structure(self, project_path):
        dir_path = os.path.join(project_path, "refs")
        self.logger.warning("Create REFS directory: {}".format(dir_path))
        os.mkdir(dir_path)

        dir_path = os.path.join(project_path, "db")
        self.logger.warning("Create DB   directory: {}".format(dir_path))
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
            assert(i < 9999)
        self.logger.debug("project_dirs {}, new pid {}".format(project_dirs, pid))
        return pid


    def create_project(self):
        self.logger.warning("Create new project in {}".format(CACHE_HOME))
        pid = self.pick_free_id()
        self.logger.warning("New project ID: {}".format(pid))
        project_path = os.path.join(CACHE_HOME, pid)
        self.logger.warning("Create project path: {}".format(project_path))
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

        self.parser.print_help()
        self.logger.error("")
        self.logger.error("create/list/set option missing to project")


class ConfigCmd(Command):

    def initialize(self):
        self.parser = argparse.ArgumentParser(description='Process some integers.')
        self.parser.add_argument('-c', '--create', help='Create configuration', action="store_true")
        self.parser.add_argument('-l', '--list', help='List current configuration', action="store_true")
        self.parser.add_argument('-a', '--add', help='Add configuration value (common.username=fooo)', action="store_true")
        self.parser.add_argument('-v', '--verbose', help='verbose output', action="store_true")
        self.parser.add_argument('args', nargs=argparse.REMAINDER)
        self.args = self.parser.parse_args(sys.argv[2:])
        self.logger.setLevel(logging.DEBUG) if self.args.verbose else None
        if not self.args.list and not self.args.create and not self.args.add:
            self.parser.print_help()
            self.logger.error("")
            self.logger.error("create/list/set option missing")
            sys.exit(1)

    def get_username(self):
        return pwd.getpwuid(os.getuid())[4]


    def conf_template(self):
        root = collections.OrderedDict()
        root['common'] = collections.OrderedDict()
        root['common']['perf-exec-path'] = 'perf'
        root['common']['username'] = self.get_username()
        root['projects'] = collections.OrderedDict()
        root['projects']['max-perf-data-per_project'] = '1GiB'
        return root


    def args_config(self):
        conf = collections.OrderedDict()
        for argument in self.args.args:
            (key, valval) = argument.split('=')
            (group, val) = key.split('.')
            if not group in conf:
                conf[group] = collections.OrderedDict()
            conf[group]["%s" % (val)] = valval
        return conf



    def diff_config(self, new_conf):
        """ return false if no difference, true if not identical"""
        self.logger.debug("Configuration file already exists ({})".format(CONFIG_CONF))

        foo = tempfile.TemporaryFile()
        foo_path = tempfile.mktemp()
        with open(foo_path, 'w') as foo:
            new_conf.write(foo)

        foo = open(foo_path, 'r')
        foolines = foo.readlines()

        bar = open(CONFIG_CONF, 'r')
        barlines = bar.readlines()

        m = difflib.SequenceMatcher(None, barlines, foolines)
        difference = 100.0 - (m.ratio() * 100.0)
        if difference == 0.0:
            return False

        self.logger.warning("Configuration differs! (ratio {0:.1f} %)".format(difference))

        d = difflib.Differ()
        diff = difflib.unified_diff(barlines, foolines)
        for i in diff:
            self.logger.warning("%s" % (i.rstrip()))

        foo.close()
        os.remove(foo_path)
        bar.close()

        return True


    def write_config(self, config):
        if not os.path.isdir(CONFIG_HOME):
            self.logger.warning("Create perf-studio configuration directory %s" % (CONFIG_HOME))
            os.mkdir(CONFIG_HOME)

        self.logger.warning("Write new configuration to \"%s\"" % (CONFIG_CONF))

        for section in config.sections():
            self.logger.info(" [%s]" % (section))
            for option in config.options(section):
                self.logger.info("  %s = %s" % (option, config.get(section, option)))

        with open(CONFIG_CONF, 'w') as configfile:
            config.write(configfile)

    def create_config_parser(self, data):
        new_config = configparser.ConfigParser()
        for group in data.keys():
            for key in data[group].keys():
                if not group in new_config:
                    new_config[group] = {}
                new_config[group]["  {}".format(key)] = data[group][key]
        return new_config

    def merge_conf_dicts(self, dicta, dictb):
        data = collections.OrderedDict()

        # merge keys from dicta and dictb into new dict
        for key in dicta:
            if not key in data:
                data[key] = collections.OrderedDict()
            if key in dictb:
                # merge dictionaries
                data[key] = dict(list(dicta[key].items()) + list(dictb[key].items()))
            else:
                # merge not required because dictb has no identical key
                # simple copy the data into new dict
                data[key] = dicta[key]

        # add remaining keys from dictb into new dict
        for key in dictb:
            if not key in data:
                data[key] = collections.OrderedDict()
            if not key in dictb:
                data[key] = dictb[key]

        return data


    def create_configuration(self):
        template = self.conf_template()
        argsdata = self.args_config()

        # merge dictionaries, argsdata dict will overwrite template dict
        data = self.merge_conf_dicts(template, argsdata)

        new_config = self.create_config_parser(data)

        if not os.path.exists(CONFIG_CONF):
            self.logger.warning("Write configuration file: {0}".format(CONFIG_CONF))
            self.write_config(new_config)
            return

        configuration_difference = self.diff_config(new_config)
        if configuration_difference:
            answer = StdinReader.yes_no(self.logger.warning, "Overwrite file?", default='no')
            if answer == False:
                self.logger.warning("Aborted - configuration file untouched")
                return
            self.write_config(new_config)
        else:
            self.logger.warning("Configuration identical, no change, no write")
            self.logger.warning("Configuration file: {0}".format(CONFIG_CONF))


    def run(self):
        if self.args.create:
            self.create_configuration()
            return



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
        self.logger.warning("{0} (C)".format(__programm__))
        classinstance.run()
        return 0


if __name__ == "__main__":
    try:
        psc = PerfStudioControl()
        sys.exit(psc.run())
    except KeyboardInterrupt:
        sys.stderr.write("SIGINT received, exiting\n")
