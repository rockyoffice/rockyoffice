#!/usr/bin/env python

import os
import sys
import build_common
sys.path.append('./../../../../build_tools/scripts')
import base


def make():
    tools_root = base.get_path(os.getcwd())
    pwd_path = base.get_path(base.get_script_dir(__file__))
    os.chdir(pwd_path)
    print('pwd_path = ' + pwd_path)
    print('tools_root = ' + tools_root)

    lib_version = 'curl-7_68_0'
    lib_name = 'curl-7.68.0'
    lib_dest_dir = pwd_path + base.get_path('/build/android/curl-universal')

    download_url = 'https://github.com/curl/curl/releases/download/' + lib_version + '/' + lib_name + '.tar.gz'
    print(download_url)

    # 'https://curl.haxx.se/download/' + lib_name + '.tar.gz'
    # https://github.com/curl/curl/releases/download/curl-7_69_0/curl-7.69.0.tar.gz
    # https://github.com/curl/curl/releases/download/curl-7_68_0/curl-7.68.0.tar.gz


make()
