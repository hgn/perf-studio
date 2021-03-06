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
import re
import glob



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

PROJECTS_DIR   = os.path.join(xdg.BaseDirectory.xdg_cache_home, "perf-studio/projects")


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
        self.parser.add_argument('-c', '--create', help='Create project', action="store_true")
        self.parser.add_argument('-l', '--list', help='List all available projects', action="store_true")
        self.parser.add_argument('-s', '--show', help='Show project directory structure', action="store_true")
        self.parser.add_argument('-e', '--edit', help='Spawn editor to edit project configuration', action="store_true")
        self.parser.add_argument('-v', '--verbose', help='Verbose output', action="store_true")
        self.parser.add_argument('args', nargs=argparse.REMAINDER)
        self.args = self.parser.parse_args(sys.argv[2:])
        self.logger.setLevel(logging.DEBUG) if self.args.verbose else None

        if not self.args.list and not self.args.create and not self.args.edit and not self.args.show:
            self.parser.print_help()
            self.logger.error("")
            self.logger.error("create/list/set option missing")
            sys.exit(1)


    def list_projects(self):
        self.logger.info("Project path: {0}".format(PROJECTS_DIR))
        self.logger.warning("Registered projects:")
        self.logger.warning("")
        try:
            for fn in os.listdir(PROJECTS_DIR):
                if not os.path.isdir(os.path.join(PROJECTS_DIR, fn)):
                    continue
                self.logger.warning("Project ID {0}".format(fn))
        except OSError:
            self.logger.warning("No project available!")


    def set_common_defaults(self, config):
        config['common'] = {}
        config['common']['cmd'] = '/usr/bin/gcc'
        config['common']['cmd-args'] = ''
        config['common']['description'] = 'Unnamed'

        config['stats'] = {}
        config['stats']['created'] = str(int(time.time() * 1000000))


    def create_project_conf(self, project_path):
        conf_path = os.path.join(project_path, "config")
        self.logger.warning("Write config file to: {}".format(conf_path))

        config = configparser.ConfigParser()
        self.set_common_defaults(config)

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
        for fn in os.listdir(PROJECTS_DIR):
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

    def which(self, cmd):
        def is_exe(fpath):
            return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

        fpath, fname = os.path.split(cmd)
        if fpath:
            if is_exe(cmd):
                return cmd
        else:
            for path in os.environ["PATH"].split(os.pathsep):
                path = path.strip('"')
                exe_file = os.path.join(path, cmd)
                if is_exe(exe_file):
                    return exe_file
        return None


    def show_dir(self, dir, prefix, path):
        items = glob.glob(dir + '/*')
        for name in items:
            basename = name[len(dir)+1:]
            if name == items[0]:
                newpath = path+ '`-' +basename
            else:
                newpath = prefix + '`-' +basename

            if name == items[-1]:
                newprefix = prefix + '   ' + ' '*len(basename)
            else:
                newprefix = prefix + '|  ' + ' '*len(basename)

            if glob.glob('%s/*' % name):
                self.show_dir(name, newprefix, newpath + '-')

            else:
                print(newpath)


    def show_projects(self):
        self.logger.warning("Show all project under {}".format(PROJECTS_DIR))
        if self.which("tree"):
            os.system("tree -n {}".format(PROJECTS_DIR))
        else:
            self.show_dir(PROJECTS_DIR, '','');


    def create_project(self):
        self.logger.warning("Create new project in {}".format(PROJECTS_DIR))
        if not os.path.exists(PROJECTS_DIR):
            os.makedirs(PROJECTS_DIR)
            with open(os.path.join(PROJECTS_DIR, "README"), 'w') as readme:
                    readme.write("Here are dragons - edit with care!\n")
        pid = self.pick_free_id()
        self.logger.warning("New project ID: {}".format(pid))
        project_path = os.path.join(PROJECTS_DIR, pid)
        self.logger.warning("Create project path: {}".format(project_path))
        os.mkdir(project_path)
        self.create_project_conf(project_path)
        self.create_project_dir_structure(project_path)


    def edit_project_conf(self):
        if not self.args.args:
            self.logger.error("--edit requires a project-id (e.g. 0001), see --list")
            sys.exit(1)
        for argument in self.args.args:
            project_path      = os.path.join(PROJECTS_DIR, argument)
            project_conf_path = os.path.join(project_path, "config")
            if not os.path.exists(project_path):
                self.logger.error("Project {0} do not exist ({1})!".format(argument, project_path))
                sys.exit(1)
            if not os.path.exists(project_conf_path):
                self.logger.error("Project {0} do not exist - strange!".format(project_conf_path))
                sys.exit(1)
            editor = os.environ.get('EDITOR','vi')
            self.logger.warning("spawn editor to edit project configuration")
            self.logger.info("{} {}".format(editor, project_conf_path))
            subprocess.call([editor, project_conf_path])



    def run(self):
        if self.args.list:
            self.list_projects()
            return
        if self.args.create:
            self.create_project()
            return
        if self.args.edit:
            self.edit_project_conf()
            return
        if self.args.show:
            self.show_projects()
            return

        self.parser.print_help()
        self.logger.error("")
        self.logger.error("create/list/set option missing to project")


