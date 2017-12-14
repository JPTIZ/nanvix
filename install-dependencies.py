#!/usr/bin/env python3
import platform
import subprocess
import shlex
from typing import NamedTuple, Dict
from subprocess import PIPE

PACKAGES = {
    'g++': 'g++',
    'doxygen': 'doxygen',
    'genisoimage': 'genisoimage',
    'gdb': 'gdb'
}


class Distro(NamedTuple):
    manager: str
    command: str
    packages: Dict[str, str] = PACKAGES


PACMANS = {
    'arch': Distro(
        manager = 'pacman',
        command = '-S --noconfirm',
    ),
    'ubuntu': Distro(
        manager = 'apt-get',
        command = 'install -y'
    ),
}


def run_cmd(arglist, noprint=False):
    if not noprint:
        print(' '.join(arglist))
    subprocess.check_call(args=arglist, stdout=PIPE)


def install(manager, command, packages):
    run_cmd(shlex.split(f'{manager} {command} {packages}'))


if __name__ == '__main__':
    distro, version, identifier = plaftorm.linux_distribution()
    pacman = PACMANS[distro]

    install(pacman.manager, pacman.command, pacman.packages)

    apt-get install -y g++ doxygen genisoimage gdb
