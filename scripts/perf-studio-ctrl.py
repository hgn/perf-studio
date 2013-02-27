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


__programm__ = "perf-studio-control"
__author__   = "Hagen Paul Pfeifer"
__version__  = "0.1"
__license__  = "GPLv3"


CONFIG_HOME = os.path.join(xdg.BaseDirectory.xdg_config_home, "perf-studio")
CONFIG_CONF = os.path.join(CONFIG_HOME, "config")

DATA_HOME   = os.path.join(xdg.BaseDirectory.xdg_data_home, "perf-studio")


class Command:

    def __init__(self):
        pass

    def _register_logger(self, logger):
        self.logger = logger

    def initialize(self):
        pass

    def run(self):
        pass


class CreateConfigCmd(Command):

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


class CreateProjectCmd(Command):
    pass



class PerfStudioControl:

    modes = {
            "create-config":   [ "CreateConfigCmd",  "Create use globale configure file" ],
            "create-project":  [ "CreateProjectCmd", "Create a new perf-studio project" ]
            }

    def __init__(self):
        ch = logging.StreamHandler()
        formatter = logging.Formatter("# %(message)s")
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
