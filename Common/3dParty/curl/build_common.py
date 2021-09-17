#!/usr/bin/env python

import os  # system, getenv
import subprocess  # check_output
import sys  # path.append
sys.path.append('./../../../../build_tools/scripts')
import base  # host_platform


class SgrParameter:
    Reset = 0
    Bold = 1
    Black = 30
    Red = 31
    Green = 32
    Yellow = 33
    Blue = 34
    Magenta = 35
    Cyan = 36
    White = 37


def get_escape_str(code):
    csi = '\x1B['
    end_symbol = 'm'
    return csi + str(code) + end_symbol


def print_with_codes(text, escape_codes):
    s = ''
    for code in escape_codes:
        s += get_escape_str(code)
    s += text
    s += get_escape_str(SgrParameter.Reset)
    if 'windows' == base.host_platform():
        os.system('')  # some bug that makes windows cmd understand escape codes
    print(s)


def log_info(text):
    print_with_codes(text, [SgrParameter.Yellow])


def log_warning(text):
    print_with_codes(text, [SgrParameter.Yellow, SgrParameter.Bold])


def log_error(text):
    print_with_codes(text, [SgrParameter.Red, SgrParameter.Bold])


def get_command_output(command, stderr=None):
    try:
        return subprocess.check_output(command, stderr=stderr)
    except subprocess.CalledProcessError:
        return None


def get_cpu_count():
    result = None
    if 'windows' == base.host_platform():
        result = os.getenv('NUMBER_OF_PROCESSORS')
    if 'mac' == base.host_platform():
        result = get_command_output(['sysctl', '-n', 'hw.physicalcpu'])
    if 'linux' == base.host_platform():
        result = get_command_output(['nproc'])
    if result:
        return int(result)
    return None


def set_pkg_config_path():
    if 'windows' == base.host_platform():
        command = 'where'
    else:
        command = 'which'
    result = get_command_output(
        [command, 'pkg-config']
        , stderr=open(os.devnull, 'w')
    )
    if not result:
        return False
    os.environ['PKG_CONFIG_PATH'] = result
    return True


def remove_dirs(dirs, stderr=None, stdout=None):
    command = ['rd', '/s', '/q'] if 'windows' == base.host_platform() else ['rm', '-rf']
    command += dirs
    try:
        subprocess.call(command, stderr=stderr, stdout=stdout)
        return True
    except Exception as e:
        print ('remove_dirs: exception %s' % str(e))
        return False


def mkdir_p(dir, stderr=None, stdout=None):
    command = 'md ' if 'windows' == base.host_platform() else 'mkdir -p '
    command += dir
    os.system(command)
    return


def silently_remove_dir_if_exist(dir):
    if os.path.isdir(dir):
        remove_dirs(dirs=[dir], stderr=open(os.devnull, 'w'), stdout=open(os.devnull, 'w'))
    return