class ConfigCmd(Command):

    def initialize(self):
        self.parser = argparse.ArgumentParser(description='Process some integers.')
        self.parser.add_argument('-c', '--create', help='Create configuration', action="store_true")
        self.parser.add_argument('-l', '--list', help='List current configuration', action="store_true")
        self.parser.add_argument('-s', '--set', help='set configuration value (common.username "John Doo")', action="store_true")
        self.parser.add_argument('-e', '--edit', help='Edit user configuration', action="store_true")
        self.parser.add_argument('-v', '--verbose', help='verbose output', action="store_true")
        self.parser.add_argument('args', nargs=argparse.REMAINDER)
        self.args = self.parser.parse_args(sys.argv[2:])
        self.logger.setLevel(logging.DEBUG) if self.args.verbose else None
        if not self.args.list and not self.args.create and not self.args.set and not self.args.edit:
            self.parser.print_help()
            self.logger.error("")
            self.logger.error("create/list/set/edit option missing")
            sys.exit(1)


    def get_username(self):
        s = pwd.getpwuid(os.getuid())[4]
        return re.sub(r'[,.:;]', '', s)


    def conf_template(self):
        root = collections.OrderedDict()
        root['common'] = collections.OrderedDict()
        root['common']['perf-path'] = 'perf'
        root['common']['username'] = self.get_username()
        root['projects'] = collections.OrderedDict()
        root['projects']['max-perf-data-per-project'] = '1GiB'
        root['module-conf'] = collections.OrderedDict()
        root['module-conf']['show-experimental-modules'] = 'true'
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

    def edit_user_conf(self):
        if not os.path.exists(CONFIG_CONF):
            self.logger.error("Configuration file {0} do not exist ({1})!".format(CONFIG_CONF))
            sys.exit(1)
        editor = os.environ.get('EDITOR','vi')
        self.logger.warning("Open editor to edit configuration ({})".format(CONFIG_CONF))
        self.logger.info("{} {}".format(editor, CONFIG_CONF))
        subprocess.call([editor, CONFIG_CONF])


    def set_user_conf(self):
        if not os.path.exists(CONFIG_CONF):
            self.logger.error("Configuration file {0} do not exist ({1})!".format(CONFIG_CONF))
            sys.exit(1)
        if len(self.args.args) != 2:
            self.logger.error("Error: expect key value string! common.username \"John Doo\"")
            sys.exit(1)
        (group_key, value) = self.args.args
        self.logger.info("Key:   {} ".format(group_key))
        self.logger.info("Value: {} ".format(value))

        try:
            (group, key) = group_key.split('.')
        except ValueError:
            self.logger.error("Error: expect key value string! common.username \"John Doo\"")
            sys.exit(1)

        self.logger.warn("Set \"{}\" to \"{}\"".format(group_key, value))

        config_parser = configparser.ConfigParser()
        config_parser.read(CONFIG_CONF)
        config_parser.set(group, key, value)

        conf_fd = open(CONFIG_CONF,'w')
        config_parser.write(conf_fd)
        conf_fd.close()


    def run(self):
        if self.args.create:
            self.create_configuration()
            return
        if self.args.edit:
            self.edit_user_conf()
            return
        if self.args.set:
            self.set_user_conf()
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
